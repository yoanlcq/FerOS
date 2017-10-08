#pragma once

#include <vmath.h>

// NOTE: Quite everything in this module is a blatant work-in-progress.
//
// References:
// https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
// https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
// https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes

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
    Vec4* vposition;
    Rgba* vcolor;
    Vec2* vtexcoords;
    Vec4* vnormal;
    usize vertex_count;
} Vbo, VShaderInput, VShaderOutput;

typedef struct {
    u32* indices;
    usize index_count;
} Ebo;

typedef struct {
    Mat4 mvp, vp, normal_matrix;
    Sampler2D sampler;
    Light *lights;
    usize light_count;
} Uniforms;

typedef struct {
    Vbo vbo;
    Ebo ebo;
    Uniforms uniforms;
} RasterBatch;

typedef struct {
    float *frag_depth;
    Vec2u *frag_pos;
    Vec2  *vtexcoords;
    Vec3  *vnormal;
    usize count;
} FShaderInput;

typedef struct {
    Rgba *frag_color;
    usize count;
} FShaderOutput;

void RgbaFb_rasterize(RgbaFb *dst, const RasterBatch *b);

// Misc notes:
// - Divide by w;
// - Perspective-correct interpolation: C = Z * lerp(C0/Z0, C1/Z1, q);
