#!/usr/bin/env python3
"""zxflashctl -- push a firmware image into the running ZX board over USB.

TEMPORARY LOCAL TOOL.  Not part of the PUD protocol and not a replacement for
the vendor's upgcmd/BROM path, which stays as the recovery route.  See
notes/pud-port.md for why this exists (the vendor path measures 1.01 MB/s, this
one rides EP3 at the board's high-speed rate) and for its open question about
who triggers the A/B switch and reboot afterwards.

Exit codes follow the workspace convention:
    0 success   1 failure   2 usage   3 environment (no device/permission)

Usage:
    zxflashctl.py flash <image.img> [--vid 0x2E8A] [--pid 0x0001]
    zxflashctl.py info
"""
import argparse
import os
import struct
import sys
import time
import zlib

try:
    import usb.core
    import usb.util
except ImportError:
    print("zxflashctl: pyusb is required (pip install pyusb)", file=sys.stderr)
    sys.exit(3)

PROG = 'zxflashctl'
VID_DEFAULT = 0x2E8A
PID_DEFAULT = 0x0001

# The local flash channel's own interface and request numbers; deliberately not
# PUD's REQ_* space.
FLASH_INTERFACE = 1
FLASH_EP_OUT = 0x03
REQ_START = 0x80
REQ_STOP = 0x81
TYPE_VENDOR_OUT = 0x40

# 2048: the OTA layer's internal queue overflows at 4096.  Matches ota.h's
# OTA_BUFF_LEN and uart_ota.c's chunking.
CHUNK = 2048


def open_device(vid, pid):
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print("zxflashctl: no device %04x:%04x" % (vid, pid), file=sys.stderr)
        sys.exit(3)
    # pyusb does not set a configuration, and without one every later transfer
    # fails with "Configuration not set".  Do not swallow the failure: a silent
    # one leaves the tool reporting success on a device it cannot talk to.
    # Do not reset on failure.  A reset re-enumerates the device, so using it
    # as a retry turns every attempt to talk to it into another configuration
    # -- visible in the board's own log ring as a fresh "Open ep" group per
    # attempt.  The project's own tool (pud_usb.py) does not reset either.
    try:
        dev.set_configuration()
    except usb.core.USBError:
        pass
    return dev


def cmd_info(dev):
    cfg = dev.get_active_configuration()
    print("device %04x:%04x speed %s" % (dev.idVendor, dev.idProduct, dev.speed))
    for intf in cfg:
        print("  interface %d: class %#x, %d endpoints"
              % (intf.bInterfaceNumber, intf.bInterfaceClass, intf.bNumEndpoints))
    return 0


CPIO_MAGIC_NEWC = b"070701"
CPIO_TRAILER = "TRAILER!!!"


def cpio_members(data):
    """Walk the archive and yield (name, size).

    Raises ValueError on anything malformed or missing its terminator.  This
    exists because a complete transfer of an incomplete archive is not an
    error anywhere downstream: ota_shard_download_fun() is a streaming parser,
    so a truncated archive simply ends early and it reports success.  Measured
    -- a truncated cpio was sent, no OTA error appeared, and the device
    switched to the half-written side and stopped booting.  The transfer-level
    size and CRC cannot see this; only the content can.
    """
    off = 0
    while True:
        hdr = data[off:off + 110]
        if len(hdr) < 110 or hdr[:6] not in (CPIO_MAGIC_NEWC, CPIO_MAGIC_CRC):
            raise ValueError("no cpio header at offset %d" % off)
        fields = [int(hdr[6 + i * 8:14 + i * 8], 16) for i in range(13)]
        filesize, namesize = fields[6], fields[11]
        if namesize == 0:
            raise ValueError("zero-length name at offset %d" % off)
        name = data[off + 110:off + 110 + namesize - 1].decode("ascii", "replace")
        off = (off + 110 + namesize + 3) & ~3
        yield name, filesize
        off = (off + filesize + 3) & ~3
        if name == CPIO_TRAILER:
            return
CPIO_MAGIC_CRC = b"070702"   # what tools/scripts/mkcpio.py produces (-H crc)


