// IO Ports intrinsics.

#pragma once

#include <defs.h>
#include <stdint.h>
#include <c_ext.h>

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
// XXX Not emitting a `cld` instruction, because we assume GCC keeps it cleared.
// NOTE: The actual source address is DS:ESI in 32-bit mode for `outs`.
// NOTE: The actual source address is DS:EDI in 32-bit mode for `ins`.
// NOTE: Setting DF to zero (`cld`) ensure the counter moves forward.
// NOTE: `count` really is the number of _items_, NOT the size in bytes.

static inline_always void outsb(uint16_t port, const uint8_t* data, size_t count) {
    asm volatile (
        "rep outsb"   LFTAB
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void outsw(uint16_t port, const uint16_t* data, size_t count) {
    asm volatile (
        "rep outsw"   LFTAB
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void outsd(uint16_t port, const uint32_t* data, size_t count) {
    asm volatile (
        "rep outsd"   LFTAB
        : : "d"(port), "S"(data), "c"(count)
    );
}
static inline_always void insb(uint16_t port, uint8_t* data, size_t count) {
    asm volatile (
        "rep insb"   LFTAB
        : : "d"(port), "D"(data), "c"(count)
    );
}
static inline_always void insw(uint16_t port, uint16_t* data, size_t count) {
    asm volatile (
        "rep insw"   LFTAB
        : : "d"(port), "D"(data), "c"(count)
    );
}
static inline_always void insd(uint16_t port, uint32_t* data, size_t count) {
    asm volatile (
        "rep insd"   LFTAB
        : : "d"(port), "D"(data), "c"(count)
    );
}


static inline_always void outb(uint16_t port, uint8_t data) {
    asm volatile (
        "out dx, al"   LFTAB
        : : "d"(port), "a"(data)
    );
}

static inline_always void outw(uint16_t port, uint16_t data) {
    asm volatile (
        "out dx, ax"   LFTAB
        : : "d"(port), "a"(data)
    );
}

static inline_always void outd(uint16_t port, uint32_t data) {
    asm volatile (
        "out dx, eax"   LFTAB
        : : "d"(port), "a"(data)
    );
}

static inline_always uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile (
        "in al, dx"   LFTAB
        : "=a"(data) : "d"(port)
    );
    return data;
}

static inline_always uint16_t inw(uint16_t port) {
    uint16_t data;
    asm volatile (
        "in ax, dx"   LFTAB
        : "=a"(data) : "d"(port)
    );
    return data;
}

static inline_always uint32_t ind(uint16_t port) {
    uint32_t data;
    asm volatile (
        "in eax, dx"   LFTAB
        : "=a"(data) : "d"(port)
    );
    return data;
}
