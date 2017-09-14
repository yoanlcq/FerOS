#include <defs.h>
#include <bios.h>

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

static uint8_t boot_drive_number = 0;

void main(void) {
    putline("Hello, Real Mode World!");
    //char buf[512];
    //buf[0] = 'x';
    //uint8_t ret = bios_read_sectors_ex(boot_drive_number, buf, 0, 1);
    //ret = bios_drive_get_status();
    //buf[8] = '\0';
    ////putline(buf);
    //buf[0] = '0' + (ret>>4);
    //buf[1] = '0' + (ret&0xf);
    //buf[2] = '\0';
    //putline(buf);
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
