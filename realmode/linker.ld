OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS
{
	.text : { kernel.s.o (.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
    bootsig 510 : { BYTE(0x55) BYTE(0xAA) }
}
