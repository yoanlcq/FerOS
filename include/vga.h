#pragma once

#include <stdint.h>

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
    uint8_t bg: 4;
    uint8_t fg: 4;
    uint8_t ascii;
} VgaCell;

static inline VgaCell VgaCell_new(uint8_t ascii, VgaColor fg, VgaColor bg) {
    return (VgaCell) {
        .ascii = ascii, .fg = fg, .bg = bg
    };
}

#define VGA_FB ((VgaCell*)0xB8000)
#define VGA_W 80
#define VGA_H 25
