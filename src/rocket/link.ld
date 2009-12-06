OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")

SECTIONS
{
	. = 0x200000;
	start = .;
	.multiboot :
	{
		*(.multiboot)
		. = ALIGN(16);
	}
	.text :
	{
		*(.text)
		. = ALIGN(16);
	}
	.data :
	{
		*(.data)
		. = ALIGN(16);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(16);
	}
	end = .;
}
