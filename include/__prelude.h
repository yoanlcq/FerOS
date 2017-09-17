// TODO Mention this in the FAQ

// The prelude is implicitly #include-d in all source files, using
// GCC's `-include __prelude.h`.
// The intent is quite the same as Rust's prelude.
//
// What determines if a given item should be in the prelude or not is
// fairly arbitrary, but generally :
// - Build-time assertions;
// - C extensions (because they "should" be part of the language);
// - Compiler/Hardware intrinsics (they slightly count as C extensions);
// - Anything that is used so often it should be "standard".
//
// NOTE to self: If the cross-compiler was installed with "/usr/local" as the
// prefix, then the built-in include files are located in (in my case) :
//     /usr/local/lib/gcc/i686-elf/6.3.0/include/

#pragma once

#if defined(__linux__)
#error "Please use a genuine cross-compiler. It's really needed."
#endif
 
#if !defined(__i386__)
#error "The kernel can only be compiled with a x86-elf compiler."
#else
#define targets_ia32 1
#endif


#if defined(__x86_64__)
#define targets_x64 1
#endif

#if defined(__i386__) || defined(__x86_64__)
#define targets_x86 1
#endif


// Also in general we pretty much assume C11 throughout the code.
#if __STDC_VERSION__ < 201112L
#error _Generic is only avaliable with the C11 standard. Try with -std=c11.
#endif


#include <stdalign.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
// #include <tgmath.h>

#define static_assert _Static_assert


//
// GCC-specific - include these early on
//

/*
 * Quick reminder of inline ASM format:
 *
 * asm [volatile] (
 *     AssemblerTemplateasm_endl
 *     : OutputOperands [ : InputOperands [ : Clobbers ] ]
 * )
 */
#define asm __asm__
#define asm_endl "\n\t"

#define c_attr(...) __attribute__((__VA_ARGS__))
#define inline_always inline c_attr(always_inline)
#define typeof __typeof__
#define auto __auto_type
#define let const auto


// Define our more strongly-typed bool type. Without the explicit casts to
// bool, generics treat `stdbool.h`'s `true` and `false` as integers.
#define bool _Bool
#define false ((bool)0)
#define true ((bool)1)

typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef uint16_t char16;
typedef uint32_t char32;
typedef wchar_t wchar;
typedef uintptr_t uptr, usize;
typedef intptr_t iptr, isize; // stddef.h defines size_t, but not ssize_t.
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef uint32_t bool32, b32;
#ifdef __SIZEOF_INT128__
typedef          __int128 i128;
typedef unsigned __int128 u128;
#endif

// Assert on the size of imprecise types (they're used in generics so we
// need to know which casts are lossless)
//
// Important: If one of these gets violated, it's probably because we attempted
// to build for a target that wasn't supported before (e.g x86_64).
// We'll then want to use conditional compilation, and not forget to
// change `generic_dispatch` accordingly
// (especially `long`, `isize`, and friends).
static_assert(sizeof(  signed char     ) == 1, "Unexpected size!");
static_assert(sizeof(unsigned char     ) == 1, "Unexpected size!");
static_assert(sizeof(  signed short    ) == 2, "Unexpected size!");
static_assert(sizeof(unsigned short    ) == 2, "Unexpected size!");
static_assert(sizeof(  signed int      ) == 4, "Unexpected size!");
static_assert(sizeof(unsigned int      ) == 4, "Unexpected size!");
static_assert(sizeof(  signed long     ) == 4, "Unexpected size!");
static_assert(sizeof(unsigned long     ) == 4, "Unexpected size!");
static_assert(sizeof(  signed long long) == 8, "Unexpected size!");
static_assert(sizeof(unsigned long long) == 8, "Unexpected size!");
static_assert(sizeof(void*             ) == 4, "Unexpected size!");
static_assert(sizeof(void*             ) == 4, "Unexpected size!");


// NOTE: This is the naïve implementation, which doesn't perform
// trickery to assert that the argument is a static array.
// It's enough for me right now.
#define countof(x) (sizeof((x)) / sizeof((x[0])))

