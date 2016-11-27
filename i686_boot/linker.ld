OUTPUT_FORMAT(binary)
ENTRY(boot)
SECTIONS
{
    .start  : { *(.start) }
	.text   : { *(.text) }
	.rodata : { *(.rodata) }
	.rdata  : { *(.rdata) }
	.data   : { *(.data) }
    .bootsig 510 : { BYTE(0x55) BYTE(0xAA) }
}
