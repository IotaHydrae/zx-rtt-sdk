/*
 * Temporary local USB flash channel -- NOT part of the PUD protocol.
 *
 * See notes/pud-port.md ("TODO: 本工程临时的 USB 烧写通道").  The two boards
 * have different flash sizes and types, so this must not become part of the
 * shared protocol definition.  It exists because the vendor path measures
 * 1.01 MB/s (5.6 s per image) while EP1 already measures 39.3 MB/s, i.e. about
 * 0.15 s for the same image -- and the verification loop is the bottleneck.
 *
 * What it does NOT do: touch flash itself.  The SDK's OTA layer owns unpacking,
 * erasing, alignment and the A/B switch; we only hand it the byte stream.
 *
 * UNVERIFIED: who triggers the A/B switch and the reboot once the stream ends.
 * ota_shard_download_fun() may not do it, and this file does not assume it does
 * -- see the note in pud-port.md.
 */
#include <rtthread.h>
#include <aic_core.h>
#include "usbd_core.h"
#include "ota.h"
#include "absystem.h"
#include "msh.h"        /* msh_exec(): run a command sent from the host */   /* aic_upgrade_end() */
#include <rthw.h>       /* rt_hw_cpu_reset() */

/*
 * EP3, and the address is spelled out here rather than taken from
 * pud_vendor.h: this file must not depend on the protocol header at all,
 * so that the protocol cannot be edited by accident from here.  EP3 is
 * free because the PUD protocol defines it but deliberately leaves it out
 * of the descriptor.
 */
#define ZX_FLASH_EP_OUT_ADDR (USB_EP_DIR_OUT | 3)
/* Log stream, device -> host.  0x85 is free: 0x01/0x82/0x03/0x84/0x81/0x02 are taken. */
#define ZX_LOG_EP_IN_ADDR    (USB_EP_DIR_IN | 3)

/*
 * Matches the OTA layer's own OTA_BUFF_LEN (ota.h) and the chunk size its
 * working caller uses (uart_ota.c:230 feeds 2048).  4096 overflowed the
 * internal queue -- the device said so itself:
 *     E/NO_TAG: Queue overflow,please increase buffer size
 */
#define ZX_FLASH_CHUNK 2048
#define ZX_FLASH_CMD_MAX 128   /* msh command line accepted from the host */

/*
 * Running a shell command must not happen in the USB callback.
 *
 * msh_exec() prints, walks the command table and can take real time; doing it
 * in the completion callback holds the endpoint and the next transfer times
 * out.  Measured: a LOGSTAT read right after RUN_CMD(list_thread) timed out.
 * The same lesson already cost 20.34 -> 1.37 MB/s when the NAND burn ran in the
 * callback.  So the callback only copies the line and raises a flag, and a
 * task runs it.
 *
 * A flag polled by the task, rather than a message queue: the host-side
 * latency of 50 ms is irrelevant for a debug tool, and it avoids depending on
 * whether a given IPC call is legal from interrupt context -- which is not a
 * question this file can answer from the repository alone.
 */
static char zx_cmd_pending[ZX_FLASH_CMD_MAX];
static volatile int zx_cmd_ready;
static struct rt_thread zx_cmd_thread;
static char zx_cmd_stack[2048];

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t zx_flash_buf[ZX_FLASH_CHUNK];

static volatile uint32_t zx_flash_bytes;
static volatile uint32_t zx_flash_chunks;
static volatile int zx_flash_active;
static volatile int zx_flash_error;

/* Log streaming state.  One chunk per completion keeps the host's read
 * and the device's write in step without any protocol of their own. */
static rt_uint32_t zx_log_seq;
static volatile int zx_log_active;
static char zx_log_buf[ZX_FLASH_CHUNK + 16];

/* Records are handed out by the ring; see application/os/widgets/zx_logring.c. */
rt_uint32_t zxring_next(rt_uint32_t from_seq, char *buf, rt_size_t cap, rt_size_t *len);
void zxring_stats(rt_uint32_t *head, rt_uint32_t *tail, rt_uint32_t *seq, rt_uint32_t *dropped);
rt_uint32_t zxring_console_writes(void);
const char *zxring_console_name(void);
void zxring_puts(const char *s);

/*
 * Control channel for the loader.  Deliberately its own request number space,
 * not PUD's REQ_*: that space is protocol and must not grow for a local tool.
 */
