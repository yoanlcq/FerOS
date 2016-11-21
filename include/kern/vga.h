#ifndef KERN_VGA_H
#define KERN_VGA_H

#include <kern/stdc.h>

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
	VGA_WHITE = 15
};
typedef enum vga_color vga_color; 

#define VGA_BUFADDR ((void*)0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

#define vga_entry(c, fg, bg) ((c) | ((fg)<<8) | ((bg)<<12))

#endif /* KERN_VGA_H */
