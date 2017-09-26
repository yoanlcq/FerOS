#pragma once

typedef struct { u8 r, g, b;    } Rgb24 ;
typedef struct { u8 r, g, b, a; } Rgba32;

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

void noto_mono_rasterize_rgba32(Rgba32 *out, const char *str, Rgba32 color);
