OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
	. = 1M;

	.boot :
	{
		*(.boot)
	}

	_kernel_vma = 0xFFFFffff80000000;
	. += _kernel_vma;

	.text ALIGN(4K) : AT (ADDR (.text) - _kernel_vma)
	{
		_text = . - _kernel_vma;
		
		*(.text .text.*)
	}

	.data ALIGN(4K) : AT (ADDR (.data) - _kernel_vma)
	{
		_data = . - _kernel_vma;
		*(.rodata .rodata.*)
		*(.data .data.*)
	}

	.bss ALIGN(4K) : AT (ADDR (.bss) - _kernel_vma)
	{
		_bss = . - _kernel_vma;
		*(COMMON)
		*(.bss .bss.*)
		. += 16K;
		_stack_bottom = .;
		_stack_bottom_phys = _stack_bottom - _kernel_vma;
	}

	_end = . - _kernel_vma;
	
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
