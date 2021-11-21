#include <kernel/kprintf.h>

void kmain(void)
{
	kprintf("kmain()");

	while (true)
		;
}
