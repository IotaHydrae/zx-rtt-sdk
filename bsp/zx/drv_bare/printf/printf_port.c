#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <aic_common.h>

#define NANOPRINTF_IMPLEMENTATION 1
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_VISIBILITY_STATIC

#include "nanoprintf.h"

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	return npf_vsnprintf(buf, size, fmt, args);
}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i = vsnprintf(buf, size, fmt, args);

	if (i < size)
		return i;
	if (size != 0)
		return size - 1;
	return 0;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
    int i;

    va_start(args, fmt);
	i = vscnprintf(buf, U32_MAX, fmt, args);
	va_end(args);

	return i;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
    int i;

    va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

int vprintf(const char *fmt, va_list args)
{
	unsigned int i, c;
	char printbuffer[512], *p;

	/*
	 * For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);

	/* Handle error */
	if (i <= 0)
		return i;
	/* Print the string */
	p = printbuffer;
	for (;;) {
		c = *p;
		if (c == 0)
			break;
		if (putchar(c) < 0)
			break;
		p++;
	}

	return i;
}

int printf_port(const char *fmt, ...)
{
	va_list args;
	unsigned int i;

	va_start(args, fmt);
	i = vprintf(fmt, args);
	va_end(args);

	return i;
}

int printf(const char *format, ...) __attribute__ ((alias("printf_port")));