// C11's _Generic doesn't allow us to be more precise than that, with
// fundamental types.
// For instance, we can't put `wchar_t` or `isize` in here.
#define generic_dispatch(each,x) (_Generic((x), \
    bool                : each(bool), \
    char                : each(char), \
      signed char       : each(i8), \
    unsigned char       : each(u8), \
      signed short      : each(i16), \
    unsigned short      : each(u16), \
      signed int        : each(i32), \
    unsigned int        : each(u32), \
      signed long       : each(i32), \
    unsigned long       : each(u32), \
      signed long long  : each(i64), \
    unsigned long long  : each(u64), \
    float               : each(f32), \
    double              : each(f64), \
    char*               : each(str), \
    const char*         : each(str), \
    void*               : each(ptr), \
    const void*         : each(ptr) \
)(x))


//
// Hardware intrinsics
//

// _MM_MALLOC_H_INCLUDED: Hack, because we don't have malloc() yet, 
// and we don't have <errno.h> either.
#define _MM_MALLOC_H_INCLUDED 
#include <x86intrin.h>
#include <cpuid.h>
#undef _MM_MALLOC_H_INCLUDED 

//
// Inhibit types.
//
// We do this at the very end, after any other "standard" header.
// This is only to enforce consistency through the project.
//
// char is still allowed because "char*" is, and always will be, a
// null-terminated string (ASCII or UTF-8).
#define char16_t  dont_use__superseded_by_char16
#define char32_t  dont_use__superseded_by_char32
#define ssize_t   dont_use__superseded_by_isize
#define size_t    dont_use__superseded_by_usize
#define intptr_t  dont_use__superseded_by_iptr
#define uintptr_t dont_use__superseded_by_uptr
#define uint8_t   dont_use__superseded_by_u8
#define uint16_t  dont_use__superseded_by_u16
#define uint32_t  dont_use__superseded_by_u32
#define uint64_t  dont_use__superseded_by_u64
#define int8_t    dont_use__superseded_by_i8
#define int16_t   dont_use__superseded_by_i16
#define int32_t   dont_use__superseded_by_i32
#define int64_t   dont_use__superseded_by_i64
#define __int128  dont_use__superseded_by_i128
// Commented to allow literals as generics
// #define int       dont_use__imprecise_type
// #define long      dont_use__imprecise_type
// #define short     dont_use__imprecise_type
// #define float     dont_use__superseded_by_f32
// #define double    dont_use__superseded_by_f64
// #define wchar_t   dont_use__superseded_by_wchar


//
// Our own intrinsics
//


static inline_always void hlt() {
    asm volatile ("hlt" asm_endl);
}


// memset-like intrinsics
//
// From Intel's instruction set manual:
//     "Note that a REP STOS instruction is the fastest way to initialize a
//     large block of memory."
// Whether it's true or not, it's interesting to implement.

