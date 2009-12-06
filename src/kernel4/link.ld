OUTPUT_FORMAT("elf64-x86-64")

SECTIONS
{
	. = 0x100000;
	. = ALIGN(16);
	start_kernel = .;
	.multiboot :
	{
		. = ALIGN(16);
		*(.multiboot)
	}
	.text :
	{
		. = ALIGN(16);
		*(.text)
	}
	.data :
	{
		. = ALIGN(16);
		start_ctors = .;
		*(.ctors)
		end_ctors = .;

		. = ALIGN(16);
		start_dtors = .;
		*(.dtors)
		end_dtors = .;

		. = ALIGN(16);
		*(.data)
		*(.rodata)
	}
	.bss :
	{
		. = ALIGN(16);
		*(.bss)
	}
	. = ALIGN(16);
	end_kernel = .;
}
