OUTPUT_FORMAT(binary)
ENTRY(boot)
SECTIONS
{
    .start   : { *(.start) }
	.text    : { *(.text) }
	.data    : { *(.data) }
	.rodata  : { *(.rodata) }
	.rdata   : { *(.rdata) }
	.bss     : { *(.bss) }
    .bootsig : AT(510) { BYTE(0x55) BYTE(0xAA) }
    .foobar : AT(512) { LONG(0x65656565) }
}
