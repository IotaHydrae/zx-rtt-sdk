/*
 * PUD vendor protocol -- the device side of the Pico USB Display protocol,
 * re-implemented on the ZX/ArtInChip board.
 *
 * The wire format is fixed by the host side: PUD-kernel-drivers/notes/usb-protocol.md
 * is the authoritative definition and this file mirrors it.  Nothing here may be
 * renumbered or reordered without changing the host's header too.
 */
#ifndef __PUD_VENDOR_H
#define __PUD_VENDOR_H

/* Endpoint addresses (protocol doc, "端点分配") */
#define PUD_EP1_OUT_ADDR (USB_EP_DIR_OUT | 1) /* image stream, bulk OUT  */
#define PUD_EP2_IN_ADDR  (USB_EP_DIR_IN | 2)  /* query response, bulk IN */
#define PUD_EP4_IN_ADDR  (USB_EP_DIR_IN | 4)  /* touch reports, interrupt */

/* ArtInChip's application USB identity; keep Pico's identity driver-side only. */
#define PUD_USB_VID 0x33C3
#define PUD_USB_PID 0x7788

/*
 * Vendor request numbers.  REQ_EP1_OUT (0x02) is retired with protocol v2: the
 * rectangle now travels in the EP1 header, and an old host asking for it should
 * get a stall rather than have its frames misparsed.
 */
#define REQ_EP0_OUT   0x00
#define REQ_EP0_IN    0x01
#define REQ_EP2_IN    0x03
#define REQ_EP3_OUT   0x04
#define REQ_EP4_IN    0x05
#define REQ_SET_PARAM 0x06

/* Commands carried in struct req_ep2_in.cmd */
#define PUD_CMD_GET_SN   0x01
#define PUD_CMD_GET_CAPS 0x02

/* struct pud_caps.magic */
#define PUD_CAPS_MAGIC 0x43445550 /* "PUDC" */
#define PUD_PROTO_VER  2

/* struct pud_caps.flags */
#define PUD_CAPS_TOUCH 0x0001 /* an input driver is compiled in and polled */

/*
 * The EP2 request header.  The command travels in the control transfer's data
 * payload, not in the setup packet's wValue/wIndex -- the host sends this
 * struct as the data stage of REQ_EP2_IN.
 */
struct req_ep2_in {
	uint16_t cmd;
	uint16_t size;
};

/*
 * What the device tells the host about itself.  The first 16 bytes are the
 * original layout; the fields after them were appended, so a host that only
 * reads 16 bytes still works.  Field order is protocol, not style.
 */
struct pud_caps {
	uint32_t magic;
	uint32_t proto_ver;
	uint32_t frame_max;    /* single EP1 transfer limit, header included */
	uint32_t decoder_type; /* PUD_DECODER_* */

	uint16_t xres;
	uint16_t yres;
	uint16_t pixelclock_khz;
	uint8_t  rotation;
	uint8_t  bpp;
	uint8_t  intf_type;
	uint8_t  tp_polling_period;
	uint16_t width_mm;  /* 0 = unknown to the device */
	uint16_t height_mm;
	uint16_t flags;     /* PUD_CAPS_TOUCH */
};

/* Decoder ids the host chooses its encoder from (protocol field, append only) */
#define PUD_DECODER_JPEG 1
#define PUD_DECODER_QOI 3

/*
 * Compile-time identity of this board, reported through PUD_CMD_GET_CAPS.
 *
 * The panel is natively 480x800 (portrait), but the framebuffer layer runs it
 * rotated 90 degrees (AICFB_ROTATE_90), so the canvas the host actually drives
 * is 800x480 landscape.  The protocol asks for the panel "in the coordinate
 * system it is driven in", and that is the rotated one.
 *
 * Confirmed on the glass: test bars painted in buffer coordinates come out
 * left-to-right in the order they were written, so the buffer's 800-pixel rows
 * do map to the horizontal axis -- which is also what stride/2 = 800 says.
 */
#define PUD_DISP_XRES          800
#define PUD_DISP_YRES          480
#define PUD_DISP_BPP           16
#define PUD_DISP_ROTATION      90
#define PUD_DISP_PIXELCLOCK    0 /* not estimated here; 0 means "unknown" */
#define PUD_DISP_INTF_TYPE     0
#define PUD_DISP_WIDTH_MM      0 /* unknown until the panel data is filled in */
#define PUD_DISP_HEIGHT_MM     0
#define PUD_DISP_DECODER_TYPE  PUD_DECODER_JPEG
#define PUD_HAS_TOUCH          0 /* this panel has no touch controller */

/* EP1 transfer limit, header included.  Sized to the receive buffer. */
#define PUD_FRAME_MAX 65536

#endif /* __PUD_VENDOR_H */
