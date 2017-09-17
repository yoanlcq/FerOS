#pragma once

#include <vec.h>

typedef enum {
	VgaBlack = 0,
	VgaBlue = 1,
	VgaGreen = 2,
	VgaCyan = 3,
	VgaRed = 4,
	VgaMagenta = 5,
	VgaBrown = 6,
	VgaLightGrey = 7,
	VgaDarkGrey = 8,
	VgaLightBlue = 9,
	VgaLightGreen = 10,
	VgaLightCyan = 11,
	VgaLightRed = 12,
	VgaLightMagenta = 13,
	VgaLightBrown = 14,
	VgaWhite = 15
} VgaColor;

typedef struct {
    u8 ascii;
    u8 fg: 4;
    u8 bg: 4;
} VgaCell;

static inline VgaCell VgaCell_new(u8 ascii, VgaColor fg, VgaColor bg) {
    return (VgaCell) {
        .ascii = ascii, .fg = fg, .bg = bg
    };
}

#define VGA_FB ((VgaCell*) 0xB8000)
#define VGA_W ((u16)80)
#define VGA_H ((u16)25)
#define VGA_DATADESC_PORT ((u16)0x3D4)
#define VGA_DATA_PORT ((u16)0x3D5)
#define VGA_SET_CURSOR_HI_BYTE ((u8)14)
#define VGA_SET_CURSOR_LO_BYTE ((u8)15)

void vga_clear();
void vga_puts(Vec2_u8 position, const char *str, VgaColor fg, VgaColor bg);
void vga_puts_logd(Vec2_u8 position, const char *str, VgaColor fg, VgaColor bg);
void vga_set_cursor(Vec2_u8 position);
