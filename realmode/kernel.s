.code16gcc
.text
.global _start
.set STACKTOP,0x7C00
_start:
    mov eax, 0 # Not xor - crashes for some reason
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, STACKTOP # C functions use 'esp', not only 'sp', 
                      # so also zero first 2 bytes for safety

    lea si, [STACKTOP+hello]
    call print_string
    call do_stuff
    lea si, [STACKTOP+hello2]
    call print_string
    # Uncomment to test - but it just hangs :/
    # call return_deadc0de
    lea si, [STACKTOP+hello3]
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
hello:.asciz "Hello World!\r\n"
