#include <idt.h>
#include <irq.h>
#include <isr.h>
#include <mouse.h>

IdtEntry idt[256] = {0};

const IdtPtr idt_ptr = {
    .limit = (sizeof idt) - 1,
    .base = (u32) (uptr) idt
};

static void isr0_divide_error_exception(IsrContext* ctx) {(void)ctx;}
static void isr1_debug_exception(IsrContext* ctx) {(void)ctx;}
static void isr2_nmi_interrupt(IsrContext* ctx) {(void)ctx;}
static void isr3_breakpoint_exception(IsrContext* ctx) {(void)ctx;}
static void isr4_overflow_exception(IsrContext* ctx) {(void)ctx;}
static void isr5_bound_range_exceeded_exception(IsrContext* ctx) {(void)ctx;}
static void isr6_invalid_opcode_exception(IsrContext* ctx) {(void)ctx;}
static void isr7_device_not_available_exception(IsrContext* ctx) {(void)ctx;}
static void isr8_double_fault_exception(IsrContext* ctx) {(void)ctx;}
static void isr9_coprocessor_segment_overrun(IsrContext* ctx) {(void)ctx;}
static void isr10_invalid_tss_exception(IsrContext* ctx) {(void)ctx;}
static void isr11_segment_not_present(IsrContext* ctx) {(void)ctx;}
static void isr12_stack_fault_exception(IsrContext* ctx) {(void)ctx;}
static void isr13_general_protection_exception(IsrContext* ctx) {(void)ctx;}
static void isr14_page_fault_exception(IsrContext* ctx) {(void)ctx;}
static void isr15_unknown_interrupt(IsrContext* ctx) {(void)ctx;}
static void isr16_x87_fpu_error(IsrContext* ctx) {(void)ctx;}
static void isr17_alignment_check_exception(IsrContext* ctx) {(void)ctx;}
static void isr18_machine_check_exception(IsrContext* ctx) {(void)ctx;}
static void isr19_simd_floatingpoint_exception(IsrContext* ctx) {(void)ctx;}
static void isr20_virtualization_exception(IsrContext* ctx) {(void)ctx;}
static void isrXX_reserved(IsrContext* ctx) {(void)ctx;}
#define isr21_reserved isrXX_reserved
#define isr22_reserved isrXX_reserved
#define isr23_reserved isrXX_reserved
#define isr24_reserved isrXX_reserved
#define isr25_reserved isrXX_reserved
#define isr26_reserved isrXX_reserved
#define isr27_reserved isrXX_reserved
#define isr28_reserved isrXX_reserved
#define isr29_reserved isrXX_reserved
#define isr30_reserved isrXX_reserved
#define isr31_reserved isrXX_reserved
static volatile f32 irq0_timer_frequency = 18.222f;
static volatile u64 irq0_timer_ticks = 0;
u64 irq0_get_timer_ticks() {
    return irq0_timer_ticks;
}
f32 irq0_get_timer_frequency() {
    return irq0_timer_frequency;
}

static void irq0 (IsrContext* ctx) {
    (void)ctx;
    ++irq0_timer_ticks;
    if(irq0_timer_ticks % ((u32)irq0_timer_frequency) == 0) {
        // logd("Approximately one second has passed!");
    }
}

static void irq1 (IsrContext* ctx) {
    (void)ctx;
    u8 scancode = inb(0x60);
    if(scancode & 0x80) {
        scancode &= ~0x80;
        logd("Released key (scancode = ", (u32)scancode, ")");
    } else {
        logd("Pressed key (scancode = ", (u32)scancode, ")");
    }
}
static void irq2 (IsrContext* ctx) {(void)ctx;}
static void irq3 (IsrContext* ctx) {(void)ctx;}
static void irq4 (IsrContext* ctx) {(void)ctx;}
static void irq5 (IsrContext* ctx) {(void)ctx;}
static void irq6 (IsrContext* ctx) {(void)ctx;}
static void irq7 (IsrContext* ctx) {(void)ctx;}
static void irq8 (IsrContext* ctx) {(void)ctx;}
static void irq9 (IsrContext* ctx) {(void)ctx;}
static void irq10(IsrContext* ctx) {(void)ctx;}
static void irq11(IsrContext* ctx) {(void)ctx;}
static void irq12(IsrContext* ctx) {(void)ctx; mouse_handler(ctx); }
static void irq13(IsrContext* ctx) {(void)ctx;}
static void irq14(IsrContext* ctx) {(void)ctx;}
static void irq15(IsrContext* ctx) {(void)ctx;}


