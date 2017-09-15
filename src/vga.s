.include "macros.s"
.include "vga_defs.s"

/*
vga_make_cell
	ascii: u8 (eax)
	fg: u4 (ebx)
	bg: u4 (ecx) 
return in eax, (u16) (ascii | (fg<<8) | (bg<<12))
*/
.fn vga_make_cell
	sal bx, 8
	sal cx, 12
	or ax, bx
	or ax, cx
	ret

/*
vga_puts
	str: asciz (eax)
*/
.fn vga_puts
	mov esi, eax
	# int i = 0;
	xor edi, edi
.L_iter:
	# char c = str[i];
	xor eax, eax
	mov al, byte ptr [esi+edi]

	# if c == '\0' return;
	cmp al, 0
	je .L_return

	# vga_fb[i] = vga_make_entry(c, fg, bg);
	mov ebx, vga_red
	mov ecx, vga_light_brown
	call vga_make_cell
	mov word ptr [vga_fb+2*edi], ax

	# ++i;
	inc edi
	jmp .L_iter
.L_return:
	ret

.fn vga_putc
	mov ebx, vga_green
	mov ecx, vga_light_blue
	call vga_make_cell
	mov word ptr [vga_fb], ax
	ret
