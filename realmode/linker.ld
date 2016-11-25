OUTPUT_FORMAT(binary)
ENTRY(bootloader_start)
SECTIONS
{
    .start  : { *(.start) }
	.text   : { *(.text) }
	.rodata : { *(.rodata) }
	.rdata  : { *(.rdata) }
	.data   : { *(.data) }
    .bootsector0_signature 510 : { BYTE(0x55) BYTE(0xAA) }
}
