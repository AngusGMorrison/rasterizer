#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdbool.h>

#include "color.h"
#include "sort.h"
#include "vector.h"

// triangle_t stores 3 vectors in 2D space that represent the vertices of a projected triangular
// face.
typedef struct triangle_t {
	vec2_t points[3];
	color_t color;
	float avg_depth;
} triangle_t;

void triangle_sort_vertices_y(triangle_t* t);
bool triangle_less_depth(const void* a, const void* b);

#endif