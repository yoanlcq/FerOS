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

typedef struct {
    Rgba32 *pixels;
    Extent2u;
} Rgba32Image;

typedef struct {
    Rgba32Image image;
    enum {
        FilteringLinear,
        FilteringNearest
    } min_filter, max_filter;
    enum {
        WrapClamp,
        WrapClampToEdge,
        WrapClampToBorder,
        WrapRepeat,
        WrapRepeatMirrored
    } wrapping;
} Sampler2D;

typedef struct {
    Vec3 position;
    Rgba color;
    // TODO: Other attributes here
} Light;

// NOTE: For now there's no `topology` member. The topology is implied
// by the `*rasterize*` function.
typedef struct {
    struct {
        Vec4* position;
        Rgba* color;
        Vec2* texcoords;
        Vec3* normal;
    } vertices;
    usize vertex_count;
    u32* indices;
    usize indices_count;
    Mat4 mvp, vp, normal_matrix;
    Sampler2D sampler;
    Light *lights;
    usize light_count;
} RasterBatch;

void RgbaFb_rasterize(RgbaFb *dst, const RasterBatch *b);
