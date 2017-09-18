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

#if defined(__linux__) || defined(_WIN32) || defined(__APPLE__) \
 || defined(__unix__) || defined(_POSIX_VERSION)
#error "Please use a genuine cross-compiler. It's really needed."
#endif

#if defined(__i386__) || defined(__x86_64__)
#define targets_x86 1
#endif

#if defined(__i386__)
#define targets_ia32 1
#endif

#if defined(__x86_64__)
#define targets_x64 1
#endif

#if !(defined(targets_ia32) && defined(__ELF__))
#error "The kernel can only be compiled with a x86-elf compiler."
#endif

// Also in general we pretty much assume C11 throughout the code.
#if __STDC_VERSION__ < 201112L
#error "_Generic is only available with the C11 standard. Try with -std=c11."
#endif

// And we pretty much assume GNU C because of its cool extensions
#ifndef __GNUC__
#error "I don't think the code is ready to support other compilers than GCC."
#endif


// Note that we don't include <stdbool.h>. It's on purpose, see further down.
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
 *     AssemblerTemplate asm_endl
 *     : OutputOperands [ : InputOperands [ : Clobbers ] ]
 * )
 *
 * `volatile` informs GCC that there are side effects not implied by
 * the output, input and clobbers. Generally `volatile` is fine and the
 * safest bet, but it might prevent some optimizations.
 */
#define asm __asm__
#define asm_endl "\n\t"

#define c_attr(...) __attribute__((__VA_ARGS__))
// Super cool when warnings are errors!
#define TODO c_attr(warning("This is not implemented yet!"))
#define inline_always inline c_attr(always_inline)
// Also don't be tempted to define `always_inline` as well - won't work.
#define typeof __typeof__
#define auto __auto_type // So modern omg
#define let const auto   // Such Rust, much safe, wow


// Define our more strongly-typed bool type. Without the explicit casts to
// bool, generics treat `stdbool.h`'s `true` and `false` as integers.
#define bool _Bool
#define false ((bool)0)
#define true ((bool)1)
#define _STDBOOL_H // Silently prevent inclusions of <stdbool.h>
#define __bool_true_false_are_defined 1

// These guys are annoying to type
typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef long double ldouble;

typedef uint32_t bool32, b32;
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
#ifdef __SIZEOF_INT128__ // Not defined on unextended IA-32
typedef          __int128 i128;
typedef unsigned __int128 u128;
#endif
typedef float f32;
typedef double f64;
#ifdef targets_x86
typedef long double f80;
#else
#error "We don't know the actual size of long double!"
#endif
#ifdef __SIZEOF_F128__ // Not defined on unextended IA-32
typedef __float128 f128;
#endif
// NOTE: `f80` is named so because it has 80 bits of entropy,
// BUT this doesn't mean it's the actual size it will take in structs or
// memory in general.
// `f80` is actually padded to take either 96 bits or 128 bits, depending on the
// options given to GCC. See the documentation on `-m96bit-long-double`
// and `-m128bit-long-double`.
// 96 bits is the default because it's required by the x86 ABI, but apparently
// 128 bits is somewhat "preferred" by modern CPUs.
// See the `static_assert`s further down to see which size it is.


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
static_assert(sizeof(long double       ) ==12, "Unexpected size!");
static_assert(sizeof(f80               ) ==12, "Unexpected size!");
static_assert(sizeof(void*             ) == 4, "Unexpected size!");
static_assert(sizeof(void*             ) == 4, "Unexpected size!");

static_assert(CHAR_BIT == 8, "We expect char to be of 8 bits!");
static_assert(CHAR_MIN == SCHAR_MIN, "We expect char to be signed!");
static_assert(CHAR_MAX == SCHAR_MAX, "We expect char to be signed!");

// countof() macro
// NOTE: This is the naïve implementation, which doesn't perform
// trickery to assert that the argument is a static array.
// It's enough for me right now.
#define countof(x) (sizeof((x)) / sizeof((x[0])))

// C11's _Generic doesn't allow us to be more precise than that, with
// fundamental types.
// For instance, we can't put `wchar_t` or `isize` in here.
//
// We refer to the actual standard base types instead of our typedefs because
// literals are of these types.
// For instance, if I remember correctly, `42` is a `signed int`, and even
// though it's therefore also an `i32`, it doesn't match the `i32` arm in
// a `_Generic` (it only matches `signed int`).
// I'm pretty certain I don't have the bigger picture in mind, but I don't
// care that much, I just want to get stuff working and move on.
//
// We'll have to conditionally change some of this stuff when we move to x64.
#define def_generic_over_primitives(each,x) (_Generic((x), \
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
    const char*         : each(cstr), \
    void*               : each(ptr), \
    const void*         : each(cptr) \
))
#define def_generic_over_primitives_value(each,x) \
    (def_generic_over_primitives(each,x)(x))
