#include <stddef.h>
#include <stdint.h>
#include <kernel/compiletime.h>
#include <kernel/vga.h>

void kernel_main(void) {
    /*
    node n = nopen("vga://", N_WRITE);
    nputs(n, "Hello, kernel World!"); 
    nclose(n);                        
    */
    int x, y;
    uint16_t offset, step, entry, *vga = (uint16_t*)VGA_BUFADDR;
    for(step=y=0 ; y<VGA_HEIGHT ; ++y, step=0) {
        for(offset=x=0 ; x<VGA_WIDTH ; ++x, ++step, step%=4) {
            if(!step) {
                ++offset;
                offset %= 6;
                entry = vga_entry('0'+y, 9+offset, 1+offset);
            }
            vga[y*VGA_WIDTH+x] = entry;
        }
    }
}
