#include "color.h"

const color_t DEFAULT_BORDER_COLOR = BLACK;
const color_t DEFAULT_FILL_COLOR = WHITE;
const color_t DEFAULT_WIREFRAME_COLOR = GREEN;
const color_t DEFAULT_VERTEX_COLOR = RED;

color_t color_adjust_intensity(color_t original, float intensity) {
	// Modify each byte in isolation.
	color_t a = original & MAX_ALPHA;
	color_t r = (original & 0x00FF0000) * intensity;
	color_t g = (original & 0x0000FF00) * intensity;
	color_t b = (original & 0x000000FF) * intensity;
	// Combine into a new color.
	color_t new = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
	return new;
}