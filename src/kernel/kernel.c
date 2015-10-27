#include <stddef.h>
#include <stdint.h>
#include <kernel.h>
#include <node.h>

void kernel_main(void) {
    node n = nopen("vga://display", N_WRITE);
    nputs(n, "Hello, kernel World!");
    nclose(n);
    /*
    int i;
    for(i=0 ; i<VGA_WIDTH ; ++i) {
        ((uint16_t*)VGA_BUFADDR)[i] = ('0'+i) | (i%15)<<8 | (i+8%15)<<12;
    }
    */
}
