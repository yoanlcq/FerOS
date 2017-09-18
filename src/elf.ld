OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
	. = 1M;

	.text ALIGN(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text*)
	}

	/* Read-only data (initialized) */
	.rodata ALIGN(4K) : ALIGN(4K) {
		*(.rodata*)
		*(.rdata*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : ALIGN(4K) {
		*(.data*)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : ALIGN(4K) {
		*(COMMON*)
		*(.bss*)
		*(.bootstrap_stack*)
	}
}
