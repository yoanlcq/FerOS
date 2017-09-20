OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(_start)
mbh_entry_addr = _start;

__phys = 1M;

SECTIONS {
	.text __phys : AT(__phys) {
        mbh_load_addr = .;
		build/boot.S.o(.text*)
		*(.text*)
        mbh_header_addr = .;
		*(.multiboot)
		*(.rodata*)
		*(.rdata*)
        . = ALIGN(4K);
	}

	.data : AT(__phys + __data - mbh_load_addr) {
        __data = .;
		*(.data*)
        mbh_load_end_addr = .;
        . = ALIGN(4K);
	}

	.bss : AT(__phys + __bss - mbh_load_addr) {
        __bss = .;
		*(COMMON*)
		*(.bss*)
		*(.bootstrap_stack*)
        . = ALIGN(4K);
        mbh_bss_end_addr = .;
	}
}
