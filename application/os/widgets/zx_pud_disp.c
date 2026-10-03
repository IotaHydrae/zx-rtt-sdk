/*
 * PUD display path: decode a band and put it on the panel.
 *
 * The decode deliberately does not run in the EP1 completion callback.  The
 * RP2350 firmware reached the same conclusion in decoder_internal.h:
 *
 *   "JPEG decoding and the TFT flush must NOT run on the USB interrupt stack:
 *    ... holding the USB IRQ for tens of milliseconds wedges the USB
 *    controller and corrupts the interrupt stack.  The USB ISR therefore just
 *    copies the received frame into a slot and wakes a dedicated decoder task."
 *
 * That matches what this board measured directly: doing the NAND burn in the
 * callback cost 20.34 -> 1.37 MB/s, and running msh_exec there made the control
 * channel time out.  So the callback copies into a slot, and this task decodes.
 *
 * The panel is driven by the SoC's framebuffer layer, so "flush a band" is:
 * write pixels, clean the cache, pan.  The four steps come from the LVGL
 * port's fbdev_flush().
 */
#include <rtthread.h>
#include <aic_core.h>
#include "zx_fb.h"
#include "mpp_fb.h"
#include "msh.h"
#include "pud_qoi.h"

void zxring_puts(const char *s);

/* (65536 - 12 - 16) / 3 + 1, the same bound the RP2350 build uses: one band
 * can hold this many decoded pixels, which is what a band-sized rect needs. */
#define ZX_BAND_MAX_PIXELS 21837
/* A band's *compressed* payload must fit one EP1 transfer: frame_max
 * minus the 12-byte header.  Uncompressed output is the bound above. */
/*
 * One band's compressed payload: the protocol requires 12 + payload <= the
 * frame_max the device reports in its capabilities, and this device reports
 * 65536 (pud_vendor.h's PUD_FRAME_MAX).  Spelled out here rather than included
 * because the application does not depend on the protocol header -- the same
 * boundary the flash channel keeps.  If PUD_FRAME_MAX changes, this changes
 * with it; the two are a documented pair.
 */
#define ZX_BAND_MAX_PAYLOAD 65524

struct zx_band {
    rt_uint16_t xs, ys, xe, ye;
    rt_uint32_t len;
    rt_uint8_t payload[ZX_BAND_MAX_PAYLOAD];
};

static struct zx_band zx_band;
static volatile int zx_band_ready;
static struct rt_thread zx_disp_thread;
static char zx_disp_stack[8192];

/* Framebuffer geometry, read once at startup and printed into the log ring so
 * it is checkable without a console. */
static struct mpp_fb *zx_fb;
static struct aicfb_screeninfo zx_fb_info;
static rt_uint16_t zx_fb_buf[ZX_BAND_MAX_PIXELS];
static volatile rt_uint32_t zx_disp_submitted;
static volatile rt_uint32_t zx_disp_drawn;
static volatile rt_uint32_t zx_disp_dropped;
static volatile rt_uint32_t zx_disp_decode_failed;

static int zx_fb_open(void)
{
    char msg[128];

    zx_fb = mpp_fb_open();
    if (!zx_fb)
        return -1;
    if (mpp_fb_ioctl(zx_fb, AICFB_GET_SCREENINFO, &zx_fb_info))
        return -1;

    /* width x stride is the layout with AICFB_ROTATE_90: width is the row
     * count and stride/2 is the pixels per row.  height x stride is not. */
    rt_snprintf(msg, sizeof(msg),
                "[zxdisp] fb %ux%u stride %u bpp %u smem %u\n",
                (unsigned)zx_fb_info.width, (unsigned)zx_fb_info.height,
                (unsigned)zx_fb_info.stride, (unsigned)zx_fb_info.bits_per_pixel,
                (unsigned)zx_fb_info.smem_len);
    zxring_puts(msg);
    return 0;
}

/* Copy a decoded rect into the framebuffer.  Rows are `stride` bytes apart and
 * hold pixels_per_row pixels -- not `width`, which is the row count here. */
