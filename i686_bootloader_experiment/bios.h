#pragma once

#include <defs.h>

typedef uint8_t bios_drive_index;

#define BIOS_DRIVE_FLOPPY1 0x00
#define BIOS_DRIVE_FLOPPY2 0x01
#define BIOS_DRIVE_HDD1    0x80
#define BIOS_DRIVE_HDD2    0x81

#define GP_REG(letter) \
    union { \
        struct { \
            uint8_t letter##l; \
            uint8_t letter##h; \
        }; \
        uint16_t  letter##x; \
        uint32_t e##letter##x; \
    }
static_assert(offsetof(GP_REG(a),al)==0, "");
static_assert(offsetof(GP_REG(a),ah)==1, "");
typedef struct {
    GP_REG(a); GP_REG(d);
    uint16_t si;
    union {
        uint16_t cf;
        uint8_t cf_lo;
    };
} biosregs;

void bioscall(uint8_t func, biosregs *r);
int bios_drive_get_status(void);
int bios_read_sectors_ex(bios_drive_index drive_index, void *buf, 
                         uint64_t first_sector, uint16_t sector_count);