// Entry points, defined in assembly (`src/idt.S`)
//
// Processor-reserved interrupts
void _isr_entry_0();
void _isr_entry_1();
void _isr_entry_2();
void _isr_entry_3();
void _isr_entry_4();
void _isr_entry_5();
void _isr_entry_6();
void _isr_entry_7();
void _isr_entry_8();
void _isr_entry_9();
void _isr_entry_10();
void _isr_entry_11();
void _isr_entry_12();
void _isr_entry_13();
void _isr_entry_14();
void _isr_entry_15();
void _isr_entry_16();
void _isr_entry_17();
void _isr_entry_18();
void _isr_entry_19();
void _isr_entry_20();
void _isr_entry_21();
void _isr_entry_22();
void _isr_entry_23();
void _isr_entry_24();
void _isr_entry_25();
void _isr_entry_26();
void _isr_entry_27();
void _isr_entry_28();
void _isr_entry_29();
void _isr_entry_30();
void _isr_entry_31();

// Remapped IRQ 0 through IRQ 15
void _isr_entry_32();
void _isr_entry_33();
void _isr_entry_34();
void _isr_entry_35();
void _isr_entry_36();
void _isr_entry_37();
void _isr_entry_38();
void _isr_entry_39();
void _isr_entry_40();
void _isr_entry_41();
void _isr_entry_42();
void _isr_entry_43();
void _isr_entry_44();
void _isr_entry_45();
void _isr_entry_46();
void _isr_entry_47();


typedef struct {
    const char *name;
    void (*entry)();
    void (*handler)(IsrContext*);
} IsrSpec;


static const IsrSpec isrs[] = {
    { .entry = _isr_entry_0 , .name = "Divide error"               , .handler = isr0_divide_error_exception },
    { .entry = _isr_entry_1 , .name = "Debug"                      , .handler = isr1_debug_exception },
    { .entry = _isr_entry_2 , .name = "Non-Maskable Interrupt"     , .handler = isr2_nmi_interrupt },
    { .entry = _isr_entry_3 , .name = "Breakpoint"                 , .handler = isr3_breakpoint_exception },
    { .entry = _isr_entry_4 , .name = "INTO-detected overflow"     , .handler = isr4_overflow_exception },
    { .entry = _isr_entry_5 , .name = "Out of bounds"              , .handler = isr5_bound_range_exceeded_exception },
    { .entry = _isr_entry_6 , .name = "Invalid opcode"             , .handler = isr6_invalid_opcode_exception },
    { .entry = _isr_entry_7 , .name = "Device not available"       , .handler = isr7_device_not_available_exception },
    { .entry = _isr_entry_8 , .name = "Double fault"               , .handler = isr8_double_fault_exception },
    { .entry = _isr_entry_9 , .name = "Coprocessor segment overrun", .handler = isr9_coprocessor_segment_overrun },
    { .entry = _isr_entry_10, .name = "Invalid TSS"                , .handler = isr10_invalid_tss_exception },
    { .entry = _isr_entry_11, .name = "Segment not present"        , .handler = isr11_segment_not_present },
    { .entry = _isr_entry_12, .name = "Stack fault"                , .handler = isr12_stack_fault_exception },
    { .entry = _isr_entry_13, .name = "General protection"         , .handler = isr13_general_protection_exception },
    { .entry = _isr_entry_14, .name = "Page fault"                 , .handler = isr14_page_fault_exception },
    { .entry = _isr_entry_15, .name = "Unknown interrupt"          , .handler = isr15_unknown_interrupt },
    { .entry = _isr_entry_16, .name = "x86 FPU floating-point"     , .handler = isr16_x87_fpu_error },
    { .entry = _isr_entry_17, .name = "Alignment check"            , .handler = isr17_alignment_check_exception },
    { .entry = _isr_entry_18, .name = "Machine check"              , .handler = isr18_machine_check_exception },
    { .entry = _isr_entry_19, .name = "SIMD floating-point"        , .handler = isr19_simd_floatingpoint_exception },
    { .entry = _isr_entry_20, .name = "Virtualization"             , .handler = isr20_virtualization_exception },
    { .entry = _isr_entry_21, .name = "Reserved"                   , .handler = isr21_reserved },
    { .entry = _isr_entry_22, .name = "Reserved"                   , .handler = isr22_reserved },
    { .entry = _isr_entry_23, .name = "Reserved"                   , .handler = isr23_reserved },
    { .entry = _isr_entry_24, .name = "Reserved"                   , .handler = isr24_reserved },
    { .entry = _isr_entry_25, .name = "Reserved"                   , .handler = isr25_reserved },
    { .entry = _isr_entry_26, .name = "Reserved"                   , .handler = isr26_reserved },
    { .entry = _isr_entry_27, .name = "Reserved"                   , .handler = isr27_reserved },
    { .entry = _isr_entry_28, .name = "Reserved"                   , .handler = isr28_reserved },
    { .entry = _isr_entry_29, .name = "Reserved"                   , .handler = isr29_reserved },
    { .entry = _isr_entry_30, .name = "Reserved"                   , .handler = isr30_reserved },
    { .entry = _isr_entry_31, .name = "Reserved"                   , .handler = isr31_reserved },
    { .entry = _isr_entry_32, .name = "IRQ n°0"                    , .handler = irq0  },
    { .entry = _isr_entry_33, .name = "IRQ n°1"                    , .handler = irq1  },
    { .entry = _isr_entry_34, .name = "IRQ n°2"                    , .handler = irq2  },
    { .entry = _isr_entry_35, .name = "IRQ n°3"                    , .handler = irq3  },
    { .entry = _isr_entry_36, .name = "IRQ n°4"                    , .handler = irq4  },
    { .entry = _isr_entry_37, .name = "IRQ n°5"                    , .handler = irq5  },
    { .entry = _isr_entry_38, .name = "IRQ n°6"                    , .handler = irq6  },
    { .entry = _isr_entry_39, .name = "IRQ n°7"                    , .handler = irq7  },
    { .entry = _isr_entry_40, .name = "IRQ n°8"                    , .handler = irq8  },
    { .entry = _isr_entry_41, .name = "IRQ n°9"                    , .handler = irq9  },
    { .entry = _isr_entry_42, .name = "IRQ n°10"                   , .handler = irq10 },
    { .entry = _isr_entry_43, .name = "IRQ n°11"                   , .handler = irq11 },
    { .entry = _isr_entry_44, .name = "IRQ n°12"                   , .handler = irq12 },
    { .entry = _isr_entry_45, .name = "IRQ n°13"                   , .handler = irq13 },
    { .entry = _isr_entry_46, .name = "IRQ n°14"                   , .handler = irq14 },
    { .entry = _isr_entry_47, .name = "IRQ n°15"                   , .handler = irq15 },
};

