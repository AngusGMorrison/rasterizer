#include "color.h"

color_t color_adjust_intensity(color_t original, float intensity) {
	color_t a = original & MAX_ALPHA;
	color_t r = (original & 0x00FF0000) * intensity;
	color_t g = (original & 0x0000FF00) * intensity;
	color_t b = (original & 0x000000FF) * intensity;
	color_t new = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
	return new;
}