#include <kernel/generic.h>
#include <kernel/misc/kprintf.h>
#include <kernel/types.h>

int generic_main(void) {
	kprintf("generic_main()");
	
	while(true);
}
