#!/usr/bin/env python3
"""Display one JPEG image on the ZX PUD display."""

from io import BytesIO
import argparse
import os
import sys

PUD_TOOLS = os.path.abspath(os.path.join(
    os.path.dirname(__file__), "../../Pico-USB-Display/tools"))
if PUD_TOOLS not in sys.path:
    sys.path.insert(0, PUD_TOOLS)
import pud_usb


def image_jpeg(path, width, height, fit, quality):
    from PIL import Image

    image = Image.open(path).convert("RGB")
    if fit:
        image.thumbnail((width, height), Image.Resampling.LANCZOS)
        canvas = Image.new("RGB", (width, height), (0, 0, 0))
        canvas.paste(image, ((width - image.width) // 2,
                             (height - image.height) // 2))
        image = canvas
    else:
        image = image.resize((width, height), Image.Resampling.LANCZOS)
    out = BytesIO()
    image.save(out, format="JPEG", quality=quality, optimize=False)
    return out.getvalue()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("image")
    ap.add_argument("--xres", type=int, default=None)
    ap.add_argument("--yres", type=int, default=None)
    ap.add_argument("--stretch", action="store_true")
    ap.add_argument("--quality", type=int, default=90)
    ap.add_argument("--repeat", type=int, default=1)
    ap.add_argument("--stats", action="store_true")
    args = ap.parse_args()

    if not 1 <= args.quality <= 100:
        ap.error("--quality must be between 1 and 100")
    if args.repeat < 1:
        ap.error("--repeat must be >= 1")

    try:
        with pud_usb.open_device() as disp:
            caps = disp.caps or {}
            width = args.xres or caps.get("xres") or 800
            height = args.yres or caps.get("yres") or 480
            disp.width, disp.height = width, height
            if disp.decoder_type not in (None, pud_usb.DECODER_TYPES["jpeg"]):
                sys.exit("the device reports decoder_type=%s, not 1 (jpeg); "
                         "rebuild and flash the JPEG firmware"
                         % disp.decoder_type)

            jpeg = image_jpeg(args.image, width, height,
                              not args.stretch, args.quality)
            if len(jpeg) > disp._payload_limit():
                raise pud_usb.PudError(
                    "JPEG image is %d bytes, device limit is %d" %
                    (len(jpeg), disp._payload_limit()))
            for _ in range(args.repeat):
                disp.send_raw(jpeg, 0, 0, width - 1, height - 1)
            if args.stats:
                print("%d frame(s), %d bytes each" % (args.repeat, len(jpeg)))
    except pud_usb.PudError as exc:
        sys.exit(str(exc))
    except ImportError:
        sys.exit("Pillow is required for JPEG image input")
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
