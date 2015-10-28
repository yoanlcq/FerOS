#include <stddef.h>
#include <kernel/kernel.h>
#include <kernel/node.h>

void kernel_main(void) {
    node n = nopen("vga://", N_WRITE); /* Calls nopen_vga(NULL, N_WRITE) */
    nputs(n, "Hello, kernel World!");  /* Calls nputs_vga(n, "str"); */
    nclose(n);                         /* Calls nclose_vga(n); */
    /*
    int i;
    for(i=0 ; i<VGA_WIDTH ; ++i) {
        ((uint16_t*)VGA_BUFADDR)[i] = ('0'+i) | (i%15)<<8 | (i+8%15)<<12;
    }
    */
}
