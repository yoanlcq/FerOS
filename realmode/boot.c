#define LFTAB "\n\t"
#define asm __asm__
#define SECTION(sec) __attribute__((section(#sec)))
#define SECTOR_MEM 0x7C00
#define SECTOR_SIZE 512
#define STACK_SIZE 4096

#include <stdnoreturn.h>

const char hello[] = "Hello, Real Mode World!";

static void putchar(char c) {
    asm (
        "mov al, %0"   LFTAB
        "mov ah, 0x0E" LFTAB
        "int 0x10"     LFTAB
        : : "r"(c)
    );
}

static void puts(const char *str) {
    for(; *str ; ++str) 
        putchar(*str);
    putchar('\r');
    putchar('\n');
}

noreturn static void hang() { for(;;); }

#define SECTOR_START ((SECTOR_MEM)/16)
#define SECTOR_END   ((SECTOR_MEM+SECTOR_SIZE)/16)

SECTION(.start) void boot(void) {
    asm (
        "xor ax, ax"  LFTAB
        "mov es, ax"  LFTAB
        "mov ax, %0"  LFTAB
        "mov ds, ax"  LFTAB
        "mov ax, %1"  LFTAB
        "mov ss, ax"  LFTAB
        "mov esp, %2" LFTAB
        : : "i"(SECTOR_START), "i"(SECTOR_END), "i"(STACK_SIZE)
    );
    puts(hello);
    hang();
}
