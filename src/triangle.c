#include "triangle.h"

// triangle_sort_vertices_y sorts the triangle's vertices by their y-components.
void triangle_sort_vertices_y(triangle_t* t) {
	insertion_sort(t->points, 3, sizeof(vec2_t), &vec2_less_y);
}