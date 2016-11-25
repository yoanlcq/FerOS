.code16gcc
.section .rodata
hello:.asciz "Hello World!\r\n"
.text
.global _start
.set STACKTOP,0x7C00
_start:
    xor ax, ax
    mov es, ax

    mov ax, STACKTOP>>4
    mov ds, ax
    add ax, 512>>4
    mov ss, ax
    mov esp, 4096

    lea si, [hello]
    call print_string
    call do_stuff
    call test_func
    lea si, [hello2]
    call print_string
    call return_deadc0de
    lea si, [hello3]
    call print_string
foo:
    jmp foo
print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp print_string
done:
    ret
test_func:
    push    ebp
    mov ebp, esp
    mov eax, 1145128260
    pop ebp
    ret

