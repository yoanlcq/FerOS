#define LFTAB "\n\t"
#define DECL_SECTION(sec) __attribute__((section(#sec)))
#define DECL_NOINLINE __attribute__((noinline))
#define SECTOR_MEM  0x7C00
#define SECTOR_SIZE 512
#define STACK_SIZE  4096

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

uint8_t boot_drive_number = 0;

static void putchar(char c) {
    asm (
        "mov al, %0"   LFTAB
        "mov ah, 0x0E" LFTAB
        "int 0x10"     LFTAB
        : : "q"(c) : "ah"
    );
}

static void puts(const char *str) {
    for(; *str ; ++str) 
        putchar(*str);
}
static void putline(const char *str) {
    puts(str);
    puts("\r\n");
}

noreturn static void hang() { for(;;); }

void main(void) {
    putline("Hello, Real Mode World!");
    puts("Boot drive number: ");
    char hexb[] = "0x  ";
    hexb[2] = '0' + (boot_drive_number>>4);
    hexb[3] = '0' + (boot_drive_number&0xf);
    putline(hexb);
}

#define SECTOR_START ((SECTOR_MEM)/16)
#define SECTOR_END   ((SECTOR_MEM+SECTOR_SIZE)/16)

// Needs to be compiled with -fomit-frame-pointer
DECL_SECTION(.start) void boot(void) {
    asm (
        "xor ax, ax"  LFTAB
        "mov es, ax"  LFTAB
        "mov ax, %1"  LFTAB
        "mov ds, ax"  LFTAB
        "mov ss, ax"  LFTAB
        "mov esp, %2" LFTAB
        "mov %0, dl"  LFTAB
        : "=m"(boot_drive_number) 
        : "i"(SECTOR_START), "i"(STACK_SIZE)
        : "ax", "esp"
    );
    main();
    hang();
}
