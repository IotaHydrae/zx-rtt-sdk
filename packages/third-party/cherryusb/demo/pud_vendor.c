/*
 * The device half of the PUD vendor protocol, on the ZX/ArtInChip board.
 *
 * The host is the same kernel driver (and the same pyusb tools) that talks to
 * the RP2350 build, so this file keeps two things deliberately unchanged:
 *
 *   - the ids 0x2E8A:0x0001, because that is what the driver matches on
 *   - the capability report, because the driver derives its buffer sizes, DRM
 *     mode and input axes from it before it registers anything
 *
 * Everything the host does not look at is fair game and differs: the RP2350 is
 * full-speed with 64-byte bulk packets, this board is high-speed with 512, and
 * the descriptor macros below pick that up from CONFIG_USB_HS.
 *
 * Protocol reference: PUD-kernel-drivers/notes/usb-protocol.md
 */
#include "usbd_core.h"
#include "pud_vendor.h"

#ifdef CONFIG_ZX_LOCAL_USB_FLASH
#warning "PUD: local flash channel ENABLED in this translation unit"
#else
#warning "PUD: local flash channel DISABLED in this translation unit"
#endif


#ifdef CONFIG_ZX_LOCAL_USB_FLASH
int zx_usb_flash_init(void);   /* application/os/widgets/zx_usb_flash.c */
#endif
#ifdef CONFIG_ZX_ADB_COMPOSITE
int adb_winusb_register(void); /* packages/third-party/adbd/core/adbcherryusb.c */
#endif

#define USBD_VID           0x2E8A
#define USBD_PID           0x0001
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*
 * High-speed bulk endpoints take 512-byte packets, full-speed 64.  Getting this
 * wrong does not fail to enumerate -- the host simply transfers a fraction of
 * the bandwidth, which on this board is the whole reason for the port.
 */
#ifdef CONFIG_USB_HS
#define PUD_BULK_MPS 512
#else
#define PUD_BULK_MPS 64
#endif

#define PUD_INT_MPS 64
#define PUD_INT_INTERVAL 8 /* bInterval, in frames: the value the host expects */

/*
 * config(9) + interface(9) + three endpoints(7 each).  With the local flash
 * channel enabled there is a second interface and a fourth endpoint; with it
 * off the descriptor is byte-for-byte the PUD one.
 */
/*
 * config(9) + one interface per slot (9 each) + 7 per endpoint.
 *
 * Slots: 0 PUD, 1 local flash channel (optional), 2 ADB (optional).
 */
#if defined(CONFIG_ZX_LOCAL_USB_FLASH) && defined(CONFIG_ZX_ADB_COMPOSITE)
#define USB_CONFIG_SIZE (9 + 9*3 + 7*7)
#elif defined(CONFIG_ZX_LOCAL_USB_FLASH)
#define USB_CONFIG_SIZE (9 + 9*2 + 7*5)
#elif defined(CONFIG_ZX_ADB_COMPOSITE)
#define USB_CONFIG_SIZE (9 + 9*2 + 7*5)
#else
#define USB_CONFIG_SIZE (9 + 9 + 7*3)
#endif

