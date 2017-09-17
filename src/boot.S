/*
 * Declares a Multiboot header.
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#boot_002eS
 */

#include <macros.S>
#include <multiboot.h>


	.set mbh_magic,			0x1BADB002
#ifdef WANTS_VIDEOMODE /* Defined at build-time, in the Makefile */
	.set mbh_flags,			(MB_PAGE_ALIGN | MB_MEMORY_INFO | MB_VIDEO_MODE)
#else
	.set mbh_flags,			(MB_PAGE_ALIGN | MB_MEMORY_INFO)
#endif
	.set mbh_checksum,		-(mbh_magic + mbh_flags)
	/* The following are only used if the MB_AOUT_KLUDGE flag is set. */
	.set mbh_header_addr,	0
	.set mbh_load_addr,		0
	.set mbh_load_end_addr, 0
	.set mbh_bss_end_addr,	0
	.set mbh_entry_addr,	0
	/* 
	 * The following are only used if the MB_VIDEO_MODE flag is set.
	 * mbh_mode_type: 1 => Text mode, 0 => Linear graphics mode.
	 *                The Multiboot spec says it's only a hint.
	 * mbh_width|height: Could be 1024x768, but I'd rather have it fit in 
	 *                   my desktop...
	 * The VIDEOMODE_* constants are defined at build-time, in the Makefile.
	 */
	.set mbh_mode_type,		0
	.set mbh_width,			VIDEOMODE_WIDTH
	.set mbh_height,		VIDEOMODE_HEIGHT
	.set mbh_depth,			VIDEOMODE_DEPTH
	
	.section .multiboot
	.balign 32
	.long mbh_magic
	.long mbh_flags
	.long mbh_checksum
	.long mbh_header_addr
	.long mbh_load_addr
	.long mbh_load_end_addr
	.long mbh_bss_end_addr
	.long mbh_entry_addr
	.long mbh_mode_type
	.long mbh_width
	.long mbh_height
	.long mbh_depth

	.section .bootstrap_stack, "aw", @nobits
stack_bottom:
	.skip 65536 # 64 Kib
	#.skip 16384 # 16 Kib
stack_top:

	.section .text
.fn _start
	# Initialize stack pointer
	mov		esp, stack_top
	mov		ebp, stack_top

	# Reset EFLAGS (also sets DF to 0)
	pushd	0
	popf

	push	ebx # 32-bit physical address to Multiboot information structure
	push	eax # Must contain 0x2BADBOO2
	call	main

	cli
.L_hang:
	hlt
	jmp		.L_hang
.size _start, . - _start
