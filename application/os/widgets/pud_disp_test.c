/*
 * Scratch display path for the PUD port: paint known colour bars.
 *
 * The panel is driven by the SoC's framebuffer layer, not by us touching the
 * controller, so "put a frame on screen" is: write pixels, clean the cache,
 * pan.  The recipe and the double-buffer layout come from the LVGL port's
 * fbdev_flush() in packages/third-party/lvgl/lvgl-8.3.10/zx/zx_port.c.
 *
 * Bars rather than a picture on purpose: they show at a glance whether the
 * image is rotated, mirrored, or has red and blue swapped -- none of which a
 * photograph reveals.  AICFB_ROTATE_90 is set, so the answer is not obvious
 * from the geometry alone.
 */
#include <rtthread.h>
#include "aic_core.h"
#include "zx_fb.h"
#include "mpp_fb.h"

/* Left to right, the classic order: any transposition shows up as a wrong
 * colour, and a mirror shows up as a reversed order. */
static const uint16_t pud_bars[] = {
    0xF800, /* red     */
    0x07E0, /* green   */
    0x001F, /* blue    */
    0xFFE0, /* yellow  */
    0x07FF, /* cyan    */
    0xF81F, /* magenta */
    0xFFFF, /* white   */
    0x0000, /* black   */
};

#define PUD_BAR_COUNT (sizeof(pud_bars) / sizeof(pud_bars[0]))

static int pud_disp_test(void)
{
    struct mpp_fb *fb;
    struct aicfb_screeninfo info = { 0 };
    uint16_t *base;
    uint32_t fb_size, rows, pixels_per_row, y, x;
    int index = 0;

    fb = mpp_fb_open();
    if (!fb) {
        rt_kprintf("pud: mpp_fb_open failed\n");
        return -1;
    }

    rt_memset(&info, 0, sizeof(info));
    if (mpp_fb_ioctl(fb, AICFB_GET_SCREENINFO, &info)) {
        rt_kprintf("pud: AICFB_GET_SCREENINFO failed\n");
        return -1;
    }

    rt_kprintf("pud: fb %ux%u stride %u bpp %u smem %u buf %p\n",
               (unsigned)info.width, (unsigned)info.height,
               (unsigned)info.stride, (unsigned)info.bits_per_pixel,
               (unsigned)info.smem_len, (void *)info.framebuffer);

    if (info.bits_per_pixel != 16) {
        rt_kprintf("pud: expected RGB565 (16 bpp), got %u -- not painting\n",
                   (unsigned)info.bits_per_pixel);
        return -1;
    }

    /*
     * With AICFB_ROTATE_90 the names read backwards and the arithmetic is the
     * only reliable guide:
     *
     *     width  x stride =  480 x 1600 = 768000 = smem_len   <- the layout
     *     height x stride =  800 x 1600 = 1280000 != smem_len <- not the layout
     *
     * So the buffer holds width (480) rows of stride (1600) bytes each, i.e.
     * 800 pixels per row -- and `height` is the panel's dimension, not the
     * buffer's.  Using height rows (what the first version did, copied from the
     * LVGL port's fbdev_flush) writes 1.28 MB into a 768 KB buffer.
     */
    rows = info.width;
    pixels_per_row = info.stride / 2;
    fb_size = info.smem_len;
    base = (uint16_t *)info.framebuffer;

    if ((uint32_t)info.width * info.stride != info.smem_len) {
        rt_kprintf("pud: layout does not add up: %u x %u != %u -- not painting\n",
                   (unsigned)info.width, (unsigned)info.stride,
                   (unsigned)info.smem_len);
        return -1;
    }

    /* Vertical bars in buffer coordinates.  Whether the glass shows them
     * vertically or horizontally is exactly what the visual check settles. */
    for (y = 0; y < rows; y++) {
        uint16_t *row = (uint16_t *)((uint8_t *)base + y * info.stride);

        for (x = 0; x < pixels_per_row; x++)
            row[x] = pud_bars[(x * PUD_BAR_COUNT) / pixels_per_row];
    }

    /* The display engine reads this through DMA, so the cache clean is not
     * optional: without it the panel shows whatever was in memory before. */
    aicos_dcache_clean_invalid_range((ulong *)base,
                                     (ulong)ALIGN_UP(fb_size, CACHE_LINE_SIZE));

    if (mpp_fb_ioctl(fb, AICFB_PAN_DISPLAY, &index))
        rt_kprintf("pud: AICFB_PAN_DISPLAY failed\n");

    if (mpp_fb_ioctl(fb, AICFB_POWERON, 0))
        rt_kprintf("pud: AICFB_POWERON failed (may already be on)\n");

    if (mpp_fb_ioctl(fb, AICFB_WAIT_FOR_VSYNC, 0))
        rt_kprintf("pud: AICFB_WAIT_FOR_VSYNC failed\n");

    rt_kprintf("pud: %u rows x %u px (stride %u), %u bytes, page %d\n",
               (unsigned)rows, (unsigned)pixels_per_row,
               (unsigned)info.stride, (unsigned)fb_size, index);
    return 0;
}
/*
 * A shell command, not an INIT_APP_EXPORT hook.
 *
 * As an init hook it produced no output at all -- the symbol was in the
 * binary (__rt_init_pud_disp_test was there) but nothing was ever printed,
 * which left two unverified explanations: the hook ran before the console
 * was usable, or it blocked somewhere in mpp_fb_open().  A command removes
 * the timing question entirely and shows the return value as well.
 */
MSH_CMD_EXPORT_ALIAS(pud_disp_test, pudbars, paint PUD test bars on the panel);
