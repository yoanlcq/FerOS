#include <vga.h>
#include <log.h>

void vga_puts(const char *str, VgaColor fg, VgaColor bg) {
    for(int i=0 ; str[i] ; ++i) {
        VGA_FB[i] = VgaCell_new(str[i], fg, bg);
    }
}
void vga_puts_logd(const char *str, VgaColor fg, VgaColor bg) {
    logd(str);
    vga_puts(str, fg, bg);
}
