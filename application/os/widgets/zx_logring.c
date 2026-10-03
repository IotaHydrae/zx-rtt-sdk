/*
 * A log ring that survives from boot, and a way to read it.
 *
 * The UART console is 115200 baud, about 11.5 KB/s, which is both too slow to
 * read a boot's worth of logs and intrusive enough to change what is being
 * measured -- a single rt_kprintf per frame was measured at ~5.2 ms and cost
 * 20.34 -> 1.37 MB/s on the USB channel.  So the logs go into a ring and the
 * host pulls them out at USB speed instead of watching the console.
 *
 * Two properties the design has to have:
 *
 *   - It must be up from the very start, or "since boot" misses the beginning.
 *     ulog's own backends register at INIT_PREV_EXPORT and so does this one;
 *     at that point there is no heap yet, hence the static buffer.
 *   - The host must be able to tell whether it missed anything, so every
 *     record carries a sequence number.  A gap in the sequence is a dropped
 *     record; the alternative (silently losing lines) is what makes console
 *     logs untrustworthy in the first place.
 */
#include <rtthread.h>
#include <rthw.h>
#include <ulog.h>

#define ZXRING_SIZE      (256 * 1024)
#define ZXRING_MAX_LINE  ULOG_LINE_BUF_SIZE

/* seq + len + payload.  Records are variable length and wrap around the end. */
struct zxring_rec {
    rt_uint32_t seq;
    rt_uint16_t len;
    char data[ZXRING_MAX_LINE];
};

static char zxring_buf[ZXRING_SIZE];
static rt_uint32_t zxring_head;   /* next byte to write */
static rt_uint32_t zxring_tail;   /* next byte to read (dump starts here) */
static rt_uint32_t zxring_seq;    /* last sequence number written */
static rt_uint32_t zxring_dropped;/* records that did not fit */

static struct ulog_backend zxring_backend = { 0 };

/* One record occupies 6 + len bytes, or 6 when it cannot fit and is counted as
 * dropped.  Alignment is not needed: reads and writes both go through here. */
#define ZXRING_REC_HDR ((rt_uint32_t)(sizeof(rt_uint32_t) + sizeof(rt_uint16_t)))

static void zxring_write(const char *data, rt_size_t len)
{
    rt_base_t level;
    rt_uint32_t need, room;
    struct zxring_rec *rec;

    if (len > ZXRING_MAX_LINE)
        len = ZXRING_MAX_LINE;

    need = ZXRING_REC_HDR + len;

    level = rt_hw_interrupt_disable();

    room = (zxring_head >= zxring_tail)
         ? ZXRING_SIZE - (zxring_head - zxring_tail)
         : (zxring_tail - zxring_head);

    /* Keep one record's worth free so head == tail always means "empty". */
    if (room <= need + ZXRING_REC_HDR) {
        zxring_dropped++;
        rt_hw_interrupt_enable(level);
        return;
    }
    if (zxring_head + need > ZXRING_SIZE) {
        /* Not enough room at the end: skip to the start and lose the tail. */
        zxring_head = 0;
    }

    rec = (struct zxring_rec *)(zxring_buf + zxring_head);
    rec->seq = ++zxring_seq;
    rec->len = (rt_uint16_t)len;
    rt_memcpy(rec->data, data, len);
    zxring_head += need;

    rt_hw_interrupt_enable(level);
}

/*
 * Console capture, so that rt_kprintf reaches the ring too.
 *
 * The ulog backend above only sees what goes through ulog.  The boot banner,
 * msh command output and a good deal of the SDK use rt_kprintf, which does not
 * pass through ulog at all -- so reading the ring would miss exactly the
 * output a person wants when running a command.  This registers a console
 * device instead: rt_kprintf routes to _console_device when one is set, so
 * everything printed is captured, and the text is still forwarded to the real
 * UART through rt_hw_console_output() exactly as before.
 */
static struct rt_device zxconsole_dev;
/* How many times the console path actually reached us.  If this stays 0
 * while rt_kprintf output appears on the UART, the console is not ours. */
static rt_uint32_t zxconsole_writes;

static rt_err_t zxconsole_open(rt_device_t dev, rt_uint16_t oflag)
{
    (void)dev; (void)oflag;
    return RT_EOK;
}

static rt_size_t zxconsole_write(rt_device_t dev, rt_off_t pos, const void *buf,
                                 rt_size_t size)
{
    (void)dev;
    (void)pos;

    if (buf && size) {
        zxconsole_writes++;
        zxring_write((const char *)buf, size);
        /*
         * rt_hw_console_output() takes a C string, and it is the same call the
         * no-console-device path would have made, so the UART keeps working
         * the way it did.  The buffer is not guaranteed to be terminated --
         * rt_kprintf's own buffer is, but rt_kputs callers need not be.
         */
        {
            static char line[ZXRING_MAX_LINE + 1];
            rt_size_t n = size;

            if (n > ZXRING_MAX_LINE)
                n = ZXRING_MAX_LINE;
            rt_memcpy(line, buf, n);
            line[n] = '\0';
            rt_hw_console_output(line);
        }
    }
    return size;
}

/*
 * This build sets RT_USING_DEVICE_OPS, so rt_device carries an ops table
 * rather than the per-operation members (that is also what adbd tripped over:
 * it still assigns rt_device.fops).
 */
static const struct rt_device_ops zxconsole_ops = {
    .open = zxconsole_open,
    .write = zxconsole_write,
};

static void zxconsole_init(void)
{
    zxconsole_dev.type = RT_Device_Class_Char;
    zxconsole_dev.ops = &zxconsole_ops;
    if (rt_device_register(&zxconsole_dev, "zxconsole", RT_DEVICE_FLAG_RDWR) != RT_EOK)
        return;
    rt_console_set_device("zxconsole");
}

