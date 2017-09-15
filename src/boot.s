# Declares a Multiboot header.
# https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#boot_002eS

	.set mb_align,    1<<0             # align loaded modules on page boundaries
	.set mb_meminfo,  1<<1             # provide memory map
	# TODO mb_videomode ?
	.set mb_flags,    (mb_align | mb_meminfo)  # this is the Multiboot 'flag' field
	.set mb_magic,    0x1BADB002       # 'magic number' lets bootloader find the header
	.set mb_checksum, -(mb_magic + mb_flags) # checksum of above, to prove we are multiboot
	
	.section .multiboot
	.balign 32 # Aligne to 32-byte boundary
	.long mb_magic
	.long mb_flags
	.long mb_checksum
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 1024
	.long 768
	.long 32

	.section .bootstrap_stack, "aw", @nobits
stack_bottom:
	.skip 65536 # 64 Kib
	#.skip 16384 # 16 Kib
stack_top:

	.section .text
	.global	_start
	.type _start, @function
_start:
	# Initialise stack pointer
	mov		esp, stack_top
	mov		ebp, stack_top

	# Reset EFLAGS
	push 0x00000000l
	popf

	# Push pointer to Multiboot information structure
	push ebx
	# Push magic value
	push eax

	mov		esp, ebp
	sub		esp, 32
	call	main
	cli
.L_hang:
	hlt
	jmp		.L_hang
.size _start, . - _start
