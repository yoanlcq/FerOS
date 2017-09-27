// Our minimal, restricted version of <elf.h>.

#pragma once

typedef u16 Elf32_Half;
typedef u32 Elf32_Word;
typedef i32 Elf32_Sword;
typedef u64 Elf32_Xword;
typedef i64 Elf32_Sxword;
typedef u32 Elf32_Addr;
typedef u32 Elf32_Off;
typedef u16 Elf32_Section;
typedef Elf32_Half Elf32_Versym;

// Section header.
typedef struct {
    Elf32_Word name;   // Section name (string tbl index)
    Elf32_Word type;   // Section type
    Elf32_Word flags;  // Section flags
    Elf32_Addr addr;   // Section virtual addr at execution
    Elf32_Off  offset; // Section file offset
    Elf32_Word size;   // Section size in bytes
    Elf32_Word link;   // Link to another section
    Elf32_Word info;   // Additional section information
    Elf32_Word addralign;  // Section alignment
    Elf32_Word entsize;    // Entry size if section holds table
} Elf32_Shdr;

// Symbol table entry.
typedef struct {
    Elf32_Word    name;  // Symbol name (string tbl index)
    Elf32_Addr    value; // Symbol value
    Elf32_Word    size;  // Symbol size
    u8            info;  // Symbol type and binding
    u8            other; // Symbol visibility
    Elf32_Section shndx; // Section index
} Elf32_Sym;

