#include <stdint.h>
#include <vga.h>

struct vga_contextp vga_display = { (uint16_t*) VGA_BUFADDR };

void vga_init(vga_context *ctx) {
    uint16_t x, y;
    for(y=0 ; y<VGA_HEIGHT ; ++y)
        for(x=0 ; x<VGA_WIDTH ; ++x)
            vga_set(ctx, x, y, ' ', VGA_LIGHT_GREY, VGA_RED);
}
void vga_deinit(vga_context *ctx) {}
void vga_set(vga_context *ctx, uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg) {
    ctx->buf[y][x] = c | fg<<12 | bg<<8;
}
