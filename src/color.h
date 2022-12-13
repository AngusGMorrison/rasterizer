#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef uint32_t color_t;

#define BLACK 0xFF000000
#define BLUE 0xFF0000FF
#define CYAN 0xFF00FFFF
#define GREEN 0xFF00FF00
#define MAGENTA 0xFFFF00FF
#define MAX_ALPHA 0xFF000000
#define SILVER 0xFFC0C0C0
#define RED 0xFFFF0000
#define WHITE 0xFFFFFFFF
#define YELLOW 0xFFFFFF00

color_t color_adjust_intensity(color_t original, float intensity);

#endif