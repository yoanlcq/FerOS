#pragma once

#define MB_SEARCH                8192u
#define MB_HEADER_ALIGN          4u
#define MB_HEADER_MAGIC          0x1BADB002u
#define MB_BOOTLOADER_MAGIC      0x2BADB002u
#define MB_MOD_ALIGN             0x00001000u
#define MB_INFO_ALIGN            0x00000004u
#define MB_PAGE_ALIGN            0x00000001u
#define MB_MEMORY_INFO           0x00000002u
#define MB_VIDEO_MODE            0x00000004u
#define MB_AOUT_KLUDGE           0x00010000u

#define MB_INFO_MEMORY           0x00000001u
#define MB_INFO_BOOTDEV          0x00000002u
#define MB_INFO_CMDLINE          0x00000004u
#define MB_INFO_MODS             0x00000008u
#define MB_INFO_AOUT_SYMS        0x00000010u
#define MB_INFO_ELF_SHDR         0X00000020u
#define MB_INFO_MEM_MAP          0x00000040u
#define MB_INFO_DRIVE_INFO       0x00000080u
#define MB_INFO_CONFIG_TABLE     0x00000100u
#define MB_INFO_BOOT_LOADER_NAME 0x00000200u
#define MB_INFO_APM_TABLE        0x00000400u
#define MB_INFO_VBE_INFO         0x00000800u
#define MB_INFO_FRAMEBUFFER_INFO 0x00001000u

#ifndef ASM_FILE

#include <stdint.h>

typedef struct {
    u32 magic;
    u32 flags;
    u32 checksum;
    // These are only valid if MB_AOUT_KLUDGE is set.
    u32 header_addr;
    u32 load_addr;
    u32 load_end_addr;
    u32 bss_end_addr;
    u32 entry_addr;
    // These are only valid if MB_VIDEO_MODE is set.
    u32 mode_type;
    u32 width;
    u32 height;
    u32 depth;
} MultibootHeader;

typedef struct {
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
} MultibootElfSectionHeaderTable;

typedef struct {
    u32 control_info;
    u32 mode_info;
    u16 mode;
    u16 interface_seg;
    u16 interface_off;
    u16 interface_len;
} MultibootInfoVbe;

typedef struct {
    u64 addr;
    u32 pitch;
    u32 width;
    u32 height;
    u8  bpp;
#define MB_FRAMEBUFFER_TYPE_INDEXED  0
#define MB_FRAMEBUFFER_TYPE_RGB      1
#define MB_FRAMEBUFFER_TYPE_EGA_TEXT 2
    u8  type;
    union {
        struct {
            u32 addr; // Pointer to array of Rgb24.
            u16 num_colors;
        } palette;
        struct {
            u8 r_bits_offset;
            u8 r_num_bits;
            u8 g_bits_offset;
            u8 g_num_bits;
            u8 b_bits_offset;
            u8 b_num_bits;
        };
    };
} MultibootFramebuffer;

typedef struct {
    // Multiboot info version number
    u32 flags;

    // Available memory from BIOS
    struct {
        u32 lower;
        u32 upper;
    } mem;

    // "root" partition
    u32 boot_device;

    // Kernel command line
    u32 cmdline;

    // Boot-Module list
    struct {
        u32 count;
        u32 addr;
    } mods;

    MultibootElfSectionHeaderTable elf_sec;

    // Memory Mapping buffer
    struct {
        u32 length;
        u32 addr;
    } mmap;

    // Drive Info buffer
    struct {
        u32 length;
        u32 addr;
    } drives;

    // ROM configuration table
    u32 config_table;

    // Boot Loader Name
    u32 boot_loader_name;

    // APM table
    u32 apm_table;

    // Video
    MultibootInfoVbe vbe;
    MultibootFramebuffer framebuffer;

} MultibootInfo;

typedef struct {
    u32 size;
    u64 addr;
    u64 len;
#define MB_MEMORY_AVAILABLE        1
#define MB_MEMORY_RESERVED         2
#define MB_MEMORY_ACPI_RECLAIMABLE 3
#define MB_MEMORY_NVS              4
#define MB_MEMORY_BADRAM           5
    u32 type;
} MultibootMmapEntry
#ifndef targets_ia32
    c_attr(packed)
#endif
;

typedef struct {
    // The memory used goes from bytes 'start' to 'end-1' inclusive
    u32 start;
    u32 end;

    // Module command line
    u32 cmdline;

    // padding to take it to 16 bytes (must be zero)
    u32 pad;
} MultibootModList;

typedef struct {
    u16 version;
    u16 cseg;
    u32 offset;
    u16 cseg_16;
    u16 dseg;
    u16 flags;
    u16 cseg_len;
    u16 cseg_16_len;
    u16 dseg_len;
} MultibootApmInfo;

#endif // ASM_FILE
