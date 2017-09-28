OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
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

    /*
    .rel.dyn : {
        *(.rel.text)
    }
    */

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
    /*
    .eh_frame : { *(.eh_frame) }
    */

	/* Read-write data (initialized) */
	.data ALIGN(4K) : {
        ks_data_start = .;
		*(.data*)
        ks_data_end = .;
	}

	/* Read-write data (uninitialized) and stack.
     * The data in this section is normally cleared to zero by multiboot
     * bootloaders. */
	.bss ALIGN(4K) : {
        mbh_load_end_addr = .;
        ks_bss_start = .;
		*(COMMON*)
		*(.bss*)
        ks_bss_end = .;
	}
    
    mbh_bss_end_addr = .;
    /* ^ Note that Multiboot bootloaders use this to clear the
     * `.bss` section to zero. Guess we shouldn't rely on it in
     * our code, in case we drop Multiboot in the future.
     */

/*
    .debug_info    : { *(.debug_info   ) }
    .debug_abbrev  : { *(.debug_abbrev ) } 
    .debug_aranges : { *(.debug_aranges) } 
    .debug_line    : { *(.debug_line   ) }
    .debug_str     : { *(.debug_str    ) }
    .comment       : { *(.comment      ) }
    .stab          : { *(.stab         ) } 
    .stabstr       : { *(.stabstr      ) } 
    .debug_loc     : { *(.debug_loc    ) }  
    .debug_ranges  : { *(.debug_ranges ) }
    .symtab        : { *(.symtab       ) }
    .strtab        : { *(.strtab       ) } 
    .shstrtab      : { *(.shstrtab     ) }   
    */

    ks_end = .;
}
