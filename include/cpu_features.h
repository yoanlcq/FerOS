#pragma once

// Features begin with `has_` because there might be
// extra non-boolean info about some features, later on.
// For instance:
//     bool has_somefeature :1;
//     SomeFeature somefeature; // Extra info about SOMEFEATURE
//
// NOTE: Doesn't bother to match the exact layout of CPUID bits.
typedef struct {
    bool has_sse3                : 1;
    bool has_ssse3               : 1;
    bool has_sse4_1              : 1;
    bool has_sse4_2              : 1;
    bool has_rdrand              : 1;
    bool has_thermal_monitor     : 1;
    bool has_sse2                : 1;
    bool has_sse                 : 1;
    bool has_mmx                 : 1;
    bool has_fxsr                : 1;
    bool has_acpi                : 1;
    bool has_clflush             : 1;
    bool has_cmov                : 1;
    bool has_apic                : 1;
    bool has_phys_address_ext    : 1;
    bool has_tsc                 : 1;
    bool has_page_size_extension : 1;
    bool has_x87_fpu             : 1;
    usize clflush_line_size; // In bytes
} CpuFeatures;

// Filled by __init()
extern const CpuFeatures *const cpu_features;
