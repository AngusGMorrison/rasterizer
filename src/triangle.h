#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdbool.h>

#include "color.h"
#include "face.h"
#include "sort.h"
#include "texture.h"
#include "vector.h"

// triangle_t stores 3 vectors in 2D space that represent the vertices of a projected triangular
// face.
typedef struct triangle_t {
	vec2_t points[3];
	tex2_t tex_coords[3];
	color_t color;
	float avg_depth;
} triangle_t;

triangle_t new_triangle();
triangle_t new_triangle_from_face(face_t f, mat4_t projection);
void triangle_position_on_screen(triangle_t *t, int window_width, int window_height);
void triangle_sort_vertices_y(triangle_t* t);
bool triangle_less_depth(const void* a, const void* b);
// void triangle_set_tex_coords_from_face(triangle_t* t, mesh_face_t f);

#endif