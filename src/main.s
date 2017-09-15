	.set vga_fb, 0xB8000
	.set vga_w, 80
	.set vga_h, 25
	.set vga_black, 0
	.set vga_blue, 1
	.set vga_green, 2
	.set vga_cyan, 3
	.set vga_red, 4
	.set vga_magenta, 5
	.set vga_brown, 6
	.set vga_light_grey, 7
	.set vga_dark_grey, 8
	.set vga_light_blue, 9
	.set vga_light_green, 10
	.set vga_light_cyan, 11
	.set vga_light_red, 12
	.set vga_light_magenta, 13
	.set vga_light_brown, 14
	.set vga_white, 15

	.section .rdata
	.global hello_world
	.type hello_world, @common
hello_world:
	.asciz "Hello World! It works! It's awesome!"

	.section .text
	.global main
	.type main, @function
main:
	push ebp
	mov ebp, esp
	sub esp, 32

	# int i = 0;
	xor edx, edx
.L_iter_hello:
	# char c = hello_world[i];
	xor eax, eax
	mov al, byte ptr [hello_world+edx]

	# if c == 0 return;
	cmp al, 0
	je .L_return

	# vga_fb[i] = vga_make_entry(c, fg, bg);
	mov ebx, vga_red
	mov ecx, vga_light_brown
	call vga_make_entry
	mov word ptr [vga_fb+2*edx], ax

	# ++i;
	inc edx
	jmp .L_iter_hello
.L_return:	
	add esp, 32
	pop ebp
	ret

# fn vga_entry(ascii: u8, fg: u4, bg: u4) -> u16 { ascii | (fg<<8) | (bg<<12) }
# Params ascii (eax), fg (ebx), bg (ecx)
# Returns in eax
	.global vga_make_entry
	.type vga_make_entry, @function
vga_make_entry:
	sal bx, 8
	sal cx, 12
	or ax, bx
	or ax, cx
	ret
