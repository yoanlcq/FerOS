#pragma once

#include <vmath.h>

typedef struct {
    void *pixels;
    u32 pitch;
    union {
        Extent2u;
        Extent2u extent;
    };
    u32 bits_per_pixel; // solve ambiguity, "bpp" could mean "bytes per pixel".
    struct {
        u32 num_bits, bits_offset;
    } r, g, b;
} VbeRgbFb;

typedef struct {
    union {
        Extent2u;
        Extent2u extent;
    };
    Rgba *pixels;
    Vec4 *depth; // Array of depth values for 4 fragments
} RgbaFb;

typedef struct {
    union {
        Vec2u;
        Vec2u position;
    };
    union {
        Extent2u;
        Extent2u extent;
    };
} FbArea;


typedef struct { 
    enum {
        BlendCopy,
        BlendAlpha,
        BlendMul
    } enumval;
} BlendOp;

typedef struct {
    enum {
        DepthAlways,
        DepthBelow,
        DepthBelowOrEqual
    } enumval;
} DepthTest;

typedef struct {
    usize w, h, char_w;
    u8 *bits;
} XbmMonoFont;


extern const XbmMonoFont noto_mono;

void XbmMonoFont_rasterize(
    const XbmMonoFont *f, RgbaFb *dst,
    const char *str, usize str_len, Rgba color
);

void RgbaFb_blend(
    RgbaFb *dst, Vec2u dst_start,
    const RgbaFb *src, Vec2u src_start,
    Extent2u size, BlendOp blend_op, DepthTest depth_test
);

void VbeRgbFb_copy_RgbaFb(
    VbeRgbFb *restrict dst, Vec2u dst_start,
    const RgbaFb *restrict src, Vec2u src_start,
    Extent2u size
);

// NOTE: Everything about what follows sucks in its own way. It's only to get
// the basics going.

typedef struct {
    Vec4 position;
    Rgba color;
} Vertex;

typedef union {
    struct { Vertex v0, v1, v2; };
    Vertex v[3];
} Triangle;

void RgbaFb_rasterize(RgbaFb dst, const Triangle *tri, usize count);
