#include <kio.h>
#include <kio_vga.h>
#include <vga.h>

void kio_vga_open(kio_stream *s, const char *path) {
    /* if(streq(path, "display")) */
    s->impl.vga.ctx = (vga_context*) &vga_display;
    s->impl.vga.x = 0;
    s->impl.vga.y = 0;
    s->impl.vga.fg = VGA_LIGHT_GREY;
    s->impl.vga.bg = VGA_RED;
    vga_init(s->impl.vga.ctx);
}
void kio_vga_close(kio_stream *s) {}
void kio_vga_putc(kio_stream *s, char c) {
    vga_set(s->impl.vga.ctx, s->impl.vga.x, s->impl.vga.y,
            c, s->impl.vga.fg, s->impl.vga.bg); 
    if(++(s->impl.vga.x) >= VGA_WIDTH) {
        s->impl.vga.x = 0;
        if(++(s->impl.vga.y) >= VGA_HEIGHT) {
            s->impl.vga.y = 0;
            /* Handle overflow */
        }
    }
}
