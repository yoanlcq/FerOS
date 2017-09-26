#pragma once

typedef u32 VbeFarPtr;

typedef struct {
    u8 major, minor;
} VbeVersion;

typedef struct {
    char vbe_signature[4];
    VbeVersion vbe_version;
    VbeFarPtr oem_string_ptr;
    u32 capabilities;
    VbeFarPtr videomode_ptr;
    u16 total_memory; // Number of 64KB memory blocks
    VbeVersion oem_software_rev;
    VbeFarPtr oem_vendor_name_ptr;
    VbeFarPtr oem_product_name_ptr;
    VbeFarPtr oem_product_rev_ptr;
    u8 reserved;
    u8 oem_data[256]; // TODO Only if 'VBE2' was put by in `vbe_signature` by GRUB
} VbeInfoBlock;

typedef struct {
    u16 mode_attributes;
    u8 win_a_attributes;
    u8 win_b_attributes;
    u16 win_granularity;
    u16 win_size;
    u16 win_a_segment;
    u16 win_b_segment;
    u32 win_func_ptr;
    u16 bytes_per_scanline;

    u16 x_resolution;
    u16 y_resolution;
    u8 x_char_size;
    u8 y_char_size;
    u8 number_of_planers;
    u8 bits_per_pixel;
    u8 number_of_banks;
    u8 memory_model;
    u8 bank_size;
    u8 number_of_image_pages;
    u8 reserved0;

    // Direct color fields
    u8 red_mask_size; // In bits
    u8 red_field_position; // Bit position of lsb
    u8 green_mask_size; // In bits
    u8 green_field_position; // Bit position of lsb
    u8 blue_mask_size; // In bits
    u8 blue_field_position; // Bit position of lsb
    u8 reserved_mask_size;
    u8 reserved_field_position;
    u8 direct_color_mode_info;

    // VBE 2+
    u32 phys_base_ptr;
    u32 reserved1;
    u16 reserved2;

    // VBE 3+
    u16 lin_bytes_per_scan_line;
    u8 bank_number_of_image_pages;
    u8 lin_number_of_image_pages;
    u8 lin_red_mask_size; // In bits
    u8 lin_red_field_position; // Bit position of lsb
    u8 lin_green_mask_size; // In bits
    u8 lin_green_field_position; // Bit position of lsb
    u8 lin_blue_mask_size; // In bits
    u8 lin_blue_field_position; // Bit position of lsb
    u8 lin_reserved_mask_size;
    u8 lin_reserved_field_position;
    u32 max_pixel_clock; // Hz

    u8 reserved3;
} VbeModeInfoBlock;