/*
 * Claim the console again at the last init level.
 *
 * Registering at board level is not enough: the BSP installs its own uart0 as
 * the console later, which silently takes the routing back -- measured, not
 * assumed: rt_console_get_device() reported "uart0" while this device was
 * registered and had already received a handful of writes.  Re-asserting after
 * everything else has run leaves us last.
 */
static void zxconsole_claim(void)
{
    rt_device_t now = rt_console_get_device();

    if (now && now->parent.name && rt_strcmp(now->parent.name, "zxconsole") == 0)
        return;
    rt_console_set_device("zxconsole");
}

static void zxring_output(struct ulog_backend *backend, rt_uint32_t level,
                          const char *tag, rt_bool_t is_raw, const char *log,
                          rt_size_t len)
{
    (void)backend;
    (void)level;
    (void)tag;
    (void)is_raw;

    zxring_write(log, len);
}

static int zxring_init(void)
{
    zxring_backend.output = zxring_output;
    ulog_backend_register(&zxring_backend, "zxring", RT_FALSE);
    return 0;
}
INIT_PREV_EXPORT(zxring_init);
/* Console device has to come up after the UART it forwards to. */
INIT_BOARD_EXPORT(zxconsole_init);
INIT_APP_EXPORT(zxconsole_claim);

/*
 * Hand the consumer the oldest record whose sequence is >= from_seq.
 *
 * The ring is a byte queue, not an index, so this walks records in order --
 * fine for the few thousand a 256 KB ring holds, and it keeps the reader free
 * of any knowledge about wrap-around.  Returns the record's sequence number,
 * or 0 when there is nothing at or after from_seq.
 */
/*
 * The ring's own counters, for diagnosing "the writer ran but nothing comes
 * out" without guessing which half is at fault.
 */
/*
 * Append a line from outside this file.  Used by components that must report
 * their own startup outcome: a silent failure is what made "the task is not
 * running" take two rounds to establish.
 */
void zxring_puts(const char *s)
{
    if (s)
        zxring_write(s, rt_strlen(s));
}

rt_uint32_t zxring_console_writes(void)
{
    return zxconsole_writes;
}

/* Name of the device rt_kprintf is currently routed to, or "-" if none. */
const char *zxring_console_name(void)
{
    rt_device_t dev = rt_console_get_device();

    return (dev && dev->parent.name) ? dev->parent.name : "-";
}

void zxring_stats(rt_uint32_t *head, rt_uint32_t *tail, rt_uint32_t *seq,
                  rt_uint32_t *dropped)
{
    rt_base_t level = rt_hw_interrupt_disable();

    *head = zxring_head;
    *tail = zxring_tail;
    *seq = zxring_seq;
    *dropped = zxring_dropped;
    rt_hw_interrupt_enable(level);
}

/* Compile-time check that the record stride assumed by both sides is real.
 * A padded struct would make the writer and the walker disagree by exactly the
 * padding, which looks like "written but unreadable". */
typedef char zxring_stride_check[
    (sizeof(rt_uint32_t) + sizeof(rt_uint16_t) == ZXRING_REC_HDR) ? 1 : -1];
typedef char zxring_offset_check[
    ((rt_size_t)((char *)&((struct zxring_rec *)0)->data - (char *)0) == ZXRING_REC_HDR) ? 1 : -1];

rt_uint32_t zxring_next(rt_uint32_t from_seq, char *buf, rt_size_t cap, rt_size_t *len)
{
    rt_base_t level;
    rt_uint32_t pos, end, found = 0;

    level = rt_hw_interrupt_disable();
    pos = zxring_tail;
    end = zxring_head;

    while (pos < end) {
        const struct zxring_rec *rec = (const struct zxring_rec *)(zxring_buf + pos);

        if (rec->seq >= from_seq) {
            rt_size_t n = rec->len;

            if (n > cap)
                n = cap;
            rt_memcpy(buf, rec->data, n);
            *len = n;
            found = rec->seq;
            break;
        }
        pos += ZXRING_REC_HDR + rec->len;
    }

    rt_hw_interrupt_enable(level);
    return found;
}

/*
 * Dump and/or clear.  Deliberately the same shape as `logcat`: no arguments
 * prints everything held, `-c` clears, and once the host is reading over USB
 * this is where a sequence-numbered incremental read will plug in.
 */
static int logcat(int argc, char **argv)
{
    rt_base_t level;
    rt_uint32_t pos, end;
    rt_uint32_t dropped;

    level = rt_hw_interrupt_disable();
    if (argc > 1 && rt_strcmp(argv[1], "-c") == 0) {
        zxring_head = zxring_tail = 0;
        zxring_seq = 0;
        zxring_dropped = 0;
        rt_hw_interrupt_enable(level);
        rt_kprintf("logcat: cleared\n");
        return 0;
    }
    pos = zxring_tail;
    end = zxring_head;
    dropped = zxring_dropped;
    rt_hw_interrupt_enable(level);

    while (pos < end) {
        const struct zxring_rec *rec = (const struct zxring_rec *)(zxring_buf + pos);

        rt_kprintf("[%u] ", (unsigned)rec->seq);
        rt_kprintf("%.*s", (int)rec->len, rec->data);
        pos += ZXRING_REC_HDR + rec->len;
    }
    rt_kprintf("logcat: %u records, %u dropped\n",
               (unsigned)zxring_seq, (unsigned)dropped);
    return 0;
}
MSH_CMD_EXPORT(logcat, dump the log ring (logcat -c clears));
