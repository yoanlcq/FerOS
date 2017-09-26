// Filled in by linker.
// These are empty structs because the linker scripts _places_ these symbols
// in memory (instead of declaring them as pointers and setting their value).
// If we were to declare them as `void*`, their _value_ would be that of
// whatever raw data follows (e.g machine code or whatever).
// 
// That means the only sensible thing to do is to grab the address of these
// symbols (as in `&ks_stack_top`). It should be forbidden to write to the
// data, hence empty structs.

#pragma once

extern const struct {} ks_stack_top;
extern const struct {} ks_stack_bottom;
extern const struct {} ks_phys_addr;
extern const struct {} ks_text_start;
extern const struct {} ks_text_end;
extern const struct {} ks_rodata_start;
extern const struct {} ks_rodata_end;
extern const struct {} ks_assets_start;
extern const struct {} ks_assets_end;
extern const struct {} ks_data_start;
extern const struct {} ks_data_end;
extern const struct {} ks_bss_start;
extern const struct {} ks_bss_end;
extern const struct {} ks_end;
extern const struct {} mbh_load_addr;
extern const struct {} mbh_bss_end_addr;
