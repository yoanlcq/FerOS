#ifndef KIO_H
#define KIO_H

#include <stdint.h>
#include <kio_vga.h>

typedef enum {
    KIO_PROT_VGA = 0
} kio_protocols;

typedef enum {
    KIO_READ  = 1,
    KIO_WRITE = 2,
    KIO_READ_WRITE = 3
} kio_open_flags;

struct kio_stream {
    uint8_t protocol;
    size_t offset;
    union {
        kio_vga_impl vga;
    } impl;
};
typedef struct kio_stream kio_stream;

extern kio_stream kio_streams[32];
extern uint32_t kio_streams_index;

int kio_open(const char *url);
void kio_close(int fd);
void kio_putc(int fd, char c);
void kio_puts(int fd, const char *str);

#endif /* KIO_H */
