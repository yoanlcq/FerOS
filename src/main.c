#include <stdint.h>
#include <multiboot.h>
#include <vga.h>

void main(uint32_t mb_magic, MultibootInfo *mb_info) {
    char hello[] = "Hello, kernel world!";
    for(int i=0 ; hello[i] ; ++i) {
        VGA_FB[i] = VgaCell_new(hello[i], VgaRed, VgaBlue);
    }
}
