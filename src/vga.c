#include <vga.h>
#include <log.h>

void vga_clear() {
    static_assert(sizeof (VgaCell) == 2, "");
    zero_u16((u16*)VGA_FB, VGA_W * VGA_H);
}
void vga_puts(VgaCursor p, const char *str, VgaColor fg, VgaColor bg) {
    for(u16 i=0 ; str[i] ; ++i) {
        u16 cell_location = p.y*VGA_W + p.x + i;
        assert_cmp(cell_location, <, VGA_H*VGA_W, "Won't overrun video memory!");
        VGA_FB[cell_location] = VgaCell_new(str[i], fg, bg);
    }
}
void vga_puts_logd(VgaCursor p, const char *str, VgaColor fg, VgaColor bg) {
    logd(str);
    vga_puts(p, str, fg, bg);
}
void vga_set_cursor(VgaCursor p) {
    u16 i = p.y*VGA_W + p.x;
    assert_cmp(i, <, VGA_H*VGA_W, "Tried to set cursor beyond available video memory!");
    outb(VGA_DATADESC_PORT, VGA_SET_CURSOR_HI_BYTE);
    outb(VGA_DATA_PORT, i>>8);
    outb(VGA_DATADESC_PORT, VGA_SET_CURSOR_LO_BYTE);
    outb(VGA_DATA_PORT, i);
}

