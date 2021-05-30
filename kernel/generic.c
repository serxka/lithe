#include <kernel/generic.h>
#include <kernel/kprintf.h>
#include <kernel/types.h>

int generic_main(void) {
	kprintf("generic_main()");
	
	while(true);
}
