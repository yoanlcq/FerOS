#include <macros.S>

.fn gdt_load
	lgdt [gdt_ptr] 
	mov ax, (2*8) /* Offset to our data segment (GDT entry n°2) */
	mov ds, ax 
	mov es, ax 
	mov fs, ax 
	mov gs, ax 
	mov ss, ax 
	/* Set `cs` to GDT entry n°1 by doing a far jump */
	ljmp (1*8), .L_farjump
.L_farjump: 
	ret 
.endfn gdt_load
