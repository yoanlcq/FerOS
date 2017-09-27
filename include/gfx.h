#pragma once

typedef union {
    __m128 m;
    struct _packed { f32 x, y, z, w; };
} Vec4;

static_assert(alignof(Vec4) == alignof(__m128), "");
static_assert( sizeof(Vec4) ==  sizeof(__m128), "");

typedef union {
    __m128 m;
    struct _packed { f32 r, g, b, a; };
} Rgba;

static_assert(alignof(Rgba) == alignof(__m128), "");
static_assert( sizeof(Rgba) ==  sizeof(__m128), "");

typedef struct _packed { u8 r, g, b;    } Rgb24 ;
typedef struct _packed { u8 r, g, b, a; } Rgba32;

typedef struct {
    u32 x, y;
} Vec2u;

typedef struct {
    u32 w, h;
} Extent2u;

typedef struct {
    Vec2u;
    Extent2u;
} FbArea;

typedef struct {
    u8 bit_mask, bit_position;
} BitMaskPosition;

typedef struct {
    void *data;
    Extent2u extent;
    u32 pitch;
    BitMaskPosition r, g, b;
    u32 bpp;
} VbeFb;

typedef struct {
    Extent2u;
    Rgba *pixels;
    Vec4 *depth;
} RgbaFb;

// Type-safe enum
typedef struct { u8 enumval; } DepthTest;
typedef struct { u8 enumval; } BlendOp;
#define DepthAlways ((DepthTest){0})
#define DepthBelow  ((DepthTest){1})
#define BlendCopy   ((BlendOp){0})
#define BlendAlpha  ((BlendOp){1})
#define BlendMul    ((BlendOp){2})

void RgbaFb_blend(
    RgbaFb *dst, FbArea dstarea,
    const RgbaFb *src, Vec2u srcpos,
    BlendOp blend, DepthTest test
);


typedef struct {
    u16 w, h;
    Rgb24 pixels[];
} Rgb24Image;

typedef struct {
    u16 w, h;
    Rgba32 pixels[];
} Rgba32Image;

typedef struct {
    usize w, h, char_w;
    u8 bits[];
} XbmMonoFont;

extern const XbmMonoFont noto_mono;

void XbmMonoFont_rasterize(const XbmMonoFont *f, RgbaFb *dst, const char *str, usize str_len, Rgba color);