#define def_generic_over_primitives_addr(each,x) \
    (def_generic_over_primitives(each,x)(&(x)))


// The MAP() macro for applying a macro to variable arguments.
// It somewhat ought to be part of standard C, IMO (or implemented in compilers).
// Well except for the screen flooding part when you make the slightest mistake.
#include <__map_macro.h>


#include <log.h>
// Define the super-fresh assert_cmp() macro.
//
// It's able to assert that a given comparison holds true for two operands,
// pretty-printing both of them if it doesn't.
//
// "Behold the power of C11."
#ifdef NDEBUG
#define assert_cmp(lhs, cmp, rhs, msg)
#else
#define assert_cmp(lhs, cmp, rhs, msg) do { \
    /* Expand the expressions only once */ \
    let _lhs = lhs; \
    let _rhs = rhs; \
    if(!(_lhs cmp _rhs)) { \
        logd_( \
            "\n=== ASSERTION FAILED ===\n" \
            "(", msg, ")\n" \
            "In ", __FILE__, ":", __LINE__, ", in function ", __func__, ":\n" \
            "Expected...\n" \
            "    `", #lhs, " ", #cmp, " ", #rhs, "`\n" \
            "... But :\n" \
            "lhs = ", _lhs, "; (", #lhs, ")\n" \
            "rhs = ", _rhs, "; (", #rhs, ")\n", \
            "\nAborting.\n" \
            "========================\n" \
        ); \
        abort(); \
    } \
} while(0)
#endif


// They both need the TO-DO macro and are included by <mm_malloc.h>
// itself included in <x86intrin.h>.
// They both can use the standard types like `int` and `size_t`.
#include <stdlib.h>
#include <errno.h>

//
// Hardware intrinsics
//

#include <x86intrin.h>
#include <cpuid.h>

//
// Inhibit types.
//
// We do this at the very end, after any other "standard" header.
// This is only to enforce consistency throughout the codebase.
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
#define __float128  dont_use__superseded_by_f128
// Commented out to allow literals as generics
// #define int       dont_use__imprecise_type
// #define long      dont_use__imprecise_type
// #define short     dont_use__imprecise_type
// #define float     dont_use__superseded_by_f32
// #define double    dont_use__superseded_by_f64
// #define wchar_t   dont_use__superseded_by_wchar



//
// Our own intrinsics
//


// memset-like intrinsics
//
// From Intel's instruction set manual:
//     "Note that a REP STOS instruction is the fastest way to initialize a
//     large block of memory."
// True or not, it's interesting to implement.

static inline_always void stosb(u8 *dst, u8 value, u32 count) {
    asm volatile (
        "rep stosb" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with AL".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
static inline_always void stosw(u16 *dst, u16 value, u32 count) {
    asm volatile (
        "rep stosw" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with AX".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
static inline_always void stosd(u32 *dst, u32 value, u32 count) {
    asm volatile (
        "rep stosd" asm_endl // "Fill (E)CX bytes at ES:[(E)DI] with EAX".
        : : "D"(dst), "c"(count), "a"(value) : "memory"
    );
}
#if 0 // defined(targets_x64)
static inline_always void stosq(u64 *dst, u64 value, u64 count) {
    asm volatile (
        "rep stosq" asm_endl // "Fill RCX bytes at [RDI] with RAX".
        : : "rdi"(dst), "rcx"(count), "rax"(value) : "memory"
    );
}
#endif

//
// memcpy()-like intrinsics
//

static inline_always void movsb(u8 *dst, const u8* src, u32 count) {
    asm volatile (
        "rep movsb" asm_endl // Move ECX bytes from DS:[ESI] to ES:[EDI].
        : : "D"(dst), "S"(src), "c"(count) : "memory"
    );
}
static inline_always void movsw(u16 *dst, const u16* src, u32 count) {
    asm volatile (
        "rep movsw" asm_endl // Move ECX words from DS:[ESI] to ES:[EDI].
        : : "D"(dst), "S"(src), "c"(count) : "memory"
    );
}
static inline_always void movsd(u32 *dst, const u32* src, u32 count) {
    asm volatile (
        "rep movsd" asm_endl // Move ECX doublewords from DS:[ESI] to ES:[EDI].
        : : "D"(dst), "S"(src), "c"(count) : "memory"
    );
}


//
// I/O Ports intrinsics
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
// - Not emitting a `cld` instruction, because we assume GCC keeps it cleared
//   (it appears to be the case, as mentioned in a StackOverflow answer).
//   Keeping DF to zero (`cld`) ensures the counter moves forward.
// - The actual source address is DS:ESI in 32-bit mode for `outs`.
// - The actual source address is DS:EDI in 32-bit mode for `ins`.
// - `count` really is the number of _items_, NOT the size in bytes.

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
        : : "Nd"(port), "a"(data)
    );
}

static inline_always void outw(u16 port, u16 data) {
    asm volatile (
        "out dx, ax" asm_endl
        : : "Nd"(port), "a"(data)
    );
}

// osdev.org says that the traditional name would be `outl`.
static inline_always void outd(u16 port, u32 data) {
    asm volatile (
        "out dx, eax" asm_endl
        : : "Nd"(port), "a"(data)
    );
}

static inline_always u8 inb(u16 port) {
    u8 data;
    asm volatile (
        "in al, dx" asm_endl
        : "=a"(data) : "Nd"(port)
    );
    return data;
}

static inline_always u16 inw(u16 port) {
    u16 data;
    asm volatile (
        "in ax, dx" asm_endl
        : "=a"(data) : "Nd"(port)
    );
    return data;
}

// osdev.org says that the traditional name would be `inl`.
static inline_always u32 ind(u16 port) {
    u32 data;
    asm volatile (
        "in eax, dx" asm_endl
        : "=a"(data) : "Nd"(port)
    );
    return data;
}


// Get/Set Control Register intrinsics
// e.g `get_cr0()` and `set_cr0()`
#define def_getset_cr(x) \
static inline_always u32 get_cr##x() { \
    u32 val; \
    asm volatile ( \
        "mov %0, cr" #x asm_endl \
        : "=r"(val) \
    ); \
    return val; \
} \
static inline_always void set_cr##x(u32 val) { \
    asm volatile ( \
        "mov cr" #x ", %0" asm_endl \
        : : "r"(val) \
    ); \
}
def_getset_cr(0)
def_getset_cr(1)
def_getset_cr(2)
def_getset_cr(3)
def_getset_cr(4)
def_getset_cr(5)
def_getset_cr(6)
def_getset_cr(7)
#undef def_getset_cr

