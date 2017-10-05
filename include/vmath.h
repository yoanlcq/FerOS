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
    f32 x, y;
} Vec2;

typedef struct {
    u32 x, y;
} Vec2u;

typedef struct {
    u32 w, h;
} Extent2u;


#define Vec2u_zero ((Vec2u){0})

#define lerp(a,b,t) ({ let _t = (t); (a)*(1-_t) + (b)*_t; })
