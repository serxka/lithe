#include <kernel/common.h>
#include <kernel/kprintf.h>

#include <stdarg.h>

size_t (*kprintf_writter)(uint8_t *, size_t) = NULL;

#define PUTC(c)                   \
	do {                      \
		writter(data, c); \
		++written;        \
	} while (0)

size_t print_dec(uint64_t num, int (*writter)(void *, char), void *data) {
	size_t written = 0;

	u32 width = 1;
	u64 i = num;
	while (i >= 10UL) { // Test how wide the number is
		i /= 10UL;
		++width;
	}

	i = width;
	char tmp[33];
	while (i > 0) {
		u64 n = num / 10;
		u64 r = num % 10;
		tmp[i - 1] = r + '0';
		--i;
		num = n;
	}
	while (i < width)
		PUTC(tmp[i++]);

	return written;
}

size_t print_hex(u64 val, bool upper, bool zeros, int (*writter)(void *, char),
		 void *data) {
	size_t written = 0;

	u32 width = 1;
	u64 z = 0xF;
	// Test how wide the number is so we don't pad with zero
	while (val > z && z < UINT64_MAX) {
		width += 1;
		z <<= 4;
		z |= 0xF;
	}

	if (zeros)
		width = 16;

	PUTC('0');
	PUTC('x');

	while (width-- > 0) {
		char *set = upper ? "0123456789ABCDEF" : "0123456789abcdef";
		char c = set[(val >> (width * 4)) & 0xF];
		PUTC(c);
	}

	return written;
}

size_t wvaprintf(int (*writter)(void *, char), void *data, const char *fmt,
		 va_list ap) {
	size_t written = 0;
	while (*fmt != '\0') {
		// Fast path non-format character
		if (*fmt != '%') {
			PUTC(*fmt++);
			continue;
		}
		++fmt;
		switch (*fmt) {
		case 's': {
			char *s = (char *)va_arg(ap, char *);
			if (s == NULL)
				s = "(null)";
			while (*s != '\0')
				PUTC(*s++);
			break;
		}
		case 'c':
			PUTC(*fmt);
			break;
		case 'p':
			// Fall-through
		case 'X':
		case 'x': {
			unsigned long long value = (unsigned long long)(va_arg(
				ap, unsigned long long));
			bool upper = !(*fmt == 'x');
			bool zeros = *fmt == 'p';
			written +=
				print_hex(value, upper, zeros, writter, data);
			break;
		}
		case 'd': {
			int num = (int)va_arg(ap, int);
			if (num < 0) {
				PUTC('-');
				num = -num;
			}
			written += print_dec(num, writter, data);
			break;
		}
		case 'u': {
			unsigned int num =
				(unsigned int)va_arg(ap, unsigned int);
			written += print_dec(num, writter, data);
			break;
		}
		default:
			PUTC(*fmt);
			break;
		}
		++fmt;
	}

	return written;
}

typedef struct snwritter_s {
	char *str;
	size_t size; // Size of str
	size_t len; // Amount we have written
} snwritter_t;

static int sn_writter_fn(void *data, char c) {
	snwritter_t *self = (snwritter_t *)data;
	if (self->size > self->len + 1) // if we still have space left to write
		self->str[self->len++] = c;
	// null termination is done after written
	return 0;
}

static int kprintf_writter_fn(void *_data, char c) {
	kprintf_writter((uint8_t *)&c, 1);
	return 0;
}

int vsnkprintf(char *buf, size_t len, const char *fmt, va_list ap) {
	snwritter_t data = {buf, len, 0};
	int written = wvaprintf(sn_writter_fn, (void *)&data, fmt, ap);
	sn_writter_fn(&data, '\0');
	return written;
}

int kprintf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int written = wvaprintf(kprintf_writter_fn, NULL, fmt, args);
	va_end(args);

	return written;
}
