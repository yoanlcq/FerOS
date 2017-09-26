#include <ega.h>
#include <log.h>

void ega_clear() {
    static_assert(sizeof (EgaCell) == 2, "");
    zero_u16((u16*)EGA_FB, EGA_W * EGA_H);
}
void ega_puts(EgaCursor p, const char *str, EgaColor fg, EgaColor bg) {
    for(u16 i=0 ; str[i] ; ++i) {
        u16 cell_location = p.y*EGA_W + p.x + i;
        assert_cmp(cell_location, <, EGA_H*EGA_W, "Won't overrun video memory!");
        EGA_FB[cell_location] = EgaCell_new(str[i], fg, bg);
    }
}
void ega_puts_logd(EgaCursor p, const char *str, EgaColor fg, EgaColor bg) {
    logd(str);
    ega_puts(p, str, fg, bg);
}
void ega_set_cursor(EgaCursor p) {
    u16 i = p.y*EGA_W + p.x;
    assert_cmp(i, <, EGA_H*EGA_W, "Tried to set cursor beyond available video memory!");
    outb(EGA_DATADESC_PORT, EGA_SET_CURSOR_HI_BYTE);
    outb(EGA_DATA_PORT, i>>8);
    outb(EGA_DATADESC_PORT, EGA_SET_CURSOR_LO_BYTE);
    outb(EGA_DATA_PORT, i);
}

