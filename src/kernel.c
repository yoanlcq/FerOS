#include <stddef.h>
#include <stdint.h>
#include <kernel.h>
#include <kio.h>

void kmain(void) {
    /*
    int fd = kio_open("vga://display");
    kio_puts(fd, "Hello, kernel World!");
    kio_close(fd);
    */
    int i;
    for(i=0 ; i<VGA_WIDTH ; ++i) {
        ((uint16_t*)VGA_BUFADDR)[i] = ('0'+i) | (i%15)<<8 | (i+8%15)<<12;
    }
}
