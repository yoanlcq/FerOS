#pragma once

typedef struct {
    u8 x;
    u8 y;
} Vec2_u8;

static inline Vec2_u8 Vec2_u8_new(u8 x, u8 y) {
    return (Vec2_u8) { .x = x, .y = y };
}

