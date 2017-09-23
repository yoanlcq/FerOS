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

ks_phys_addr = 1M;

SECTIONS {
	. = ks_phys_addr;

	.text . : {
        mbh_load_addr = .;
        ks_text_start = .;
		build/boot.S.o(.text*)
		*(.text*)
        ks_text_end = .;
	}

	/* Read-only data (initialized) */
	.rodata ALIGN(4K) : {
        ks_rodata_start = .;
		*(.rodata*)
		*(.rdata*)
        ks_rodata_end = .;
        ks_assets_start = .;
        *(.assets*)
        ks_assets_end = .;
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : {
        ks_data_start = .;
		*(.data*)
        ks_data_end = .;
	}

	/* Read-write data (uninitialized) and stack. */
	.bss ALIGN(4K) : {
        mbh_load_end_addr = .;
        ks_bss_start = .;
		*(COMMON*)
		*(.bss*)
        ks_bss_end = .;
	}
    ks_end = .;
    mbh_bss_end_addr = .;
    /* ^ Note that Multiboot bootloaders use this to clear the
     * `.bss` section to zero. Guess we shouldn't rely on it in
     * our code, in case we drop Multiboot in the future.
     */
}
