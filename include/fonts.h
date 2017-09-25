#pragma once

#include <colors.h>

typedef struct {
    usize w, h, char_w;
    u8 bits[];
} XbmMonoFont;

extern const XbmMonoFont noto_mono;

void noto_mono_rasterize_rgba32(Rgba32 *out, const char *str, Rgba32 color);
