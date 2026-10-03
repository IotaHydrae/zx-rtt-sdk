#!/usr/bin/env python3
"""Send an MJPEG video stream to the ZX PUD display.

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

# ZX firmware uses the vendor application identity, not the Pico USB ID.
PUD_VID = 0x33C3
PUD_PID = 0x7788


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


def panel_size(disp, xres, yres):
    caps = disp.caps or {}
    return (xres or caps.get("xres") or 800,
            yres or caps.get("yres") or 480)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("source")
    ap.add_argument("--xres", type=int, default=None)
    ap.add_argument("--yres", type=int, default=None)
    ap.add_argument("--fps", type=float, default=None)
    ap.add_argument("--frames", type=int, default=None)
    ap.add_argument("--stretch", action="store_true")
    ap.add_argument("--stats", action="store_true")
    args = ap.parse_args()

    try:
        with pud_usb.open_device(vid=PUD_VID, pid=PUD_PID) as disp:
            width, height = panel_size(disp, args.xres, args.yres)
            disp.width, disp.height = width, height
            if disp.decoder_type not in (None, pud_usb.DECODER_TYPES["jpeg"]):
                sys.exit("the device reports decoder_type=%s, not 1 (jpeg); "
                         "rebuild and flash the JPEG firmware"
                         % disp.decoder_type)

            frames = jpeg_frames(args.source, width, height, not args.stretch)

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
                disp.send_raw(jpeg, 0, 0, width - 1, height - 1)
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
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
