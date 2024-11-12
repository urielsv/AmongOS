OUTPUT_FORMAT("binary")
ENTRY(loader)
SECTIONS
{
	.text 0x100000 :
	{
		text = .;
		*(.text*)
		. = ALIGN(0x1000);
		rodata = .;
		*(.rodata*)
	}
	.data ALIGN(0x1000) : AT(ADDR(.data))
	{
		data = .;
		*(.data*)
		end_of_kernel_binary = .;
	}
	.bss ALIGN(0x1000) : AT(ADDR(.bss))
	{
		bss = .;
		*(.bss*)
		*(EXCLUDE_FILE (*.o) COMMON)
	}
	. = ALIGN(0x1000);
	end_of_kernel = .;
}