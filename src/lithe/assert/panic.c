#include <lithe/assert/panic.h>
#include <lithe/base/attributes.h>
#include <lithe/base/defs.h>

noreturn void _panic(src_location location, str fmt, print_args args) {
	UNUSED(location), UNUSED(fmt), UNUSED(args);

loop:
	__asm__ __volatile__("cli");
	__asm__ __volatile__("hlt");
	goto loop;
}
