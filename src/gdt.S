#include <macros.S> /* .fn */

.fn gdt_update
    lgdt [gdt_ptr]
    mov ax, 0x10      # 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ljmp 0x08, .L_farjump   # 0x08 is the offset to our code segment: Far jump!
.L_farjump:
    ret               # Returns back to the C code!
.endfn gdt_update