static inline_always void hlt() {
    asm volatile ("hlt" asm_endl);
}


//
// Functions intended to be portable and universally useful
//

// Halt the CPU. Could just be implemented with `for(;;);`, but is
// intended to save power.
static inline noreturn void hang() { for(;;) hlt(); }

// WISH: Now we have SSE, we might as well write "memcpy_sse" functions
static inline void memset     (void *mem, u8  value, usize size ) { stosb(      mem, value, size ); }
static inline void memset_u8  (u8   *mem, u8  value, usize count) { stosb((u8 *)mem, value, count); }
static inline void memset_i8  (i8   *mem, i8  value, usize count) { stosb((u8 *)mem, value, count); }
static inline void memset_u16 (u16  *mem, u16 value, usize count) { stosw((u16*)mem, value, count); }
static inline void memset_i16 (i16  *mem, i16 value, usize count) { stosw((u16*)mem, value, count); }
static inline void memset_u32 (u32  *mem, u32 value, usize count) { stosd((u32*)mem, value, count); }
static inline void memset_i32 (i32  *mem, i32 value, usize count) { stosd((u32*)mem, value, count); }
static inline void zero       (void *mem, usize size ) { memset    (mem, 0, size ); }
static inline void zero_u8    (u8   *mem, usize count) { memset_u8 (mem, 0, count); }
static inline void zero_u16   (u16  *mem, usize count) { memset_u16(mem, 0, count); }
static inline void zero_u32   (u32  *mem, usize count) { memset_u32(mem, 0, count); }
static inline void zero_i8    (i8   *mem, usize count) { memset_i8 (mem, 0, count); }
static inline void zero_i16   (i16  *mem, usize count) { memset_i16(mem, 0, count); }
static inline void zero_i32   (i32  *mem, usize count) { memset_i32(mem, 0, count); }
static inline void memcpy     (void *dst, const u8  *src, usize size ) { movsb(      dst,       src, size ); }
static inline void memcpy_u8  (u8   *dst, const u8  *src, usize count) { movsb((u8 *)dst, (u8 *)src, count); }
static inline void memcpy_i8  (i8   *dst, const i8  *src, usize count) { movsb((u8 *)dst, (u8 *)src, count); }
static inline void memcpy_u16 (u16  *dst, const u16 *src, usize count) { movsw((u16*)dst, (u16*)src, count); }
static inline void memcpy_i16 (i16  *dst, const i16 *src, usize count) { movsw((u16*)dst, (u16*)src, count); }
static inline void memcpy_u32 (u32  *dst, const u32 *src, usize count) { movsd((u32*)dst, (u32*)src, count); }
static inline void memcpy_i32 (i32  *dst, const i32 *src, usize count) { movsd((u32*)dst, (u32*)src, count); }

