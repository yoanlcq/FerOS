// The __init() function, which is called before main but after
// a stack has been set up.
//
// Rationale:
//
// A lot of hardware extensions intrinsics require turning on specific
// compiler options, so that it can assume it is safe to generate code
// which uses these extensions.
//
// Typically on IA-32, we'll take SSE and SSE2 for granted (even though we
// theoretically shouldn't. Only x64 does guarantee SSE and SSE2), so we'll
// compile with the `-msse` and `-msse2` flags.
//
// But now GCC's optimizers will happily turn certain instructions into
// SSE/SSE2 instructions, in _all_ source files (because we told it to), 
// and if the CPU doesn't _actually_ have SSE/SSE2 our OS will crash without
// notice.
//
// If only we had *some piece of code* which :
// - Wouldn't be subject to potentially harmful "optimizations";
// - Would be known not to perform any floating-point operation;
// - Would be called before any "actual code";
// - Would be responsible for setting up SSE/SSE2 early, and if it can't be
//   done, tell the user and hang;
//
// Sounds like a job for an `__init()` function!
// It's called before `main()`, in `src/boot.S`, and must be written with
// care.
//
// If `__init()` fails, it should not return (e.g by calling `hang()`).
// If it ever needs to pass state to `main()`, it should do so via
// global variables, such as `cpu_features`.

#include <multiboot.h>
#include <string.h>
#include <vga.h>
#include <log.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <mouse.h>
#include <cpu_features.h>


static CpuFeatures _cpu_features = {0};
const CpuFeatures *const cpu_features = &_cpu_features;

void _cold __deinit() {
    log_shutdown();
}

static _cold _no_sse void enable_sse() {
    // Chapter 13 of Intel's system programming guide.
    auto cr4 = get_cr4();
    auto cr0 = get_cr0();

    cr4 |= 1<<9;    // Set CR4.OSFXSR[bit 9] = 1;
    cr4 |= 1<<10;   // Set CR4.OSXMMEXCPT[bit 10] = 1;
    cr0 &= ~(1<<2); // Clear CR0.EM[bit 2] = 0;
    cr0 |= 1<<1;    // Set CR0.MP[bit 1] = 1;
    cr0 &= ~(1<<3); // Clear CR0.TS[bit 3] = 0;

    // The OSXMMEXCPT bit indicates we've provided an SIMD exception handler,
    // which is true now.
    //
    // Clearing the TS bit is not mandatory, but maybe important for us:
    // This supposedly prevents SSE and x87 instructions from throwing #NM.
    // Intel says that the goal is to
    // provide an opportunity for the kernel to save SSE and x87 state
    // when applications call such instructions, but I don't care and it
    // scares me more than anything else.

    set_cr4(cr4);
    set_cr0(cr0);
}

// Called at the end of `__init` and when SSE is enabled. Prior to that
// we must NOT do anything related to floating-point.
// We need it as a separate function because `__init` is marked `_no_sse`.
static _cold void __init_post_sse() {
    irq0_set_timer_frequency(100.f);
    mouse_setup();
}

void _cold _no_sse __init(u32 multiboot_magic) {
    log_setup();

    logd("--- FerOS Build ", __TIMESTAMP__, " ---");

    // FIXME: We've gotta switch to text mode so the user can see what
    // we have to say

    if(multiboot_magic != MB_BOOTLOADER_MAGIC) {
        auto cursor = VgaCursor_new(0, 0);
        auto msg = "Invalid multiboot bootloader magic value!";
        vga_puts_logd(cursor, msg, VgaLightRed, VgaBlack);
        cursor.y += 1;
        msg = "Because of this, the system won't attempt to boot.";
        vga_puts_logd(cursor, msg, VgaLightGrey, VgaBlack);
        cursor.y += 1;
        vga_set_cursor(cursor);
        hang();
    }

    // u32 is "long unsigned", but "unsigned" is expected here. Sigh.
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0;
    __get_cpuid(0x01, &eax, &ebx, &ecx, &edx);
    _cpu_features.has_sse3                = !!(ecx & (bit_SSE3));
    _cpu_features.has_ssse3               = !!(ecx & (bit_SSSE3));
    _cpu_features.has_sse4_1              = !!(ecx & (bit_SSE4_1));
    _cpu_features.has_sse4_2              = !!(ecx & (bit_SSE4_2));
    _cpu_features.has_rdrand              = !!(ecx & (1<<30));
    _cpu_features.has_thermal_monitor     = !!(edx & (1<<29));
    _cpu_features.has_sse2                = !!(edx & bit_SSE2);
    _cpu_features.has_sse                 = !!(edx & bit_SSE );
    _cpu_features.has_mmx                 = !!(edx & bit_MMX );
    _cpu_features.has_fxsr                = !!(edx & (1<<24));
    _cpu_features.has_acpi                = !!(edx & (1<<22));
    _cpu_features.has_clflush             = !!(edx & (1<<19));
    _cpu_features.has_cmov                = !!(edx & (1<<15));
    _cpu_features.has_apic                = !!(edx & (1<<9 ));
    _cpu_features.has_phys_address_ext    = !!(edx & (1<<6 ));
    _cpu_features.has_tsc                 = !!(edx & (1<<4 ));
    _cpu_features.has_page_size_extension = !!(edx & (1<<3 ));
    _cpu_features.has_x87_fpu             = !!(edx & (1<<0 ));

    if(_cpu_features.has_clflush) {
        _cpu_features.clflush_line_size = 8 * ((ebx>>8) & 0xff);
    }

    const char *missing = NULL;
    /**/ if(!_cpu_features.has_x87_fpu) { missing = "the x87 FPU"; }
    else if(!_cpu_features.has_tsc    ) { missing = "Timestamp Counter (TSC)"; }
    else if(!_cpu_features.has_fxsr   ) { missing = "FXSR"; }
    else if(!_cpu_features.has_mmx    ) { missing = "MMX"; }
    else if(!_cpu_features.has_sse    ) { missing = "SSE"; }
    else if(!_cpu_features.has_sse2   ) { missing = "SSE2"; }

    if(missing) {
        VgaCursor cursor = {0};
        auto msg = "Couldn't start the system, because your processor misses";
        vga_puts_logd(cursor, msg, VgaLightRed, VgaBlack);
        cursor.y += 1;
        cursor.x  = 4;
        vga_puts_logd(cursor, missing, VgaLightBlue, VgaBlack);
        cursor.x  = 0;
        cursor.y += 1;
        msg = "which is a required feature!";
        vga_puts_logd(cursor, msg, VgaLightRed, VgaBlack);
        cursor.y += 1;
        msg = "The required features are: x87 FPU, TSC, FXSR, MMX, SSE and SSE2.";
        vga_puts_logd(cursor, msg, VgaLightRed, VgaBlack);
        cursor.y += 2;
        msg = "Welp, looks like your processor is way too old! Sorry.";
        vga_puts_logd(cursor, msg, VgaLightGrey, VgaBlack);
        cursor.y += 1;
        vga_set_cursor(cursor);

        hang();
    }

    enable_sse();

    gdt_setup();
    idt_setup();

    __init_post_sse();
}
