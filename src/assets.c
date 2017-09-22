#if 0 /* Disable them for now, but that's how we would implement them */

typedef struct {
    u16 w, h;
    Rgb24 pixels[];
} Rgb24Image;

typedef struct {
    u16 w, h;
    Rgba32 pixels[];
} Rgba32Image;

extern const Rgb24Image *const asset_foo;
extern const Rgb24Image *const asset_bar;
extern const Rgb24Image *const asset_baz;
extern const Rgb24Image *const asset_qux;
#endif