static inline void stosb(u8 *dst, u8 value, u32 count) {
    asm volatile (
        "rep stosb" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with AL".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
static inline void stosw(u16 *dst, u16 value, u32 count) {
    asm volatile (
        "rep stosw" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with AX".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
static inline void stosd(u32 *dst, u32 value, u32 count) {
    asm volatile (
        "rep stosd" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with EAX".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
#if 0 // defined(targets_x64)
static inline void _stosq(u64 *dst, u64 value, u64 count) {
    asm volatile (
        "rep stosq" asm_endl // "Fill RCX bytes at [RDI] with RAX".
        : : "rdi"(dst), "rcx"(count), "rax"(value) : "memory"
    );
}
#endif

//
// I/O-Ports intrinsics
//
// Some random notes:
//
// The `in*` and `out*` are always non-blocking. In serial communication,
// they should be used to query the appropriate port register which tells
// if there's data available (or if data can be sent) first.
//
// In short, don't rely on them for sending/receiving data if you did not wait
// first.
//
// Also, it looks like, when there's no data available to receive, `in*` writes
// zeroed bytes to the supplied buffer. Not actually sure about the "zeroed"
// part, it's possible that these are some kind of unprintable ASCII chars.
// This calls for a hexdump.

// For `outs*`:
// XXX Not setting the value of DS. Might be bad.
// NOTE: Not emitting a `cld` instruction, because we assume GCC keeps it cleared.
// NOTE: The actual source address is DS:ESI in 32-bit mode for `outs`.
// NOTE: The actual source address is DS:EDI in 32-bit mode for `ins`.
// NOTE: Setting DF to zero (`cld`) ensure the counter moves forward.
// NOTE: `count` really is the number of _items_, NOT the size in bytes.

static inline_always void outsb(u16 port, const u8* data, usize count) {
    asm volatile (
        "rep outsb" asm_endl
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void outsw(u16 port, const u16* data, usize count) {
    asm volatile (
        "rep outsw" asm_endl
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void outsd(u16 port, const u32* data, usize count) {
    asm volatile (
        "rep outsd" asm_endl
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void insb(u16 port, u8* data, usize count) {
    asm volatile (
        "rep insb" asm_endl
        : : "d"(port), "D"(data), "c"(count)
    );
}
static inline_always void insw(u16 port, u16* data, usize count) {
    asm volatile (
        "rep insw" asm_endl
        : : "d"(port), "D"(data), "c"(count)
    );
}
static inline_always void insd(u16 port, u32* data, usize count) {
    asm volatile (
        "rep insd" asm_endl
        : : "d"(port), "D"(data), "c"(count)
    );
}


static inline_always void outb(u16 port, u8 data) {
    asm volatile (
        "out dx, al" asm_endl
        : : "d"(port), "a"(data)
    );
}

static inline_always void outw(u16 port, u16 data) {
    asm volatile (
        "out dx, ax" asm_endl
        : : "d"(port), "a"(data)
    );
}

static inline_always void outd(u16 port, u32 data) {
    asm volatile (
        "out dx, eax" asm_endl
        : : "d"(port), "a"(data)
    );
}

static inline_always u8 inb(u16 port) {
    u8 data;
    asm volatile (
        "in al, dx" asm_endl
        : "=a"(data) : "d"(port)
    );
    return data;
}

static inline_always u16 inw(u16 port) {
    u16 data;
    asm volatile (
        "in ax, dx" asm_endl
        : "=a"(data) : "d"(port)
    );
    return data;
}

static inline_always u32 ind(u16 port) {
    u32 data;
    asm volatile (
        "in eax, dx" asm_endl
        : "=a"(data) : "d"(port)
    );
    return data;
}




//
// Functions intended to be portable and universally useful
//

static inline void memset     (void *mem, u8  value, usize size ) { stosb((void*)mem, value, size ); }
static inline void memset_u8  (u8   *mem, u8  value, usize count) { stosb((void*)mem, value, count); }
static inline void memset_i8  (i8   *mem, i8  value, usize count) { stosb((void*)mem, value, count); }
static inline void memset_u16 (u16  *mem, u16 value, usize count) { stosw((void*)mem, value, count); }
static inline void memset_i16 (i16  *mem, i16 value, usize count) { stosw((void*)mem, value, count); }
static inline void memset_u32 (u32  *mem, u32 value, usize count) { stosd((void*)mem, value, count); }
static inline void memset_i32 (i32  *mem, i32 value, usize count) { stosd((void*)mem, value, count); }
static inline void zero     (void *mem, usize size ) { memset    (mem, 0, size ); }
static inline void zero_u8  (u8   *mem, usize count) { memset_u8 (mem, 0, count); }
static inline void zero_u16 (u16  *mem, usize count) { memset_u16(mem, 0, count); }
static inline void zero_u32 (u32  *mem, usize count) { memset_u32(mem, 0, count); }
static inline void zero_i8  (i8   *mem, usize count) { memset_i8 (mem, 0, count); }
static inline void zero_i16 (i16  *mem, usize count) { memset_i16(mem, 0, count); }
static inline void zero_i32 (i32  *mem, usize count) { memset_i32(mem, 0, count); }

static inline noreturn void hang() { for(;;) hlt(); }
static inline noreturn void abort() { hang(); }
static inline noreturn void exit(i32 code) {
    (void)code; // What to do ? Move to EAX ? Or call logd() ?
    hang();
}

// The MAP() macro for applying a macro to variable arguments.
// It should somewhat be part of standard C, IMO.
// Well except for the screen flooding part for the slightest mistake.
#include <__map_macro.h>

// Include last, needs abort()
#include <__assert.h>

