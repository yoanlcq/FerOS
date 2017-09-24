#pragma once

// TODO: Also support saving MMX and SSE state with FXSR
// Section 13.5 of Intel's system programming manual.

typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt_number, error_code;
    u32 eip, cs, eflags, user_esp, ss;
} IsrContext;

