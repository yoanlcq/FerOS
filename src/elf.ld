/*
 * Quick summary of the Multiboot header fields enabled with MB_AOUT_KLUDGE:
 *
 * u32 mbh_header_addr = address of Multiboot header (address of magic value)
 * u32 mbh_load_addr = beginning of text segment.
 *     Note this may include the Multiboot header itself. The bootloader only
 *     uses this to know where to start loading data, including the
 *     `.text`, `.rodata` and `.data` sections.
 *     IMPORTANT: Also the spec requires that `mbh_load_addr` <= `mbh_header_addr`.
 * u32 mbh_load_end_addr = end of text and data segment.
 * u32 mbh_bss_end_addr = end of bss segment (comes after last data segment
 *     (I believe ? It's not mentioned but looks safe to assume))
 * u32 mbh_entry_addr = address of kernel entry point
 */


OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
	. = 1M;

	.text ALIGN(4K) : AT(ADDR(.text)) {
        mbh_load_addr = .;
		build/boot.S.o(.text*)
		*(.text*)
	}

	/* Read-only data (initialized) */
	.rodata ALIGN(4K) : AT(ADDR(.rodata)) {
		*(.rodata*)
		*(.rdata*)
        *(.assets*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data)) {
		*(.data*)
	}

	/* Read-write data (uninitialized) and stack. */
	.bss ALIGN(4K) : AT(ADDR(.bss)) {
        mbh_load_end_addr = .;
		*(COMMON*)
		*(.bss*)
	}
    mbh_bss_end_addr = .;
    /* ^ Note that Multiboot bootloaders use this to clear the
     * `.bss` section to zero. Guess we shouldn't rely on it in
     * our code, in case we drop Multiboot in the future.
     */
}
