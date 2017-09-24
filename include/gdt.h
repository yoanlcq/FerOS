#pragma once

// See section 3.4.5 of Intel's System Programming Guide
// This is also named "Segment Descriptor".
//
// `limit_*` are put together (20 bits) to specify the size of the segment.
// `base_*` are put together (32 bits) to specify the base address of the
// segment.
// `type` indicates the segment or gate type. Its interpretation and
// encoding is different for code, data, and system descriptors.
// `descriptor_type`: Is it a code-or-data segment ? (it's a system
// segment otherwise)
// `ring` (a.k.a DPL, Descriptor Privilege Level)
// `operand_size`: 1 => 32-bits, 0 => 16-bits (but see the manual)
// `granularity`: Scaling of segment limit. 1 => 4Kb, 0 => 1 byte.
// `ia32e_is_code64`: See the manual. Set to zero for non-code segments or
// when not in IA-32e mode.
// `available_for_us`: Exactly as it says. We can use it to store a custom
// bit.
typedef struct _packed {
    u16 limit_low16;
    u16 base_low16;
    u8  base_middle8;
    struct {
        u8 type               :  4;
        u8 descriptor_type    :  1;
        u8 ring               :  2;
        u8 is_segment_present :  1;
    };
    struct {
        u8 limit_high4        :  4;
        u8 available_for_us   :  1;
        u8 ia32e_is_code64    :  1;
        u8 operand_size       :  1;
        u8 granularity        :  1;
    };
    u8  base_high8;
} GdtEntry;

static_assert(sizeof(GdtEntry)==8, "");

typedef struct _packed {
    u16 limit; // (sizeof gdt) - 1
    u32 base; // Base address
} GdtPtr;

static_assert(sizeof(GdtPtr)==6, "");

void gdt_setup();
void gdt_load(); // Defined in assembly, src/gdt.S

extern GdtEntry gdt[3];
extern const GdtPtr gdt_ptr;

