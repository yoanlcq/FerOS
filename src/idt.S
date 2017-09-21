#include <macros.S> /* .fn */

.fn idt_load
	lidt [idt_ptr]
	ret
.endfn idt_load

# `.irp` is a bit like a "foreach" macro.

.irp i, 0,1,2,3,4,5,6,7,9,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
.fn _isr_entry_\i
	cli
	pushd 0 # Dummy error code
	pushd \i # ISR n°i
    jmp .L_isr_dispatch
.endfn _isr_entry_\i
.endr

.irp i, 8,10,11,12,13,14
.fn _isr_entry_\i
	cli
	# An error code has implicitly been pushed
	pushd \i
    jmp .L_isr_dispatch
.endfn _isr_entry_\i
.endr

# Remapped IRQs
.irp i, 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
.fn _isr_entry_\i
	cli
	pushd 0 # Dummy error code
	pushd \i # ISR n°i
    jmp .L_isr_dispatch
.endfn _isr_entry_\i
.endr

.L_isr_dispatch:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   # Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp   # Push us the stack
    push eax
    call isr_dispatch

    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