#define ZX_FLASH_REQ_START 0x80
#define ZX_FLASH_REQ_STOP  0x81
#define ZX_FLASH_REQ_LOG   0x82   /* wValue = first sequence number to send */
#define ZX_FLASH_REQ_RUN   0x83   /* data = command line to execute */
#define ZX_FLASH_REQ_LOGSTAT 0x84 /* returns 4 u32: head, tail, seq, dropped */

static int zx_flash_request(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    (void)data;
    (void)len;

    switch (setup->bRequest) {
    case ZX_FLASH_REQ_START:
        zx_flash_bytes = 0;
        zx_flash_chunks = 0;
        zx_flash_error = 0;

        /*
         * aic_upgrade_start() reads osAB_now and points the target at the
         * *inactive* side (absystem.c:38); without it target_offset stays 0 and
         * the stream goes to a fixed side -- which is the running one half the
         * time.  aic_upgrade_end() then only writes osAB_next, so the pair is
         * the whole A/B flow and both halves have to be called.
         */
        if (aic_upgrade_start()) {
            zx_flash_error = 1;
            rt_kprintf("zxflash: aic_upgrade_start failed\n");
            return -1;
        }

        if (ota_init()) {
            zx_flash_error = 1;
            rt_kprintf("zxflash: ota_init failed\n");
            return -1;
        }
        zx_flash_active = 1;
        rt_kprintf("zxflash: ready\n");
        return 0;

    case ZX_FLASH_REQ_RUN:
        /*
         * Run a shell command sent by the host.  Its output needs no separate
         * channel: the console device forwards everything into the log ring,
         * so the host reads the result with the same read it already uses.
         * This is what makes the exported commands usable without a serial
         * console -- exercising them over the UART and then fetching the
         * output over USB was both awkward and racy.
         */
        {
            rt_size_t n = *len;

            if (n == 0 || n >= sizeof(zx_cmd_pending))
                return -1;
            /*
             * Latest command wins.  Stalling when the previous one has not
             * been picked up makes the host see a dead device -- which is
             * exactly what happened and cost a round of diagnosis -- and it
             * is worse than dropping the older command.
             */
            rt_memcpy(zx_cmd_pending, *data, n);
            zx_cmd_pending[n] = '\0';
            zx_cmd_ready = 1;
        }
        return 0;

    case ZX_FLASH_REQ_LOGSTAT:
        {
            /* 4 counters + console write count, then the console device's name. */
            static struct {
                rt_uint32_t head, tail, seq, dropped, console_writes;
                char name[16];
            } st;

            rt_memset(&st, 0, sizeof(st));
            zxring_stats(&st.head, &st.tail, &st.seq, &st.dropped);
            st.console_writes = zxring_console_writes();
            rt_strncpy(st.name, zxring_console_name(), sizeof(st.name) - 1);
            usbd_ep_start_write(ZX_LOG_EP_IN_ADDR, (const uint8_t *)&st, sizeof(st));
        }
        return 0;

    case ZX_FLASH_REQ_LOG:
        /*
         * wValue carries the first sequence number the host wants.  The stream
         * ends with a zero-length packet, so the host needs no framing: it
         * reads until a read comes back empty.  Sequence numbers are what let
         * it resume from where it stopped.
         */
        zx_log_seq = setup->wValue;
        zx_log_active = 1;
        {
            rt_size_t len = 0;
            rt_uint32_t got = zxring_next(zx_log_seq, zx_log_buf, sizeof(zx_log_buf), &len);

            if (!got) {
                zx_log_active = 0;
                usbd_ep_start_write(ZX_LOG_EP_IN_ADDR, NULL, 0);
                return 0;
            }
            zx_log_seq = got + 1;
            usbd_ep_start_write(ZX_LOG_EP_IN_ADDR, (const uint8_t *)zx_log_buf, len);
        }
        return 0;

    case ZX_FLASH_REQ_STOP:
        zx_flash_active = 0;
        ota_deinit();
        rt_kprintf("zxflash: %u bytes in %u chunks, error=%d\n",
                   (unsigned)zx_flash_bytes, (unsigned)zx_flash_chunks,
                   zx_flash_error);

        if (zx_flash_error)
            return -1;

        /*
         * A complete stream does not by itself change what boots.
         * aic_upgrade_end() only writes osAB_next into the environment
         * (packages/zx/ota/absystem.c:71) and nothing else here reboots, so
         * both steps are ours to take -- they were missing until now.
         */
        if (aic_upgrade_end()) {
            rt_kprintf("zxflash: aic_upgrade_end failed, not rebooting\n");
            return -1;
        }

        rt_kprintf("zxflash: osAB_next written, rebooting\n");
        rt_hw_cpu_reset();
        return 0;

    default:
        return -1;
    }
}

