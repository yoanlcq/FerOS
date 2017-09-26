#include <gdt.h>

// The three descriptors are:
// - The NULL descriptor     (offset 0x00);
// - The kernel code segment (offset 0x08);
// - The kernel data segment (offset 0x10);

GdtEntry gdt[3] = {0};

const GdtPtr gdt_ptr = {
    .limit = (sizeof gdt) - 1,
    .base = (u32) (uptr) gdt
};

void _cold _no_sse gdt_setup() {
    u32 limit = 0xfffff; // We don't have more than 20 bits to express it
    u32 base = 0x00000000;

    GdtEntry entry = {
        .base_low16         = base & 0xffff,
        .base_middle8       = (base>>16) & 0xff,
        .base_high8         = (base>>24) & 0xff,
        .limit_low16        = limit & 0xffff,
        .limit_high4        = (limit>>16) & 0xf,
        .is_segment_present = 0b1,
        .ring               = 0b00,
        .descriptor_type    = 0b1,
        .type               = 0b1010,
        .granularity        = 0b1,
        .operand_size       = 0b1,
        .ia32e_is_code64    = 0b0,
        .available_for_us   = 0b0
    };

    gdt[0] = (GdtEntry) {0};
    gdt[1] = entry;
    entry.type = 0b0010;
    gdt[2] = entry;

    gdt_load();
}
