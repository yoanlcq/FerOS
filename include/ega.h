#pragma once

typedef struct {
    u8 x, y;
} EgaCursor;

static inline EgaCursor EgaCursor_new(u8 x, u8 y) {
    return (EgaCursor) { .x = x, .y = y };
}

typedef enum {
	EgaBlack = 0,
	EgaBlue = 1,
	EgaGreen = 2,
	EgaCyan = 3,
	EgaRed = 4,
	EgaMagenta = 5,
	EgaBrown = 6,
	EgaLightGrey = 7,
	EgaDarkGrey = 8,
	EgaLightBlue = 9,
	EgaLightGreen = 10,
	EgaLightCyan = 11,
	EgaLightRed = 12,
	EgaLightMagenta = 13,
	EgaLightBrown = 14,
	EgaWhite = 15
} EgaColor;

typedef struct {
    u8 ascii;
    u8 fg: 4;
    u8 bg: 4;
} EgaCell;

static inline EgaCell EgaCell_new(u8 ascii, EgaColor fg, EgaColor bg) {
    return (EgaCell) {
        .ascii = ascii, .fg = fg, .bg = bg
    };
}

#define EGA_FB ((EgaCell*) 0xB8000)
#define EGA_W ((u16)80)
#define EGA_H ((u16)25)
#define EGA_DATADESC_PORT ((u16)0x3D4)
#define EGA_DATA_PORT ((u16)0x3D5)
#define EGA_SET_CURSOR_HI_BYTE ((u8)14)
#define EGA_SET_CURSOR_LO_BYTE ((u8)15)

void ega_clear();
void ega_puts(EgaCursor position, const char *str, EgaColor fg, EgaColor bg);
void ega_puts_logd(EgaCursor position, const char *str, EgaColor fg, EgaColor bg);
void ega_set_cursor(EgaCursor position);
