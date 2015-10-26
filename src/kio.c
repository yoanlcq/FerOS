#include <kio.h>
#include <utils.h>

kio_stream kio_streams[32];
uint32_t kio_streams_index = 0;

int kio_open(const char *url) {
    /* replace "vga" by url below and the OS will work.
     * This is a rodata issue. */
    if(streqn(url, "vga", 3)) {
        kio_streams[kio_streams_index].protocol = KIO_PROT_VGA;
        kio_streams[kio_streams_index].offset = 0;
        kio_vga_open(kio_streams+kio_streams_index, url+6);
    }
    return kio_streams_index++;
}
void kio_close(int fd) {}
void kio_putc(int fd, char c) {
    switch(kio_streams[fd].protocol) {
        case KIO_PROT_VGA: kio_vga_putc(kio_streams+fd, c); break;
    }
}
 
void kio_puts(int fd, const char* str) {
	for(; *str; ++str)
		kio_putc(fd, *str);
    kio_putc(fd, '\n');
}
 
