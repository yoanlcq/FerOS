#include <gfx.h>
#include <string.h>

// Pipeline overview:
// 
// vpos, vtexcoords, vnormal

typedef struct {
    struct {
        Vec4 *position;
        Vec2 *texcoords;
        Vec4 *normal;
    } vertices;
} Foo;

// Returns:
//  < 0: `c` lies in the half-space right of segment `ab`.
// == 0: `c` lies in the infinite line along segment `ab`.
//  > 0: `c` lies in the half-space left of segment `ab`.
static inline float orient2d(Vec2 a, Vec2 b, Vec2 c) {
    // WISH: We can simplify so it compiles to fewer instructions.
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static inline float Triangle_area(Vec2 a, Vec2 b, Vec2 c) {
    return abs(orient2d(a, b, c))/2.0f;
}

// TODO: THIS IS NOT FINISHED!
// Which coordinate type are we in ? Integers ? Floats ?
void RgbaFb_rasterize_triangles(RgbaFb *dst, const RasterBatch *b) {
    (void) dst;
    assert_cmp(b->ebo.index_count % 3, ==, 0u, "Indices must describe triangles!");
    for(usize tri_i=0 ; tri_i < b->ebo.index_count/3 ; ++tri_i) {
        let v0 = Vec2_from(b->vbo.vposition[b->ebo.indices[tri_i*3 + 0]]);
        let v1 = Vec2_from(b->vbo.vposition[b->ebo.indices[tri_i*3 + 1]]);
        let v2 = Vec2_from(b->vbo.vposition[b->ebo.indices[tri_i*3 + 2]]);
        let minx = min3(v0.x, v1.x, v2.x);
        let miny = min3(v0.y, v1.y, v2.y);
        let maxx = max3(v0.x, v1.x, v2.x);
        let maxy = max3(v0.y, v1.y, v2.y);
        for(usize y=miny ; y<maxy ; ++y) for(usize x=minx ; x<maxx ; ++x) {
            let p = Vec2_set(x, y);
            let w0 = orient2d(v0, v1, p);
            let w1 = orient2d(v1, v2, p);
            let w2 = orient2d(v2, v0, p);
            if(w0 < 0 || w1 < 0 || w2 < 0) {
                continue;
            }
            let twice_area = abs(orient2d(v0, v1, v2)); // But do we need abs() ?
            let wn0 = w0 / twice_area;
            let wn1 = w1 / twice_area;
            let wn2 = w2 / twice_area;

            let vcolor0 = b->vbo.vcolor[b->ebo.indices[tri_i*3 + 0]];
            let vcolor1 = b->vbo.vcolor[b->ebo.indices[tri_i*3 + 1]];
            let vcolor2 = b->vbo.vcolor[b->ebo.indices[tri_i*3 + 2]];

            __m128 rgba = vcolor0.m*wn0 + vcolor1.m*wn1 + vcolor2.m*wn2;
            dst->pixels[y*dst->w + x] = (Rgba) { .m = rgba };
        }
    }
}

// WISH: Improve this with non-temporal SIMD instructions, and sfence.
void RgbaFb_blend(
    RgbaFb *dst, Vec2u dst_start,
    const RgbaFb *src, Vec2u src_start,
    Extent2u size, BlendOp blend_op, DepthTest depth_test
) {
    assert_cmp(dst_start.x + size.w, <=, dst->w, "");
    assert_cmp(dst_start.y + size.h, <=, dst->h, "");
    assert_cmp(src_start.x + size.w, <=, src->w, "");
    assert_cmp(src_start.y + size.h, <=, src->h, "");
    for(usize y_=0 ; y_<size.h ; ++y_) for(usize x_=0 ; x_<size.w ; ++x_) {
        let dx = dst_start.x + x_;
        let dy = dst_start.y + y_;
        let sx = src_start.x + x_;
        let sy = src_start.y + y_;
        dst->pixels[dy * dst->w + dx] = src->pixels[sy * src->w + sx];
        switch(blend_op.enumval) {
        case BlendCopy: break;
        case BlendAlpha: break;
        case BlendMul: break;
        default: break;
        }
        switch(depth_test.enumval) {
        case DepthAlways: break;
        case DepthBelow: break;
        case DepthBelowOrEqual: break;
        default: break;
        }
    }
}

static inline u16 u16_swap(u16 i) {
    return i<<8 | ((i>>8)&0xff);
}

// NOTE: Only 32 bits-per-pixel modes could be tested.
void VbeRgbFb_copy_RgbaFb(
    VbeRgbFb *restrict dst, Vec2u dst_start,
    const RgbaFb *restrict src, Vec2u src_start,
    Extent2u size
) {

    switch(dst->bits_per_pixel) {
    case 15: /* 5:5:5 */
        // fallthrough
    case 16: /* 5:6:5 */
        for(usize y_=0 ; y_<size.h ; ++y_) for(usize x_=0 ; x_<size.w ; ++x_) {
            let dx = dst_start.x + x_;
            let dy = dst_start.y + y_;
            let sx = src_start.x + x_;
            let sy = src_start.y + y_;
            let color = src->pixels[sy*src->w + sx];
            let dst8 = (u8*) dst->pixels;

            let idx = dy*dst->pitch + dx*((dst->bits_per_pixel+1)/8);

            u8 r = color.r * ((1 << dst->r.num_bits) - 1);
            u8 g = color.g * ((1 << dst->g.num_bits) - 1);
            u8 b = color.b * ((1 << dst->b.num_bits) - 1);
            u16 color16 = 
                  r << ((16 - dst->r.bits_offset) - dst->r.num_bits)
                | g << ((16 - dst->g.bits_offset) - dst->g.num_bits)
                | b << ((16 - dst->b.bits_offset) - dst->b.num_bits)
                ;
            color16 = u16_swap(color16);
            *(u16*)&dst8[idx] = color16;
        }
        break;
    case 24: /* 8:8:8 */
        // fallthrough
    case 32: /* 8:8:8:8 */
        for(usize y_=0 ; y_<size.h ; ++y_) for(usize x_=0 ; x_<size.w ; ++x_) {
            let dx = dst_start.x + x_;
            let dy = dst_start.y + y_;
            let sx = src_start.x + x_;
            let sy = src_start.y + y_;
            let color = src->pixels[sy*src->w + sx];
            let dst8 = (u8*) dst->pixels;

            let idx = dy*dst->pitch + dx*(dst->bits_per_pixel/8);

            dst8[idx + dst->r.bits_offset/8] = color.r*0xff;
            dst8[idx + dst->g.bits_offset/8] = color.g*0xff;
            dst8[idx + dst->b.bits_offset/8] = color.b*0xff;
        }
        break;
    default: 
        // Draw garbage because we don't support this mode
        for(usize y_=0 ; y_<size.h ; ++y_) for(usize x_=0 ; x_<size.w ; ++x_) {
            let dx = dst_start.x + x_;
            let dy = dst_start.y + y_;
            let sx = src_start.x + x_;
            let sy = src_start.y + y_;
            let color = src->pixels[sy*src->w + sx];
            let dst8 = (u8*) dst->pixels;
            let idx = dy*dst->pitch + dx*((dst->bits_per_pixel+7)/8);
            dst8[idx] = ((color.r + color.g + color.b)/3.0f)*0xff;
        }
        break;
    }
}

void XbmMonoFont_rasterize(const XbmMonoFont *f, RgbaFb *dst, const char *str, usize str_len, Rgba color) {

    assert_cmp(str_len, <=, strlen(str), "");
    
    for(usize si=0 ; si < str_len && str[si] ; ++si) {
        u8 ascii = (u8) str[si];

        for(usize sy=0 ; sy < f->h ; ++sy) {
            for(usize sx=0 ; sx < f->char_w ; ++sx) {
                // The "%8" mess just pads width to the next multiple of 8, if
                // width isn't already a multiple of 8.
                // This is because how each row is padded to fit a byte
                // boundary in XBM data.
                usize bit_i = sy * (f->w + (8-(f->w%8))%8) + ascii * f->char_w + sx;
                usize skip_bytes = bit_i / 8;
                usize skip_bits  = bit_i % 8;
                bool is_lit = !(f->bits[skip_bytes] & (1<<skip_bits));
                // logd_(is_lit ? "1" : "0");
                dst->pixels[sy * dst->w + si * f->char_w + sx] = is_lit ? color : (Rgba){0};
            }
            // logd("");
        }
    }
}

// The Noto Mono font, rasterized with GIMP and exported to XBM.
// The set is ASCII-extended, which means one can directly look-up
// characters with an unsigned byte.
//
// Couple notes:
// The image has been made such that its width is a multiple of 8. This is
// because each image row is padded (with undefined bits) to fit into whole
// bytes otherwise (and it complexifies rendering).
// A byte is a pack of 8 bits, 1 bit = 1 pixel. 0 is white, 1 is black.
// The bit order is reversed, that is, for the given byte array `b` :
// - (b[0] & (1<<0)) is the mask for the 1st pixel.
// - (b[0] & (1<<7)) is the mask for the 8th pixel;
// - (b[1] & (1<<0)) is the mask for the 9th pixel.
// - (b[1] & (1<<7)) is the mask for the 16th pixel;
// - etc.
static u8 noto_mono_bits[];
const XbmMonoFont noto_mono = {
    .w = 1792, .h = 16, .char_w = 7, .bits = noto_mono_bits
};
// Kindly generated by GIMP's XBM export
static u8 noto_mono_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff,
   0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xef, 0xfb, 0xf4, 0xff,
   0xff, 0xff, 0xf7, 0xef, 0xfb, 0x7f, 0xff, 0xbe, 0xff, 0xff, 0xd3, 0xfd,
   0xfb, 0x3e, 0xfd, 0xff, 0xff, 0xfb, 0xf7, 0xfd, 0xff, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd7,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xef, 0xf7, 0xf5, 0x72, 0x7d, 0xff, 0xff, 0xef, 0xf7,
   0xf5, 0xfa, 0x7e, 0x5f, 0xaf, 0xff, 0xcb, 0xfb, 0x7d, 0xbd, 0x5c, 0xff,
   0xff, 0xf7, 0xfb, 0x7a, 0x7d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xbf, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x70, 0x38, 0x1c,
   0x0e, 0x87, 0xc3, 0xe1, 0x70, 0x38, 0x1c, 0x0e, 0x87, 0xc3, 0xe1, 0x70,
   0x38, 0x1c, 0x0e, 0x87, 0xc3, 0xe1, 0x70, 0x38, 0x1c, 0x0e, 0x87, 0xc3,
   0xff, 0x73, 0x7b, 0x3b, 0xec, 0xce, 0xef, 0xe7, 0xf9, 0xfd, 0xff, 0xff,
   0xff, 0xbf, 0xe3, 0xf9, 0x78, 0xfc, 0x0e, 0x1e, 0x83, 0xe3, 0xf1, 0xff,
   0xff, 0xff, 0xff, 0xc3, 0xe3, 0x7b, 0xf8, 0x18, 0x0f, 0x06, 0x87, 0xdd,
   0xe0, 0xbb, 0xd7, 0xef, 0x76, 0xc7, 0xe1, 0x71, 0x78, 0x18, 0xe8, 0xfa,
   0x7c, 0x3e, 0x5f, 0x70, 0xdc, 0x1f, 0xdf, 0xff, 0xf7, 0x7f, 0xff, 0xff,
   0xfd, 0x1f, 0xff, 0xfd, 0xfb, 0xbb, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x79, 0xcf, 0xff, 0xc3,
   0xc3, 0xff, 0xff, 0xf8, 0xff, 0xdf, 0xef, 0x77, 0xfc, 0xf0, 0x1f, 0xfc,
   0x07, 0xff, 0xff, 0xf7, 0x7d, 0xbb, 0xfd, 0xff, 0xff, 0x53, 0xe8, 0xfa,
   0xff, 0xff, 0xaf, 0x7d, 0xff, 0xff, 0xfd, 0xf8, 0xf7, 0xdd, 0x87, 0xeb,
   0x71, 0xf8, 0xff, 0x1f, 0xff, 0xe7, 0xff, 0xf1, 0xf8, 0xfd, 0x1f, 0xfe,
   0xff, 0xf3, 0xf9, 0x9f, 0xcb, 0xc5, 0xfe, 0xef, 0xf7, 0xfb, 0xfd, 0x7e,
   0x1f, 0x1e, 0x83, 0xc1, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xe3, 0xdd, 0xf1,
   0x78, 0x3c, 0x1e, 0xff, 0x87, 0xdd, 0x6e, 0xb7, 0xeb, 0xeb, 0x8f, 0xf7,
   0xef, 0xf7, 0xf4, 0xbf, 0xfe, 0xff, 0xf7, 0xef, 0xfb, 0x7f, 0xff, 0xbe,
   0xff, 0xd7, 0xd3, 0xfd, 0xfb, 0x3e, 0xfd, 0xff, 0xff, 0xfb, 0xf7, 0xfd,
   0xff, 0xee, 0xff, 0xff, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf,
   0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e,
   0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xff, 0x7b, 0x7b, 0x5b,
   0x57, 0xb7, 0xef, 0xf7, 0xfb, 0xfd, 0xff, 0xff, 0xff, 0xdf, 0xc9, 0xfa,
   0xb7, 0x7b, 0xee, 0xef, 0xbf, 0xdd, 0xe6, 0xff, 0xff, 0xff, 0xff, 0xbf,
   0xdd, 0x75, 0x77, 0xdf, 0xee, 0xf7, 0xf7, 0xdd, 0xfb, 0xbb, 0xdb, 0xef,
   0x76, 0x93, 0xdd, 0x64, 0xb7, 0x7f, 0xef, 0x72, 0x7c, 0xdd, 0xee, 0x77,
   0xbf, 0x7f, 0xaf, 0xff, 0xef, 0x7f, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xfd,
   0xff, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x7e, 0xbf, 0xff, 0xbf, 0xfb, 0xff, 0xff, 0xfe,
   0xff, 0xdf, 0xef, 0x6b, 0x7d, 0xff, 0x4f, 0xff, 0x7f, 0xff, 0xff, 0xf3,
   0x3c, 0x99, 0xfc, 0xff, 0xff, 0xcb, 0xea, 0xfd, 0xff, 0xff, 0xdf, 0xbb,
   0xff, 0xff, 0x70, 0xff, 0xef, 0xde, 0xfb, 0xff, 0xee, 0xfb, 0xff, 0xef,
   0xfe, 0xdb, 0xff, 0xf7, 0xfb, 0xfe, 0x8f, 0xfe, 0xff, 0xf7, 0xf6, 0xbf,
   0xdd, 0x5e, 0xff, 0xd7, 0xeb, 0xf5, 0x7a, 0xbd, 0x9e, 0xef, 0xfb, 0xfd,
   0x7e, 0xff, 0x7e, 0xbf, 0xdf, 0xdb, 0xdd, 0x64, 0x32, 0x99, 0x4c, 0xfe,
   0x93, 0xdd, 0x6e, 0xb7, 0xdb, 0xed, 0xb7, 0xef, 0xf7, 0xeb, 0x72, 0x7d,
   0xff, 0xff, 0xef, 0xf7, 0xf5, 0xfa, 0x7e, 0x5f, 0xaf, 0xef, 0xcb, 0xfb,
   0x7d, 0xbd, 0x5c, 0xff, 0xff, 0xf7, 0xfb, 0x7a, 0x7d, 0xef, 0xaf, 0xff,
   0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
   0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7,
   0xfb, 0xfd, 0x7e, 0xbf, 0xff, 0x7b, 0x1b, 0x50, 0x57, 0xb7, 0xef, 0xf3,
   0x73, 0xf0, 0xfe, 0xff, 0xff, 0xdf, 0xdd, 0xfb, 0xf7, 0x7b, 0xee, 0xf7,
   0xdf, 0xdd, 0xee, 0xff, 0xff, 0xfd, 0xf7, 0xbf, 0xbe, 0x75, 0xb7, 0xdf,
   0xed, 0xf7, 0xfb, 0xdd, 0xfb, 0xbb, 0xdd, 0xef, 0x66, 0xbb, 0xdd, 0x6e,
   0xb7, 0x7f, 0xef, 0x76, 0x7d, 0xeb, 0xee, 0x7b, 0xbf, 0x7f, 0xaf, 0xff,
   0xff, 0x7f, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xfd, 0xff, 0xbf, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x7e, 0xbf, 0xff, 0xbf, 0xfd, 0xff, 0xff, 0xfe, 0xff, 0x0f, 0x83, 0x7f,
   0x7d, 0xff, 0x6f, 0xff, 0xbf, 0xff, 0xff, 0xfb, 0xbe, 0xdd, 0xfe, 0xff,
   0xff, 0xff, 0xea, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xff, 0x7f, 0x7e, 0x1f,
   0xec, 0xde, 0xe7, 0x7f, 0x53, 0xf8, 0xff, 0x17, 0xfd, 0xdb, 0xf7, 0xf7,
   0xf9, 0xff, 0x8f, 0xfe, 0xff, 0xf7, 0xf6, 0xbf, 0xdd, 0x4e, 0xff, 0xd7,
   0xeb, 0xf5, 0x7a, 0xbd, 0xae, 0xf7, 0xfb, 0xfd, 0x7e, 0xff, 0x7e, 0xbf,
   0xdf, 0xbb, 0xd9, 0x6e, 0xb7, 0xdb, 0xed, 0x76, 0x9b, 0xdd, 0x6e, 0xb7,
   0xdb, 0x0d, 0xb7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xef, 0xf7, 0xfb, 0xfd,
   0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7,
   0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf,
   0xff, 0xfb, 0x7f, 0x1b, 0xaf, 0xcf, 0xff, 0xfb, 0xf7, 0xfa, 0xfe, 0xff,
   0xff, 0xef, 0xdd, 0xfb, 0xf7, 0xb9, 0x0e, 0x87, 0xdf, 0xe9, 0xee, 0x7c,
   0x3e, 0x0e, 0x8e, 0xdf, 0xa6, 0x75, 0xb7, 0xdf, 0xed, 0xf7, 0xfb, 0xdd,
   0xfb, 0xbb, 0xde, 0x4f, 0x56, 0xbb, 0xdd, 0x6e, 0x37, 0x7f, 0xef, 0x76,
   0x6d, 0xeb, 0xf5, 0x79, 0x7f, 0x7f, 0xaf, 0xff, 0xff, 0x71, 0x78, 0x38,
   0x1c, 0x07, 0x07, 0xe1, 0xf8, 0xb8, 0x7b, 0x0f, 0x86, 0xc7, 0xe1, 0xe1,
   0x72, 0x18, 0xec, 0x76, 0x6d, 0xdd, 0x6e, 0xf0, 0x7e, 0xbf, 0xc7, 0xdf,
   0xe0, 0xff, 0x7f, 0xfc, 0xff, 0xdf, 0xef, 0x7f, 0x78, 0xfe, 0x6d, 0xff,
   0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xe2, 0x70,
   0x9f, 0xfc, 0x07, 0xd7, 0xff, 0x73, 0x7f, 0xdf, 0x5d, 0xdf, 0x9b, 0x7f,
   0x5d, 0x7b, 0xfb, 0x57, 0xfd, 0xe7, 0xf7, 0xfb, 0xfb, 0xdf, 0x8d, 0xfe,
   0xff, 0xf7, 0x76, 0xbb, 0x5e, 0x9f, 0x9f, 0xd7, 0xeb, 0xf5, 0x7a, 0xbd,
   0xae, 0xf7, 0xfb, 0xfd, 0x7e, 0xff, 0x7e, 0xbf, 0xdf, 0xbb, 0xd5, 0x6e,
   0xb7, 0xdb, 0xed, 0xae, 0xab, 0xdd, 0x6e, 0xb7, 0xbb, 0xee, 0xd6, 0xc7,
   0xe3, 0xf1, 0x78, 0x3c, 0x4e, 0x0e, 0xc7, 0xe3, 0xf1, 0x38, 0x1e, 0x8f,
   0xc7, 0x9f, 0xe1, 0xf1, 0x78, 0x3c, 0x1e, 0xff, 0xc7, 0xdd, 0x6e, 0xb7,
   0xdb, 0x0d, 0x77, 0xff, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7,
   0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf,
   0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xff, 0xfb, 0xbf, 0x3d,
   0xbe, 0xcf, 0xff, 0xfb, 0xf7, 0x32, 0xf8, 0xff, 0xff, 0xef, 0xdd, 0xfb,
   0x7b, 0xbc, 0x7e, 0x76, 0xdf, 0xe3, 0xee, 0x7c, 0xde, 0xff, 0x7f, 0xef,
   0xaa, 0x6e, 0xb8, 0xdf, 0x0d, 0x06, 0x9b, 0xc1, 0xfb, 0x3b, 0xde, 0x4f,
   0x56, 0xbb, 0xcd, 0x6e, 0x78, 0x7c, 0xef, 0x26, 0x6d, 0xf7, 0xfb, 0x7d,
   0x7f, 0x7f, 0x77, 0xff, 0xff, 0x6f, 0x37, 0xdf, 0xed, 0xde, 0xbb, 0xdd,
   0xfb, 0xbb, 0x7d, 0xaf, 0x76, 0xbb, 0xdd, 0xee, 0xbc, 0x7f, 0xef, 0x76,
   0x55, 0xeb, 0xed, 0xfb, 0x7e, 0xbf, 0x1f, 0xef, 0xfd, 0xff, 0xff, 0xfe,
   0xff, 0xdf, 0xef, 0xff, 0xfc, 0xf8, 0x6e, 0xfc, 0xdf, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0x6e, 0xfb, 0xbf, 0xef,
   0xff, 0x73, 0x3f, 0xdc, 0x5d, 0xdf, 0xbb, 0x7f, 0x5d, 0xb8, 0x1d, 0x14,
   0xfd, 0xff, 0xc1, 0xf1, 0xf8, 0xdf, 0x8d, 0xce, 0xff, 0xf7, 0xf9, 0xb6,
   0x56, 0x80, 0x9d, 0xbb, 0xdd, 0x6e, 0xb7, 0xdb, 0x2d, 0xf6, 0x83, 0xc1,
   0x60, 0xf0, 0x7e, 0xbf, 0xdf, 0xa1, 0xd5, 0x6e, 0xb7, 0xdb, 0xed, 0xde,
   0xab, 0xdd, 0x6e, 0xb7, 0x7b, 0xef, 0xd6, 0xbf, 0xdf, 0xef, 0xf7, 0xfb,
   0xbd, 0xe5, 0xbb, 0xdd, 0x6e, 0xf7, 0x7e, 0xbf, 0xdf, 0x87, 0xdd, 0x6e,
   0xb7, 0xdb, 0xed, 0x06, 0x9b, 0xdd, 0x6e, 0xb7, 0xbb, 0xed, 0x6e, 0xff,
   0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf,
   0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e,
   0xbf, 0xdf, 0xef, 0xf7, 0xff, 0xfb, 0x1f, 0x70, 0xbc, 0x56, 0xff, 0xfb,
   0xf7, 0xff, 0xfe, 0x0f, 0xff, 0xef, 0xdd, 0xfb, 0xfd, 0xdb, 0xfe, 0x76,
   0xef, 0xdd, 0xe1, 0xff, 0x3f, 0x0e, 0x8e, 0xf7, 0xaa, 0x60, 0xb7, 0xdf,
   0xed, 0xf7, 0xbb, 0xdd, 0xfb, 0xbb, 0xdd, 0x4f, 0x56, 0xbb, 0xe1, 0x6e,
   0xfb, 0x79, 0xef, 0xae, 0x93, 0xeb, 0xfb, 0x7c, 0x7f, 0x7f, 0xff, 0xff,
   0xff, 0x61, 0xb7, 0xdf, 0x0d, 0xde, 0xbb, 0xdd, 0xfb, 0x3b, 0x7e, 0xaf,
   0x76, 0xbb, 0xdd, 0xee, 0x3e, 0x7e, 0xef, 0xae, 0x55, 0xf7, 0xf5, 0x3b,
   0x7f, 0x7f, 0xfe, 0xf7, 0xf0, 0xff, 0xff, 0xfe, 0xff, 0xdf, 0xef, 0x7f,
   0xe0, 0x73, 0x6f, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x07,
   0x01, 0xff, 0x7f, 0xfc, 0x6d, 0xf8, 0xbf, 0xef, 0xff, 0x7f, 0x7f, 0xdf,
   0x0d, 0xfe, 0x87, 0x7f, 0xdd, 0xdf, 0xfe, 0x55, 0xfd, 0xff, 0xf7, 0xff,
   0xff, 0xdf, 0x9d, 0xce, 0xff, 0xff, 0xff, 0xed, 0x72, 0xbb, 0xfc, 0x83,
   0xc1, 0x60, 0x30, 0x18, 0x8c, 0xf7, 0xfb, 0xfd, 0x7e, 0xff, 0x7e, 0xbf,
   0xdf, 0xbb, 0xd5, 0x6e, 0xb7, 0xdb, 0xed, 0xae, 0xab, 0xdd, 0x6e, 0xb7,
   0x7b, 0xef, 0xb6, 0x87, 0xc3, 0xe1, 0x70, 0x38, 0x0c, 0xf4, 0x83, 0xc1,
   0x60, 0xf0, 0x7e, 0xbf, 0xdf, 0xbb, 0xdd, 0x6e, 0xb7, 0xdb, 0xed, 0xfe,
   0xab, 0xdd, 0x6e, 0xb7, 0xbb, 0xee, 0xae, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xbf, 0x7d, 0x5d, 0x35, 0xff, 0xfb, 0xf7, 0xff, 0xfe, 0xff,
   0xff, 0xf7, 0xdd, 0xfb, 0xfe, 0x1b, 0xfc, 0x76, 0xef, 0xdd, 0xef, 0xff,
   0xff, 0xfd, 0xf7, 0xff, 0xd2, 0x6e, 0xb7, 0xdf, 0xed, 0xf7, 0xbb, 0xdd,
   0xfb, 0xbb, 0xdd, 0x8f, 0x36, 0xbb, 0xfd, 0x6e, 0xf3, 0x7b, 0xef, 0xae,
   0x93, 0xeb, 0xfb, 0x7e, 0xff, 0x7e, 0xff, 0xff, 0xff, 0x6e, 0xb7, 0xdf,
   0xed, 0xdf, 0xc7, 0xdd, 0xfb, 0xbb, 0x7d, 0xaf, 0x76, 0xbb, 0xdd, 0xee,
   0xfe, 0x79, 0xef, 0xae, 0x55, 0xf7, 0xf5, 0xfd, 0x7e, 0xbf, 0xff, 0xff,
   0xfd, 0xff, 0xff, 0xfe, 0xff, 0xdf, 0x83, 0x7f, 0xe9, 0xf7, 0x6e, 0xff,
   0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0x6e, 0xff, 0xdf, 0xef, 0xff, 0x7b, 0x7e, 0x1f, 0xbc, 0xff, 0x9f, 0x7f,
   0xd3, 0xbf, 0xfd, 0x55, 0xfd, 0xff, 0xf7, 0xff, 0xff, 0xdf, 0xbd, 0xfe,
   0xff, 0xff, 0xff, 0x76, 0xb5, 0x5b, 0xbd, 0xbb, 0xdd, 0x6e, 0xb7, 0xdb,
   0xad, 0xf7, 0xfb, 0xfd, 0x7e, 0xff, 0x7e, 0xbf, 0xdf, 0xbb, 0xcd, 0x6e,
   0xb7, 0xdb, 0xed, 0x76, 0xb3, 0xdd, 0x6e, 0xb7, 0x7b, 0x0f, 0x77, 0xbb,
   0xdd, 0x6e, 0xb7, 0xdb, 0xb5, 0xf7, 0xfb, 0xfd, 0x7e, 0xff, 0x7e, 0xbf,
   0xdf, 0xbb, 0xdd, 0x6e, 0xb7, 0xdb, 0xed, 0xde, 0xab, 0xdd, 0x6e, 0xb7,
   0xbb, 0xee, 0xae, 0xff, 0xf3, 0xf9, 0x7c, 0x3e, 0x9f, 0xcf, 0xe7, 0xf3,
   0xf9, 0x7c, 0x3e, 0x9f, 0xcf, 0xe7, 0xf3, 0xf9, 0x7c, 0x3e, 0x9f, 0xcf,
   0xe7, 0xf3, 0xf9, 0x7c, 0x3e, 0x9f, 0xcf, 0xe7, 0xff, 0xfb, 0xbf, 0x7d,
   0x5d, 0x35, 0xff, 0xfb, 0xf7, 0xff, 0x7f, 0xff, 0xcf, 0xf7, 0xc9, 0xfb,
   0xfe, 0xfb, 0x7e, 0x66, 0xef, 0xdd, 0xf7, 0xfc, 0xfe, 0xff, 0xff, 0xe7,
   0x7e, 0x4e, 0x77, 0xdf, 0xee, 0xf7, 0xb7, 0xdd, 0xfb, 0xbb, 0xdb, 0xaf,
   0x76, 0x93, 0xfd, 0x64, 0xf7, 0x7b, 0x4f, 0xae, 0xbb, 0xdd, 0x7b, 0x7e,
   0xff, 0x7e, 0xff, 0xff, 0xff, 0x6e, 0x37, 0xdf, 0xcd, 0xdf, 0xfb, 0xdd,
   0xfb, 0xbb, 0x7b, 0xaf, 0x76, 0xbb, 0xdd, 0xee, 0xfe, 0x7b, 0xef, 0xae,
   0x9b, 0xeb, 0xf3, 0xfe, 0x7e, 0xbf, 0xff, 0xe7, 0xfb, 0xff, 0xfd, 0xbe,
   0xad, 0xde, 0xef, 0x7f, 0xe9, 0xf7, 0x4d, 0xff, 0xe7, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x77, 0x6f, 0xff, 0xef, 0xef,
   0xff, 0xfb, 0x70, 0xff, 0x0f, 0xde, 0xbf, 0xff, 0xee, 0x7f, 0xfb, 0xef,
   0xfe, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xbd, 0xfe, 0xff, 0xff, 0x7f, 0x7b,
   0xa1, 0x5d, 0xd8, 0x39, 0x1c, 0x0e, 0x87, 0xc3, 0xb1, 0xef, 0xfb, 0xfd,
   0x7e, 0xff, 0x7e, 0xbf, 0xdf, 0xdb, 0xdd, 0x64, 0x32, 0x99, 0x4c, 0xfe,
   0x93, 0xc9, 0x64, 0x32, 0x79, 0xef, 0x77, 0xbb, 0xdd, 0x6e, 0xb7, 0xdb,
   0xb5, 0xe7, 0xf3, 0xf9, 0x7c, 0xfe, 0x7e, 0xbf, 0xdf, 0xbb, 0xdd, 0x6e,
   0xb7, 0xdb, 0xed, 0xfe, 0xb3, 0xdd, 0x6e, 0xb7, 0x7b, 0xee, 0x9e, 0xff,
   0xf3, 0xf9, 0x7c, 0x3e, 0x9f, 0xcf, 0xe7, 0xf3, 0xf9, 0x7c, 0x3e, 0x9f,
   0xcf, 0xe7, 0xf3, 0xf9, 0x7c, 0x3e, 0x9f, 0xcf, 0xe7, 0xf3, 0xf9, 0x7c,
   0x3e, 0x9f, 0xcf, 0xe7, 0xff, 0xf3, 0xbf, 0x1d, 0xee, 0x4e, 0xfe, 0xf3,
   0xf3, 0xff, 0x3f, 0xff, 0xcf, 0xfb, 0xe3, 0x7b, 0x30, 0xfc, 0x0e, 0x8f,
   0xf7, 0xe3, 0xf8, 0x7c, 0xfe, 0xff, 0xff, 0xe7, 0x7d, 0x5f, 0x78, 0x18,
   0x0f, 0xf6, 0x87, 0xdd, 0x60, 0xbc, 0x17, 0xac, 0x76, 0xc7, 0xfd, 0x71,
   0x2f, 0x7c, 0x1f, 0xdf, 0xbb, 0xbe, 0x7b, 0x70, 0xff, 0x7d, 0xff, 0xff,
   0xff, 0x61, 0x78, 0x38, 0x1c, 0xde, 0x87, 0xdd, 0xe0, 0xbb, 0x17, 0xac,
   0x76, 0xc7, 0xe1, 0xe1, 0x3e, 0x7c, 0x1c, 0xde, 0xbb, 0xdd, 0x7b, 0xf0,
   0x7e, 0xbf, 0xff, 0xe7, 0xc3, 0xff, 0xfc, 0x9e, 0xac, 0xde, 0xef, 0x7f,
   0x60, 0xf8, 0x1f, 0xfc, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x7f, 0xf8, 0x9f, 0xf8, 0x07, 0xef, 0xff, 0xfb, 0x3d, 0xf8,
   0xbf, 0xdf, 0xc3, 0xff, 0xf1, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xc1, 0xff,
   0xff, 0x1f, 0xbc, 0xfe, 0xff, 0xff, 0xff, 0xbf, 0xd7, 0xe8, 0xed, 0x7d,
   0x3e, 0x9f, 0xcf, 0xe7, 0x33, 0x0e, 0x83, 0xc1, 0x60, 0x30, 0x18, 0x0c,
   0x06, 0xe3, 0xdd, 0xf1, 0x78, 0x3c, 0x1e, 0xff, 0xc3, 0xe3, 0xf1, 0x78,
   0x7c, 0xef, 0x17, 0x87, 0xc3, 0xe1, 0x70, 0x38, 0x44, 0x0c, 0x87, 0xc3,
   0xe1, 0x30, 0x18, 0x0c, 0x06, 0xc7, 0xdd, 0xf1, 0x78, 0x3c, 0x1e, 0xff,
   0xc7, 0xc3, 0xe1, 0x70, 0x78, 0x0f, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xf7, 0xfb, 0xff, 0xbf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x7b, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xef,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0x7e, 0xbf, 0xff, 0xff,
   0xff, 0xff, 0xfe, 0xde, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xbf, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x7f, 0xef, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xe7, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0xfc, 0x1f, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7b, 0xff,
   0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xef, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xfb, 0xff, 0x79, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xfb, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xbf, 0xfe, 0xef, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xef, 0xdf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0x7f, 0xfc, 0xff, 0xff,
   0xff, 0xff, 0xfd, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff,
   0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xdf, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x9f, 0xef, 0xe7, 0xff
};
