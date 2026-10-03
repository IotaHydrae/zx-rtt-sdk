#!/usr/bin/env python3
"""Send JPEG stills or an MJPEG stream to the ZX PUD display.

The firmware must report decoder type 1 (hardware JPEG). JPEG is sent as one
full-screen EP1 transfer; the current decoder cannot display a sub-rectangle.
"""

import argparse
import os
import subprocess
import sys
import time


PUD_TOOLS = os.path.abspath(os.path.join(
    os.path.dirname(__file__), "../../Pico-USB-Display/tools"))
if PUD_TOOLS not in sys.path:
    sys.path.insert(0, PUD_TOOLS)
import pud_usb


def jpeg_frames(path, width, height, fit):
    filters = ["scale=%d:%d:force_original_aspect_ratio=decrease" %
               (width, height),
               "pad=%d:%d:(ow-iw)/2:(oh-ih)/2:color=black" %
               (width, height)] if fit else ["scale=%d:%d" % (width, height)]
    cmd = ["ffmpeg", "-v", "error", "-i", path, "-vf", ",".join(filters),
           "-c:v", "mjpeg", "-q:v", "3", "-f", "image2pipe", "-"]
    try:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
    except FileNotFoundError:
        raise pud_usb.PudError("ffmpeg not found -- install it")

    data = bytearray()
    try:
        while True:
            chunk = proc.stdout.read(64 * 1024)
            if not chunk:
                break
            data.extend(chunk)
            start = 0
            while True:
                soi = data.find(b"\xff\xd8", start)
                if soi < 0:
                    if len(data) > 1:
                        del data[:-1]
                    break
                eoi = data.find(b"\xff\xd9", soi + 2)
                if eoi < 0:
                    if soi:
                        del data[:soi]
                    break
                yield bytes(data[soi:eoi + 2])
                del data[:eoi + 2]
                start = 0
    finally:
        proc.stdout.close()
        stopped_early = proc.poll() is None
        if stopped_early:
            proc.terminate()
        proc.wait(timeout=5)
        err = proc.stderr.read().decode(errors="replace").strip()
        proc.stderr.close()
        if err and not stopped_early and proc.returncode not in (0, -13, -15):
            raise pud_usb.PudError("ffmpeg: %s" % err)


def image_jpeg(path, width, height, fit, quality):
    from PIL import Image
    from io import BytesIO

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
    ap.add_argument("source")
    ap.add_argument("--xres", type=int, default=800)
    ap.add_argument("--yres", type=int, default=480)
    ap.add_argument("--video", action="store_true",
                    help="read source as a video and send MJPEG frames")
    ap.add_argument("--fps", type=float, default=None)
    ap.add_argument("--frames", type=int, default=None)
    ap.add_argument("--no-loop", action="store_true")
    ap.add_argument("--stretch", action="store_true")
    ap.add_argument("--quality", type=int, default=90)
    ap.add_argument("--stats", action="store_true")
    args = ap.parse_args()

    if not 1 <= args.quality <= 100:
        ap.error("--quality must be between 1 and 100")
    if args.video and args.no_loop is False:
        args.no_loop = True

    try:
        with pud_usb.open_device() as disp:
            disp.width, disp.height = args.xres, args.yres
            if disp.decoder_type not in (None, pud_usb.DECODER_TYPES["jpeg"]):
                sys.exit("the device reports decoder_type=%s, not 1 (jpeg); "
                         "rebuild and flash the JPEG firmware"
                         % disp.decoder_type)

            if args.video:
                frames = jpeg_frames(args.source, args.xres, args.yres,
                                     not args.stretch)
            else:
                frames = iter([image_jpeg(args.source, args.xres, args.yres,
                                           not args.stretch, args.quality)])

            started = time.perf_counter()
            report_frames = 0
            report_bytes = 0
            total_frames = 0
            for jpeg in frames:
                t0 = time.perf_counter()
                if len(jpeg) > disp._payload_limit():
                    raise pud_usb.PudError(
                        "JPEG frame is %d bytes, device limit is %d" %
                        (len(jpeg), disp._payload_limit()))
                disp.send_raw(jpeg, 0, 0, args.xres - 1, args.yres - 1)
                report_frames += 1
                report_bytes += len(jpeg)
                total_frames += 1
                if args.stats and time.perf_counter() - started >= 1:
                    elapsed = time.perf_counter() - started
                    print("%d frames, %.1f fps, %.2f MB/s" %
                          (report_frames, report_frames / elapsed,
                           report_bytes / elapsed / 1e6))
                    started = time.perf_counter()
                    report_frames = 0
                    report_bytes = 0
                if args.fps:
                    delay = 1.0 / args.fps - (time.perf_counter() - t0)
                    if delay > 0:
                        time.sleep(delay)
                if args.frames and total_frames >= args.frames:
                    break
    except pud_usb.PudError as exc:
        sys.exit(str(exc))
    except ImportError:
        sys.exit("Pillow is required for JPEG image input")
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
