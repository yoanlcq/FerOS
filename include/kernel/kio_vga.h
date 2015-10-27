#ifndef KIO_VGA_H
#define KIO_VGA_H

#include <vga.h>

struct kio_stream;
typedef struct kio_stream kio_stream;

struct kio_vga_impl {
    vga_context *ctx;
    uint16_t x, y;
    uint16_t fg, bg;
};
typedef struct kio_vga_impl kio_vga_impl;

void kio_vga_open(kio_stream *s, const char *path);
void kio_vga_close(kio_stream *s);
void kio_vga_putc(kio_stream *s, char c);

#endif /* KIO_VGA_H */
