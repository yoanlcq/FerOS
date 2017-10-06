#pragma once

typedef union {
    __m128 m;
    struct _packed { f32 x, y, z, w; };
} Vec4;

typedef union {
    __m128i m128i;
    i32 c_attr(vector_size(16)) alignas(16) m;
    struct _packed { i32 x, y, z, w; };
} Vec4i;

typedef union {
    f32 m[3];
    struct _packed { f32 x, y, z; };
} Vec3;

typedef union {
    f32 m[2];
    struct _packed { f32 x, y; };
} Vec2;

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


#define Rgba_set(r_, g_, b_, a_) ((Rgba){ .r = (r_), .g = (g_), .b = (b_), .a = (a_) })

#define Vec2_set(x_, y_) ((Vec2){ .x = (x_), .y = (y_) })
#define Vec2_from(v) ({ let v__ = (v); Vec2_set(v__.x, v__.y); })

typedef struct {
    u32 x, y;
} Vec2u;

typedef struct {
    u32 w, h;
} Extent2u;


#define Vec2u_zero ((Vec2u){0})
#define Vec2u_set(x_, y_) ((Vec2u){ .x = (x_), .y = (y_) })

#define lerp(a,b,t) ({ let _t = (t); (a)*(1-_t) + (b)*_t; })


#define max3(a, b, c) (max(max((a), (b)), (c)))
#define max(a, b) (_Generic((a), \
    i32: i32_max, \
    f32: f32_max, \
    f64: f64_max \
)((a), (b)))

static inline f32 f32_max(f32 a, f32 b) {
    return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)));
}
static inline f64 f64_max(f64 a, f64 b) {
    return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)));
}
static inline i32 i32_max(i32 a, i32 b) {
    return _mm_cvtsi128_si32(_mm_max_epi32(_mm_cvtsi32_si128(a), _mm_cvtsi32_si128(b)));
}

#define min3(a, b, c) (min(min((a), (b)), (c)))
#define min(a, b) (_Generic((a), \
    i32: i32_min, \
    f32: f32_min, \
    f64: f64_min \
)((a), (b)))

static inline f32 f32_min(f32 a, f32 b) {
    return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)));
}
static inline f64 f64_min(f64 a, f64 b) {
    return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)));
}
static inline i32 i32_min(i32 a, i32 b) {
    return _mm_cvtsi128_si32(_mm_min_epi32(_mm_cvtsi32_si128(a), _mm_cvtsi32_si128(b)));
}

#define abs(x) ({ let x__ = (x); max(x__, -x__); })

typedef struct {
    Vec4 cols[4];
} Mat4;


void Vec4_stream(Vec4 *dst, Vec4 v) { _mm_stream_ps((float*) &dst->m, v.m); }
void Vec4i_stream(Vec4i *dst, Vec4i v) { _mm_stream_si128(&dst->m128i, v.m128i); }

#define store_nontemporal(dst, v) (_Generic((v), \
    __m128i: _mm_stream_si128, \
    __m128d: _mm_stream_pd, \
    __m128:  _mm_stream_ps, \
    i32:     _mm_stream_si32, \
    Vec4:    Vec4_stream, \
    Vec4i:   Vec4i_stream \
)((dst), (v)))

