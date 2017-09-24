#pragma once

typedef struct _packed {
    u16 base_low16;
    u16 segment_selector;
    u8  always_zero;
    struct {
        u8 always_01110       :  5; // a.k.a 14 or 0xE
        u8 ring               :  2;
        u8 is_segment_present :  1;
    };
    u16 base_high16;
} IdtEntry;

static_assert(sizeof(IdtEntry)==8, "");

typedef struct _packed {
    u16 limit;
    u32 base;
} IdtPtr;

static_assert(sizeof(IdtPtr)==6, "");


extern IdtEntry idt[256];
extern const IdtPtr idt_ptr;

void idt_load();
void _cold idt_setup();

