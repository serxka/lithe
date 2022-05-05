OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_start)

SECTIONS
{
	. = 0x80100000;

	PROVIDE(_start = .);
	.text ALIGN(4K) :
	{
		PROVIDE(_text_start = .);
		*(.text.boot)
		*(.text .text.*)
	}

	.rodata ALIGN(4K) :
	{
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
	}

	.data ALIGN(4K) :
	{
		PROVIDE(_data_start = .);
		*(.rodata .rodata.*)
		*(.data .data.*)
	}

	.bss :
	{
		PROVIDE(_bss_start = .);
		*(COMMON)
		*(.bss .bss.*)
	}
	PROVIDE(_end = .);

	. = ALIGN(4K);

	. += 16K;
	PROVIDE(_stack = .);

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
