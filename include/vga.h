#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

enum vga_color {
	VGA_BLACK = 0,
	VGA_BLUE = 1,
	VGA_GREEN = 2,
	VGA_CYAN = 3,
	VGA_RED = 4,
	VGA_MAGENTA = 5,
	VGA_BROWN = 6,
	VGA_LIGHT_GREY = 7,
	VGA_DARK_GREY = 8,
	VGA_LIGHT_BLUE = 9,
	VGA_LIGHT_GREEN = 10,
	VGA_LIGHT_CYAN = 11,
	VGA_LIGHT_RED = 12,
	VGA_LIGHT_MAGENTA = 13,
	VGA_LIGHT_BROWN = 14,
	VGA_WHITE = 15,
};
typedef enum vga_color vga_color; 

#define VGA_BUFADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

struct vga_context {
    uint16_t buf[VGA_HEIGHT][VGA_WIDTH];
};
typedef struct vga_context vga_context;

struct vga_contextp {
    uint16_t *buf;
};

extern struct vga_contextp vga_display;

void vga_init(vga_context *ctx);
void vga_deinit(vga_context *ctx);
void vga_set(vga_context *ctx, uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg);

#endif /* VGA_H */