static void zx_fb_blit(rt_uint16_t xs, rt_uint16_t ys, rt_uint16_t xe,
                       rt_uint16_t ye, const rt_uint16_t *px)
{
    rt_uint16_t *base = (rt_uint16_t *)zx_fb_info.framebuffer;
    rt_uint16_t w = xe - xs + 1;
    rt_uint32_t y;

    for (y = ys; y <= ye; y++) {
        rt_uint16_t *row = (rt_uint16_t *)((rt_uint8_t *)base + y * zx_fb_info.stride);

        rt_memcpy(row + xs, px, w * 2);
        px += w;
    }

    aicos_dcache_clean_invalid_range((ulong *)base,
                                     (ulong)ALIGN_UP(zx_fb_info.smem_len, CACHE_LINE_SIZE));
    {
        int index = 0;

        mpp_fb_ioctl(zx_fb, AICFB_PAN_DISPLAY, &index);
        mpp_fb_ioctl(zx_fb, AICFB_POWERON, 0);
        mpp_fb_ioctl(zx_fb, AICFB_WAIT_FOR_VSYNC, 0);
    }
}

static void zx_disp_task(void *arg)
{
    (void)arg;

    for (;;) {
        if (zx_band_ready) {
            rt_uint16_t xs, ys, xe, ye;
            rt_uint32_t len;
            size_t got;

            /* Keep the single slot occupied while decoding; this bounds the
             * callback work and avoids copying 64 KiB onto the task stack. */
            xs = zx_band.xs;
            ys = zx_band.ys;
            xe = zx_band.xe;
            ye = zx_band.ye;
            len = zx_band.len;

            got = rgb565_qoi_decompress(zx_band.payload, len, zx_fb_buf,
                                        ZX_BAND_MAX_PIXELS);
            if (got == (size_t)(xe - xs + 1) *
                         (size_t)(ye - ys + 1)) {
                zx_fb_blit(xs, ys, xe, ye, zx_fb_buf);
                zx_disp_drawn++;
            } else {
                zx_disp_decode_failed++;
                zxring_puts("[zxdisp] qoi decode failed\n");
            }
            zx_band_ready = 0;
        }
        rt_thread_mdelay(5);
    }
}

/* Called from the EP1 completion callback: copy and hand over, nothing else. */
void zx_pud_disp_submit(rt_uint16_t xs, rt_uint16_t ys, rt_uint16_t xe,
                        rt_uint16_t ye, const rt_uint8_t *payload, rt_uint32_t len)
{
    rt_uint32_t width;
    rt_uint32_t height;

    if (xe < xs || ye < ys ||
        xe >= zx_fb_info.height || ye >= zx_fb_info.width) {
        zx_disp_dropped++;
        return;
    }

    width = (rt_uint32_t)xe - xs + 1;
    height = (rt_uint32_t)ye - ys + 1;
    if (width * height > ZX_BAND_MAX_PIXELS) {
        zx_disp_dropped++;
        return;
    }
    if (zx_band_ready || len > sizeof(zx_band.payload)) {
        zx_disp_dropped++;
        return;
    }
    zx_band.xs = xs; zx_band.ys = ys; zx_band.xe = xe; zx_band.ye = ye;
    zx_band.len = len;
    rt_memcpy(zx_band.payload, payload, len);
    zx_band_ready = 1;
    zx_disp_submitted++;
}

static int zx_disp_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    rt_kprintf("submitted %u drawn %u dropped %u decode_failed %u\n",
               (unsigned)zx_disp_submitted, (unsigned)zx_disp_drawn,
               (unsigned)zx_disp_dropped, (unsigned)zx_disp_decode_failed);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(zx_disp_stats, zxdisp_stats, display decoder counters);

static int zx_pud_disp_init(void)
{
    char msg[96];
    rt_err_t err;

    if (zx_fb_open()) {
        zxring_puts("[zxdisp] framebuffer open failed\n");
        return -1;
    }
    err = rt_thread_init(&zx_disp_thread, "zxdisp", zx_disp_task, RT_NULL,
                         zx_disp_stack, sizeof(zx_disp_stack), 21, 10);
    if (err != RT_EOK) {
        rt_snprintf(msg, sizeof(msg), "[zxdisp] thread init failed: %d\n", err);
        zxring_puts(msg);
        return -1;
    }
    err = rt_thread_startup(&zx_disp_thread);
    rt_snprintf(msg, sizeof(msg), "[zxdisp] started (startup=%d)\n", err);
    zxring_puts(msg);
    return 0;
}
INIT_APP_EXPORT(zx_pud_disp_init);
