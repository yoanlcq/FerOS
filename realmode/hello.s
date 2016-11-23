.data
.global hello2
hello2:.asciz "Hello Foo!\r\n"
.text
.global do_stuff
.type do_stuff,@function
do_stuff:
    mov eax, 42
    ret
