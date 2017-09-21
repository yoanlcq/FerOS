#pragma once

typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt_number, error_code;
    u32 eip, cs, eflags, user_esp, ss;
} IsrContext;


