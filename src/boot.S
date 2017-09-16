/*
 * Declares a Multiboot header.
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#boot_002eS
 */

#include <multiboot.h>

	.set mbh_magic,			0x1BADB002
	.set mbh_flags,			(MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
	.set mbh_checksum,		-(mbh_magic + mbh_flags)
	/* The following are only used if the MULTIBOOT_AOUT_KLUDGE flag is set. */
	.set mbh_header_addr,	0
	.set mbh_load_addr,		0
	.set mbh_load_end_addr, 0
	.set mbh_bss_end_addr,	0
	.set mbh_entry_addr,	0
	/* The following are only used if the MULTIBOOT_VIDEO_MODE flag is set. */
	.set mbh_mode_type,		0
	.set mbh_width,			1024
	.set mbh_height,		768
	.set mbh_depth,			32
	
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

	# Reset EFLAGS
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
