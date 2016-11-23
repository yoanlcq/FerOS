OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS
{
	.text : { kernel.s.o (.text) }
	.data : { *(.data) }
    bootsig 510 : { ; BYTE(0x55) BYTE(0xAA) }
}
