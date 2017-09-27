/*
 * Declares a Multiboot header and the initialization code.
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#boot_002eS
 */

#include <macros.S> /* .fn */
#include <multiboot.h>

	.section .text
.fn _start
	jmp .L_start
.endfn _start

	.section .rodata
	.set mbh_magic,			0x1BADB002
	.set mbh_flags,			(MB_MEMORY_INFO | MB_VIDEO_MODE)
	.set mbh_checksum,		-(mbh_magic + mbh_flags)
	
	.section .text /* Used to be `.multiboot` */
	.balign 32
.L_multiboot:
	.long mbh_magic
	.long mbh_flags
	.long mbh_checksum
	/*
	 * MB_AOUT_KLUDGE-enabled fields. Symbols not defined in this file are
	 * set by the linker (see src/elf.ld).
	 */
	.long .L_multiboot /* mbh_header_addr */
	.long mbh_load_addr
	.long mbh_load_end_addr
	.long mbh_bss_end_addr
	.long _start /* mbh_entry_addr */
	/* 
	 * The following are only used if the MB_VIDEO_MODE flag is set.
	 * mbh_mode_type: 1 => Text mode, 0 => Linear graphics mode.
	 *                The Multiboot spec says it's only a hint.
	 * mbh_width|height: Could be 1024x768, but I'd rather have it fit in 
	 *                   my desktop.
	 *                   Also dramatically affects framerate.
	 */
#ifdef IS_QEMU_GUEST
	.long 0 /* mbh_mode_type */
	.long 320 /* mbh_width */
	.long 200 /* mbh_height */
	.long 32 /* mbh_depth */
#else
	/* NOTE: zero for a setting means "no preference". This is what we should
	 * do because on EFI we can only get a graphics mode and the width+height
	 * can be completely different from documented VBE modes.
	 */
	.long 0 /* mbh_mode_type */
	.long 0 /* mbh_width */
	.long 0 /* mbh_height */
	.long 0 /* mbh_depth */
#endif

	# XXX last bug has shown that stack and code overlap, which doesn't
	# happen if we put the stack in the text section.
#if 0
	.section .bss , "aw", @nobits
#else
	.section .text
#endif
	.balign 64
	.global ks_stack_bottom
ks_stack_bottom:
	.skip 4*65536 # 4*64 Kib
	.global ks_stack_top
ks_stack_top:

	.section .text
.L_start:

	# General note: Take care to preserve eax and ebx until calls to __init
	# and main!

	# Initialize stack pointer
	mov		esp, ks_stack_top
	mov		ebp, ks_stack_top	# Not strictly needed, but convenient

	# Reset EFLAGS
	pushd	0
	popf

	# Save Multiboot magic in eax into edx (because al is overwritten below)
	mov		edx, eax

#if 0 /* defined(IS_QEMU_GUEST) */
	# Clear stack with the ASCII byte for 'S' (useful for memory dumps)
	mov		al, 'S'
	lea		edi, ks_stack_bottom
	mov		ecx, (ks_stack_top - ks_stack_bottom)
	rep stosb
#endif

	push	ebx		# 32-bit physical address to Multiboot information structure
	push	edx		# Multiboot info magic, 0x2BADBOO2, saved from eax earlier
	call	__init	# __init(magic, info_ptr)

	add		esp, 4		# Pop 0x2BADBOO2
	call	main		# main(info_ptr)
	add		esp, 4		# Pop pointer to Multiboot info

	call	__deinit
	cli		# Clear Interrupts flag
.L_hang:
	hlt
	jmp		.L_hang
.size .L_start, (. - .L_start)
