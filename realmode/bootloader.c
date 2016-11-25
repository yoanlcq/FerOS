#define LFTAB "\n\t"
#define asm __asm__
#define SECTION(sec) __attribute__((section(#sec)))
#define SEC_MEM 0x7C00
#define SEC_SIZE 512
#define STACK_SIZE 4096

#include <stddef.h>
#include <stdint.h>

const char hello[] = "Hello, Real Mode World!";

static void putchar(char c) {
    asm volatile (
        "mov al, %0" LFTAB
        "mov ah, 0x0E" LFTAB
        "int 0x10" LFTAB
        : : "r"(c)
    );
}

static void puts(const char *str) {
    for(; *str ; ++str) 
        putchar(*str);
    putchar('\r');
    putchar('\n');
}
static void hang() { for(;;); }

typedef uint16_t u16;
#define SEC_START ((u16)((SEC_MEM)/16))
#define SEC_END   ((u16)((SEC_MEM+SEC_SIZE)/16))

SECTION(.start) void bootloader_start(void) {
    asm volatile (
        "mov es, %0"  LFTAB
        "mov ds, %1"  LFTAB
        "mov ss, %2"  LFTAB
        "mov esp, %3" LFTAB
        : : "r"((u16)0), "r"(SEC_START), "r"(SEC_END), "r"(STACK_SIZE)
    );
    puts(hello);
    hang();
}
