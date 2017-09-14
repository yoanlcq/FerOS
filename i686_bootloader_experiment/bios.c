#include <bios.h>

void bioscall(uint8_t func, biosregs *r) {
    asm volatile (
        "mov ax, %3" LFTAB
        "mov dl, %4" LFTAB
        "mov si, %5" LFTAB
        "mov uglyhack+1, %2" LFTAB // XXX Flush pipeline after that ??
        "jmp uglyhack" LFTAB // http://blog.onlinedisassembler.com/blog/?p=133
        "uglyhack:"  LFTAB
        "int 0"      LFTAB
        "mov %0, ah" LFTAB
        "lahf" LFTAB
        "mov %1, ah" LFTAB
        : "=m"(r->ah), "=m"(r->cf_lo) 
        : "r"(func), "m"(r->ax), "m"(r->dl), "m"(r->si)
    );
}

int bios_drive_get_status(void) {
    biosregs regs = {.ah=1};
    bioscall(0x13, &regs);
    return regs.ah;
}

typedef struct {
    uint8_t size;
    uint8_t unused;
    uint16_t sector_count;
    void    *buffer;
    uint64_t first_sector;
} dap;
static_assert(offsetof(dap, size)         == 0, "");
static_assert(offsetof(dap, unused)       == 1, "");
static_assert(offsetof(dap, sector_count) == 2, "");
static_assert(offsetof(dap, buffer)       == 4, "");
static_assert(offsetof(dap, first_sector) == 8, "");
static_assert(sizeof(dap) == 16, "");

int bios_read_sectors_ex(bios_drive_index drive_index, void *buf, 
                          uint64_t first_sector, uint16_t sector_count) 
{
    dap p = { sizeof(dap), 0, sector_count, buf, first_sector };
    biosregs regs = { .ah=0x42, .dl=drive_index, .si=(uintptr_t)&p};
    bioscall(0x13, &regs);
    return regs.cf ? regs.ah : -1;
}
