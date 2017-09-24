#pragma once

typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt_number, error_code;
    u32 eip, cs, eflags, user_esp, ss;
} IsrContext;

typedef struct {
    const char *name;
    void (*entry)();
    void (*handler)(IsrContext*);
} IsrSpec;


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


