#!/usr/bin/env python3
"""zxlogctl -- pull the board's log ring over USB instead of watching the UART.

The console is 115200 baud (about 11.5 KB/s) and intrusive: one rt_kprintf per
frame was measured at ~5.2 ms and cost the USB channel 20.34 -> 1.37 MB/s.  The
ring in application/os/widgets/zx_logring.c holds the logs from boot instead,
and this reads them over the local channel's IN endpoint (0x85).

Protocol with the device (see zx_usb_flash.c):
    control OUT  bRequest 0x82, wValue = first record's text offset, wIndex = 1
    then read 0x85 until a zero-length read arrives (that ends the stream)

Exit codes follow the workspace convention:
    0 success   1 failure   2 usage   3 environment (no device/permission)

Usage:
    zxlogctl.py read [--from N]
    zxlogctl.py info
"""
import argparse
import sys
import time

try:
    import usb.core
except ImportError:
    print("zxlogctl: pyusb is required (pip install pyusb)", file=sys.stderr)
    sys.exit(3)

PROG = 'zxlogctl'
VID_DEFAULT = 0x2E8A
PID_DEFAULT = 0x0001
LOG_INTERFACE = 1
LOG_EP_IN = 0x83
REQ_LOG = 0x82
REQ_RUN = 0x83
REQ_LOGSTAT = 0x84
TYPE_VENDOR_OUT = 0x40
CHUNK = 4096


def open_device(vid, pid):
    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print("zxlogctl: no device %04x:%04x" % (vid, pid), file=sys.stderr)
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
    except usb.core.USBError as exc:
        if dev.get_active_configuration() is None:
            print("zxlogctl: cannot select USB configuration: %s" % exc,
                  file=sys.stderr)
            sys.exit(3)
    return dev


def cmd_info(dev):
    cfg = dev.get_active_configuration()
    print("device %04x:%04x speed %s" % (dev.idVendor, dev.idProduct, dev.speed))
    for intf in cfg:
        eps = ",".join("%#04x/%s" % (ep.bEndpointAddress, "in" if ep.bEndpointAddress & 0x80 else "out")
                       for ep in intf)
        print("  interface %d: class %#x, endpoints %s"
              % (intf.bInterfaceNumber, intf.bInterfaceClass, eps or "-"))
    return 0


def dump(dev, from_seq):
    """Ask for the ring from `from_seq` and print until the device says stop.

    The device uses the sequence number to continue from a previous read.  The
    stream contains text only, so the host does not reconstruct sequence values
    locally.
    """
    try:
        dev.ctrl_transfer(TYPE_VENDOR_OUT, REQ_LOG, from_seq, LOG_INTERFACE, None,
                          timeout=5000)
    except usb.core.USBTimeoutError as exc:
        print("zxlogctl: log request timed out: %s" % exc, file=sys.stderr)
        return 4
    except usb.core.USBError as exc:
        print("zxlogctl: log request failed: %s" % exc, file=sys.stderr)
        return 1

    total = 0
    started = time.time()
    while True:
        try:
            data = dev.read(LOG_EP_IN, CHUNK, timeout=3000)
        except usb.core.USBTimeoutError as exc:
            print("zxlogctl: log stream timed out: %s" % exc, file=sys.stderr)
            return 4
        except usb.core.USBError as exc:
            print("zxlogctl: log stream failed: %s" % exc, file=sys.stderr)
            return 1
        if len(data) == 0:
            break
        sys.stdout.write(bytes(data).decode("utf-8", "replace"))
        sys.stdout.flush()
        total += len(data)

    elapsed = time.time() - started
    print("\nzxlogctl: %d bytes in %.3f s (%.1f KB/s)"
          % (total, elapsed, total / elapsed / 1e3 if elapsed else 0.0),
          file=sys.stderr)
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--vid", type=lambda s: int(s, 0), default=VID_DEFAULT)
    ap.add_argument("--pid", type=lambda s: int(s, 0), default=PID_DEFAULT)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("read", help="dump the log ring")
    p.add_argument("--from", dest="from_seq", type=int, default=1,
                   help="first sequence number (device ignores it for now)")
    r = sub.add_parser("run", help="run a shell command on the board")
    r.add_argument("command")
    r.add_argument("--follow", action="store_true",
                   help="read the log ring afterwards to show the output")
    sub.add_parser("stats", help="ring counters: head/tail/seq/dropped")
    sub.add_parser("info", help="show interfaces and endpoints")

    args = ap.parse_args()
    dev = open_device(args.vid, args.pid)
    if args.cmd == "info":
        return cmd_info(dev)
    if args.cmd == "stats":
        try:
            dev.ctrl_transfer(TYPE_VENDOR_OUT, REQ_LOGSTAT, 0, LOG_INTERFACE, None,
                              timeout=5000)
            data = bytes(dev.read(LOG_EP_IN, 64, timeout=3000))
        except usb.core.USBTimeoutError as exc:
            print("zxlogctl: stats request timed out: %s" % exc, file=sys.stderr)
            return 4
        except usb.core.USBError as exc:
            print("zxlogctl: stats request failed: %s" % exc, file=sys.stderr)
            return 1
        if len(data) < 16:
            print("zxlogctl: short stats response (%d bytes)" % len(data),
                  file=sys.stderr)
            return 1
        import struct as _s
        head, tail, seq, dropped = _s.unpack("<IIII", data[:16])
        print("head %u  tail %u  seq %u  dropped %u  bytes_in_ring %u"
              % (head, tail, seq, dropped, head - tail))
        return 0
    if args.cmd == "run":
        try:
            dev.ctrl_transfer(TYPE_VENDOR_OUT, REQ_RUN, 0, LOG_INTERFACE,
                              args.command.encode() + b"\0", timeout=5000)
        except usb.core.USBTimeoutError as exc:
            print("zxlogctl: run request timed out: %s" % exc, file=sys.stderr)
            return 4
        except usb.core.USBError as exc:
            print("zxlogctl: run request failed: %s" % exc, file=sys.stderr)
            return 1
        if not args.follow:
            return 0
        return dump(dev, 1)
    return dump(dev, args.from_seq)


if __name__ == "__main__":
    sys.exit(main())