static void zx_flash_out(uint8_t ep, uint32_t nbytes)
{
    (void)ep;

    if (zx_flash_active && nbytes && !zx_flash_error) {
        if (ota_shard_download_fun((char *)zx_flash_buf, (int)nbytes)) {
            zx_flash_error = 1;
            rt_kprintf("zxflash: chunk %u failed at %u bytes\n",
                       (unsigned)zx_flash_chunks, (unsigned)zx_flash_bytes);
        }
        zx_flash_bytes += nbytes;
        zx_flash_chunks++;
    }

    /* Re-arm unconditionally: stopping mid-stream must not leave the endpoint
     * without a read, or the host blocks forever instead of seeing an error. */
    usbd_ep_start_read(ZX_FLASH_EP_OUT_ADDR, zx_flash_buf, sizeof(zx_flash_buf));
}

static void zx_cmd_task(void *arg)
{
    char cmd[ZX_FLASH_CMD_MAX];

    (void)arg;
    for (;;) {
        if (zx_cmd_ready) {
            rt_memcpy(cmd, zx_cmd_pending, sizeof(cmd));
            zx_cmd_ready = 0;
            msh_exec(cmd, rt_strlen(cmd));
        }
        rt_thread_mdelay(50);
    }
}

static int zx_cmd_task_start(void)
{
    char msg[96];
    rt_err_t err;

    err = rt_thread_init(&zx_cmd_thread, "zxcmd", zx_cmd_task, RT_NULL,
                         zx_cmd_stack, sizeof(zx_cmd_stack), 20, 10);
    if (err != RT_EOK) {
        rt_snprintf(msg, sizeof(msg), "[zxflash] zxcmd init failed: %d\n", err);
        zxring_puts(msg);
        return -1;
    }
    err = rt_thread_startup(&zx_cmd_thread);
    /* Always report, success included: the outcome has to be readable without
     * a console, which is the whole point of this work. */
    rt_snprintf(msg, sizeof(msg), "[zxflash] zxcmd started (startup=%d)\n", err);
    zxring_puts(msg);
    return err;
}
INIT_APP_EXPORT(zx_cmd_task_start);

static void zx_log_in_done(uint8_t ep, uint32_t nbytes)
{
    rt_size_t len = 0;
    rt_uint32_t got;

    (void)ep;
    (void)nbytes;

    if (!zx_log_active)
        return;

    got = zxring_next(zx_log_seq, zx_log_buf, sizeof(zx_log_buf), &len);
    if (!got) {
        zx_log_active = 0;
        usbd_ep_start_write(ZX_LOG_EP_IN_ADDR, NULL, 0);   /* end of stream */
        return;
    }
    zx_log_seq = got + 1;
    usbd_ep_start_write(ZX_LOG_EP_IN_ADDR, (const uint8_t *)zx_log_buf, len);
}

static void zx_flash_notify(uint8_t event, void *arg)
{
    (void)arg;

    if (event == USBD_EVENT_CONFIGURED)
        usbd_ep_start_read(ZX_FLASH_EP_OUT_ADDR, zx_flash_buf, sizeof(zx_flash_buf));
}

static struct usbd_interface zx_flash_intf;
static struct usbd_endpoint zx_flash_ep;
static struct usbd_endpoint zx_log_ep;

/* Called from pud_vendor_init() before usbd_initialize(), so that both
 * interfaces are registered before the core starts. */
int zx_usb_flash_init(void)
{
    zx_flash_intf.vendor_handler = zx_flash_request;
    zx_flash_intf.notify_handler = zx_flash_notify;
    usbd_add_interface(&zx_flash_intf);

    zx_flash_ep.ep_addr = ZX_FLASH_EP_OUT_ADDR;
    zx_flash_ep.ep_cb = zx_flash_out;
    usbd_add_endpoint(&zx_flash_ep);

    zx_log_ep.ep_addr = ZX_LOG_EP_IN_ADDR;
    zx_log_ep.ep_cb = zx_log_in_done;
    usbd_add_endpoint(&zx_log_ep);

    return 0;
}
