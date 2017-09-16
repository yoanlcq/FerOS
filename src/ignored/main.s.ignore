.include "macros.s"
	.section .rodata
	.global hello_world
	.type hello_world, @common
hello_world:
	.asciz "Hello World! It works! It's awesome!"

	.section .text
.fn main
	lea eax, [hello_world]
	call vga_puts

	pushd 48
	pop eax
	call vga_putc
	ret