static inline IdtEntry IdtEntry_init(void (*isr_entry)(), u16 sel) {
    return (IdtEntry) {
        .always_zero = 0x0,
        .always_01110 = 0b01110,
        .ring = 0b00,
        .is_segment_present = 0b1,
        .base_low16 = ((uptr) isr_entry) & 0xffff,
        .base_high16 = (((uptr) isr_entry)>>16) & 0xffff,
        .segment_selector = sel,
    };
}

void irq0_set_timer_frequency(f32 hz) {
    irq0_timer_frequency = hz;
    u32 divisor = 1193180u / hz;  /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}


// Magic which remaps IRQ 0 to 15 to IDT entries 32 to 48.
static _cold void irq_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void _cold idt_setup() {
    for(usize i=0 ; i<countof(isrs) ; ++i) {
        idt[i] = IdtEntry_init(isrs[i].entry, 0x08);
    }

    // We could also have done this _before_ preparing the IDT.
    idt_load();
    irq_remap();

    asm volatile ("sti" asm_endl);
}

void isr_dispatch(IsrContext *ctx) {
    let i = ctx->interrupt_number;
    let isr = isrs[i];
    if(ctx->interrupt_number - 32 != 0) { // Don't log IRQ 0, because it's a timer.
        logd("Encountered interrupt n°", i, ": ", isr.name, ".");
    }
    isr.handler(ctx);
    if(i < 32) { // If it's a processor-generated exception, we halt for now.
        logd("eip        = ", ctx->eip);
        logd("user_esp   = ", ctx->user_esp);
        logd("esp        = ", ctx->esp);
        logd("ebp        = ", ctx->ebp);
        logd("error code = ", ctx->error_code);
        logd("Hanging because it's assumed to be fatal.");
        hang();
    } else {
        // If we're here, we're done servicing an IRQ.

        // If it's IRQ 8 to 15, send EOI to slave PIC
        if(ctx->interrupt_number - 32 >= 8
        && ctx->interrupt_number - 32 <= 15) {
            outb(0xA0, 0x20);
        }
        // Send EOI to master PIC in all cases
        outb(0x20, 0x20);
    }
}

void irq0_timer_wait_ticks(u32 ticks) {
    let upper_bound = irq0_timer_ticks + ticks;
    while(irq0_timer_ticks < upper_bound) {
        _mm_pause();
    }
}

void sleep_ms(u32 ms) {
    irq0_timer_wait_ticks((ms/1000.f) * irq0_timer_frequency);
}
void sleep_s(u32 s) {
    irq0_timer_wait_ticks(s * irq0_timer_frequency);
}