def check_image_complete(image, data):
    """Reject an archive that does not end with its terminator."""
    try:
        members = list(cpio_members(data))
    except (ValueError, IndexError) as exc:
        return "%s is not a complete cpio archive: %s" % (image, exc)
    if not members or members[-1][0] != CPIO_TRAILER:
        return "%s is missing its %s terminator" % (image, CPIO_TRAILER)
    print("  cpio members: %s"
          % ", ".join("%s(%d B)" % (n, z) for n, z in members if n != CPIO_TRAILER))
    return None


def check_image_format(image, data):
    """Refuse a file the device cannot parse.

    The OTA layer parses a cpio archive (packages/zx/ota/ota.c:470), so the
    ArtInChip .img container -- a different format that looks just as plausible
    -- is rejected before spending a transfer on it.  That mistake cost a
    round: the device answered "chunk 0 failed" and the cause was the input
    file, not the channel.
    """
    if data[:6] in (CPIO_MAGIC_NEWC, CPIO_MAGIC_CRC):
        return None
    if image.endswith(".img") or data[:6] == b"AIC.FW":
        return ("%s is an ArtInChip .img container; the device parses cpio.\n"
                "Send the archive the build produces instead: "
                "<output>/<board>/images/ota.cpio" % image)
    return ("%s does not start with a cpio magic (%s / %s)"
            % (image, CPIO_MAGIC_NEWC.decode(), CPIO_MAGIC_CRC.decode()))


def cmd_flash(dev, image, verify_size=True):
    size = os.path.getsize(image)
    with open(image, "rb") as fh:
        data = fh.read()

    problem = check_image_format(image, data)
    if problem:
        print("zxflashctl: %s" % problem, file=sys.stderr)
        return 2
    problem = check_image_complete(image, data)
    if problem:
        print("zxflashctl: %s" % problem, file=sys.stderr)
        return 2

    print("image %s: %d bytes" % (image, size))

    # The device gates its A/B switch and reboot on these, so a transfer that
    # stops halfway cannot be mistaken for success.
    crc = zlib.crc32(data) & 0xFFFFFFFF
    rc = dev.ctrl_transfer(TYPE_VENDOR_OUT, REQ_START, 0, FLASH_INTERFACE,
                           struct.pack("<II", size, crc), timeout=5000)
    print("start -> %r (expect %d bytes, crc %08x)" % (rc, size, crc))

    started = time.time()
    sent = 0
    try:
        for off in range(0, size, CHUNK):
            chunk = data[off:off + CHUNK]
            # A short final write is fine: the device feeds whatever it gets to
            # the OTA layer, which does its own alignment.
            written = dev.write(FLASH_EP_OUT, chunk, timeout=5000)
            if written != len(chunk):
                print("zxflashctl: short write at %d (%d/%d)"
                      % (off, written, len(chunk)), file=sys.stderr)
                return 1
            sent += written
    except usb.core.USBError as exc:
        print("zxflashctl: write failed after %d bytes: %s" % (sent, exc),
              file=sys.stderr)
        return 1
    elapsed = time.time() - started

    try:
        rc = dev.ctrl_transfer(TYPE_VENDOR_OUT, REQ_STOP, 0, FLASH_INTERFACE,
                               None, timeout=5000)
        print("stop ->", rc)
    except usb.core.USBError as exc:
        # Expected if the device resets to apply the update.
        print("stop -> %s (device may be rebooting)" % exc)

    print("sent %d bytes in %.3f s (%.2f MB/s)"
          % (sent, elapsed, sent / elapsed / 1e6 if elapsed else 0.0))
    if verify_size and sent != size:
        print("zxflashctl: sent %d != image %d" % (sent, size), file=sys.stderr)
        return 1
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--vid", type=lambda s: int(s, 0), default=VID_DEFAULT)
    ap.add_argument("--pid", type=lambda s: int(s, 0), default=PID_DEFAULT)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("flash", help="send an image over the local channel")
    p.add_argument("image")
    sub.add_parser("info", help="show the device's interfaces")

    args = ap.parse_args()
    dev = open_device(args.vid, args.pid)
    if args.cmd == "info":
        return cmd_info(dev)
    return cmd_flash(dev, args.image)


if __name__ == "__main__":
    sys.exit(main())