static const uint8_t pud_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    #if defined(CONFIG_ZX_LOCAL_USB_FLASH) && defined(CONFIG_ZX_ADB_COMPOSITE)
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
#elif defined(CONFIG_ZX_LOCAL_USB_FLASH) || defined(CONFIG_ZX_ADB_COMPOSITE)
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
#else
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
#endif
    /* one vendor-specific interface, no class driver involved */
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(PUD_EP1_OUT_ADDR, USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(PUD_EP2_IN_ADDR, USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(PUD_EP4_IN_ADDR, USB_ENDPOINT_TYPE_INTERRUPT, PUD_INT_MPS, PUD_INT_INTERVAL),
#ifdef CONFIG_ZX_LOCAL_USB_FLASH
    /*
     * Local flash channel: interface 1, EP3 OUT bulk.  Not PUD protocol, and
     * deliberately not given a PUD_* constant -- the endpoint address is
     * spelled out here and in application/os/widgets/zx_usb_flash.c, which is
     * where the other end of this wire lives.  Keeping it out of pud_vendor.h
     * keeps the protocol header free of local-tool detail.
     */
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT((USB_EP_DIR_OUT | 3), USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
    /* EP3 IN, matching the EP3 OUT already in use.  EP5 was tried first and the
     * device stalled SET_CONFIGURATION with -EPIPE -- only EPs up to 4 are
     * known good here, and IN/OUT are separate register banks in the port. */
    USB_ENDPOINT_DESCRIPTOR_INIT((USB_EP_DIR_IN | 3), USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
#endif
#ifdef CONFIG_ZX_ADB_COMPOSITE
    /*
     * ADB: class 0xff / subclass 0x42 / protocol 0x01 is what the adb client
     * looks for, so the device keeps the PUD ids and adb still finds its
     * interface.  Endpoints 0x81 IN and 0x02 OUT are free -- PUD uses 0x01,
     * 0x82, 0x03 and 0x84.
     */
    USB_INTERFACE_DESCRIPTOR_INIT(0x02, 0x00, 0x02, 0xff, 0x42, 0x01, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT((USB_EP_DIR_IN | 1), USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT((USB_EP_DIR_OUT | 2), USB_ENDPOINT_TYPE_BULK, PUD_BULK_MPS, 0x00),
#endif
    /* string 0 */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* string 1: manufacturer */
    0x0C, USB_DESCRIPTOR_TYPE_STRING,
    'Z', 0x00, 'X', 0x00, 'R', 0x00, 'T', 0x00, 'T', 0x00,
    /* string 2: product -- "PUD Display" is 11 chars, so 2 + 11*2 = 24 = 0x18.
     * It said 0x1A first, which made the host read one character past the end. */
    0x18, USB_DESCRIPTOR_TYPE_STRING,
    'P', 0x00, 'U', 0x00, 'D', 0x00, ' ', 0x00, 'D', 0x00,
    'i', 0x00, 's', 0x00, 'p', 0x00, 'l', 0x00, 'a', 0x00, 'y', 0x00,
};

/* The answer buffer the EP2 response is staged into. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t ep2_write_buffer[64];

/*
 * Build the response for one EP2 command and return how many bytes it produced.
 *
 * Returning the produced length rather than the host's requested size matters:
 * a command with a short answer would otherwise leak whatever was left in the
 * buffer from the previous one.
 */
static uint32_t pud_ep2_fsm(uint16_t cmd, uint32_t len)
{
    switch (cmd) {
    case PUD_CMD_GET_CAPS: {
        const struct pud_caps caps = {
            .magic = PUD_CAPS_MAGIC,
            .proto_ver = PUD_PROTO_VER,
            .frame_max = PUD_FRAME_MAX,
            .decoder_type = PUD_DISP_DECODER_TYPE,
            .xres = PUD_DISP_XRES,
            .yres = PUD_DISP_YRES,
            .pixelclock_khz = PUD_DISP_PIXELCLOCK,
            .rotation = PUD_DISP_ROTATION,
            .bpp = PUD_DISP_BPP,
            .intf_type = PUD_DISP_INTF_TYPE,
            .tp_polling_period = 0,
            .width_mm = PUD_DISP_WIDTH_MM,
            .height_mm = PUD_DISP_HEIGHT_MM,
            .flags = PUD_HAS_TOUCH ? PUD_CAPS_TOUCH : 0,
        };
        uint32_t n = sizeof(caps);

        if (len < n)
            n = len; /* the host may ask for only the original 16 bytes */
        memcpy(ep2_write_buffer, &caps, n);
        return n;
    }
    default:
        /*
         * Unknown command: answer with nothing.  The host reads a short packet
         * and concludes "not supported", which it can tell apart from a device
         * that never answered at all.
         */
        return 0;
    }
}

static int pud_vendor_request(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    /*
     * Unconditional, and deliberately before the switch: the previous
     * diagnostic only printed on failure, so "no output" could not tell
     * "handler not called" apart from "write succeeded" -- which is exactly
     * the ambiguity that cost time.
     */
    switch (setup->bRequest) {
    case REQ_EP2_IN: {
        const struct req_ep2_in *req = (const struct req_ep2_in *)*data;
        uint32_t n;

        /* A payload shorter than the header means the host's framing was wrong;
         * stall rather than read past it. */
        if (*len < sizeof(*req))
            return -1;

        n = pud_ep2_fsm(req->cmd, req->size);

        /*
         * The port's start_write fails silently with a negative code, and the
         * host only sees a bulk read that never completes -- so report which
         * guard fired instead of guessing from the timeout.
         *
         *   -2 endpoint already enabled   -3 endpoint has no MPS configured
         *   -4 buffer not 4-byte aligned
         */
        {
            int ret = usbd_ep_start_write(PUD_EP2_IN_ADDR, ep2_write_buffer, n);

            if (ret) {
                /*
                 * USB_LOG_RAW, not USB_LOG_ERR: the latter is compiled out
                 * unless CONFIG_USB_DBG_LEVEL >= USB_DBG_ERROR, which it is
                 * not here -- a diagnostic that silently vanishes is worse
                 * than none, because its absence looks like evidence.
                 */
                USB_LOG_RAW("PUD: EP2 start_write failed: %d (n=%u align=%u)\n",
                            ret, (unsigned)n,
                            (unsigned)((uint32_t)(uintptr_t)ep2_write_buffer & 0x03));
                return -1;
            }
        }
        return 0;
    }
    default:
        /* REQ_EP1_OUT is retired in protocol v2 and an old host must notice. */
        return -1;
    }
}

/*
 * Every IN-writing demo in this repository registers a real callback; none
 * leaves ep_cb NULL.  This one only observes completion, which is enough to
 * follow the existing pattern and to see the transfer actually finish.
 */
static void pud_ep2_in(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
}

/*
 * Image stream, receive half.
 *
 * For the throughput measurement the device must never be the limit: accept
 * whatever arrives and re-arm at once, and print nothing -- a log line inside
 * this callback perturbs the very number being measured.  The host knows how
 * many bytes it sent and how long it took, so it computes the rate.
 *
 * Protocol v2 carries the rectangle in the first 12 bytes of the transfer
 * rather than in a prior control request, so a sink does not even have to
 * parse them to be useful.
 */
#define PUD_EP1_BUF_SIZE 65536

/*
 * EP1 frame header, from the protocol's own definition
 * (PUD-kernel-drivers/notes/usb-protocol.md): 12 bytes, little-endian, no
 * padding, coordinates absolute to the screen and *inclusive* on xe/ye.
 */
struct pud_ep1_header {
    uint16_t xs;
    uint16_t ys;
    uint16_t xe;   /* inclusive */
    uint16_t ye;   /* inclusive */
    uint32_t size; /* payload bytes that follow */
};

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t ep1_read_buffer[PUD_EP1_BUF_SIZE];
volatile uint32_t pud_ep1_bytes;
volatile uint32_t pud_ep1_transfers;
volatile uint32_t pud_ep1_frames;
volatile uint32_t pud_ep1_bad;

static void pud_ep1_out(uint8_t ep, uint32_t nbytes)
{
    (void)ep;

    pud_ep1_bytes += nbytes;
    pud_ep1_transfers++;

    /*
     * Step 1 of the display path: parse and validate the header, no drawing
     * yet.  The whole transfer lands in one buffer because the read was armed
     * with its full size, so nbytes is the complete frame -- unlike the RP2350
     * firmware, which reads the header and the payload in two stages.
     *
     * The protocol is explicit that a bad header is discarded and the endpoint
     * re-armed, never stalled: a stall makes the host's write fail, and the
     * host controller has been observed to wedge on clear_halt and retry.
     */
    if (nbytes >= sizeof(struct pud_ep1_header)) {
        const struct pud_ep1_header *h = (const struct pud_ep1_header *)ep1_read_buffer;

        if (h->xe < h->xs || h->ye < h->ys ||
            h->xe >= PUD_DISP_XRES || h->ye >= PUD_DISP_YRES ||
            (uint32_t)sizeof(*h) + h->size > nbytes) {
            pud_ep1_bad++;
        } else {
            pud_ep1_frames++;
            rt_kprintf("pud: frame x %u..%u y %u..%u size %u (transfer %u)\n",
                       (unsigned)h->xs, (unsigned)h->xe,
                       (unsigned)h->ys, (unsigned)h->ye,
                       (unsigned)h->size, (unsigned)nbytes);
        }
    } else {
        pud_ep1_bad++;
    }

    usbd_ep_start_read(PUD_EP1_OUT_ADDR, ep1_read_buffer, sizeof(ep1_read_buffer));
}

/*
 * The first read has to wait for SET_CONFIGURATION: before that the port has
 * no MPS for this endpoint and start_read is refused.
 */
static void pud_notify(uint8_t event, void *arg)
{
    (void)arg;

    if (event == USBD_EVENT_CONFIGURED) {
        usbd_ep_start_read(PUD_EP1_OUT_ADDR, ep1_read_buffer, sizeof(ep1_read_buffer));
    }
}

struct usbd_endpoint pud_ep1_out_ep = {
    .ep_addr = PUD_EP1_OUT_ADDR,
    .ep_cb = pud_ep1_out,
};

struct usbd_endpoint pud_ep2_in_ep = {
    .ep_addr = PUD_EP2_IN_ADDR,
    .ep_cb = pud_ep2_in,
};

struct usbd_endpoint pud_ep4_in_ep = {
    .ep_addr = PUD_EP4_IN_ADDR,
    .ep_cb = NULL,
};

static struct usbd_interface pud_intf0;

int pud_vendor_init(void)
{
    /*
     * One interface, and the vendor handler hangs off it: usbd_core tries the
     * interface handlers in order and the first one returning 0 wins, so a
     * control request this file does not recognise still falls through to the
     * core's own handling.
     */
    usbd_desc_register(pud_descriptor);

    pud_intf0.vendor_handler = pud_vendor_request;
    pud_intf0.notify_handler = pud_notify;
    usbd_add_interface(&pud_intf0);

    usbd_add_endpoint(&pud_ep1_out_ep);
    usbd_add_endpoint(&pud_ep2_in_ep);
    usbd_add_endpoint(&pud_ep4_in_ep);

#ifdef CONFIG_ZX_LOCAL_USB_FLASH
    /* Registered here but implemented in the application, not in this file:
     * it is a temporary local tool, not part of the ported protocol. */
    zx_usb_flash_init();
#endif

#ifdef CONFIG_ZX_ADB_COMPOSITE
    /* adbd's own interface only; the descriptor above is ours now. */
    adb_winusb_register();
#endif

    usbd_initialize();
    return 0;
}
INIT_DEVICE_EXPORT(pud_vendor_init);
