// color.h provides color primitives and helper functions.
#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

// color_t represents an ARGB color in 4 bytes.
typedef uint32_t color_t;

#define BLACK 0xFF000000
#define BLUE 0xFF0000FF
#define CYAN 0xFF00FFFF
#define GREEN 0xFF00FF00
#define MAGENTA 0xFFFF00FF
#define MAX_ALPHA 0xFF000000
#define RED 0xFFFF0000
#define SILVER 0xFFC0C0C0
#define WHITE 0xFFFFFFFF
#define YELLOW 0xFFFFFF00

extern const color_t DEFAULT_BORDER_COLOR;
extern const color_t DEFAULT_FILL_COLOR;
extern const color_t DEFAULT_WIREFRAME_COLOR;

// color_adjust_intensity returns a new color based on multiplying each of the ARGB fields of the
// input color by the given factor. intensity must be >= 0.
color_t color_adjust_intensity(color_t original, float intensity);

#endif