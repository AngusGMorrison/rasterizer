#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdbool.h>

#include "color.h"
#include "face.h"
#include "sort.h"
#include "texture.h"
#include "vector.h"

/*
Structs
*/

// triangle_t stores three vectors in 2D space that represent the vertices of a projected triangular
// face.
typedef struct triangle_t {
	vec2_t vertices[3];
	tex2_t tex_coords[3];
	color_t fill;
	float avg_depth;
} triangle_t;

/*
Functions
*/

// Construct a triangle with all fields initialized to sensible defaults.
triangle_t new_triangle();

// Construct a triangle by projecting a 3D face into 2D space. Preserves the color of the face and
// calculates avg_depth from the face's vertices.
triangle_t new_triangle_from_face(const face_t* f, const mat4_t* projection);

// Getters for named vertices.
vec2_t triangle_vertex_a(const triangle_t* t);
vec2_t triangle_vertex_b(const triangle_t* t);
vec2_t triangle_vertex_c(const triangle_t* t);

// Getters for named texture coordinates.
tex2_t triangle_tex_a(const triangle_t* t);
tex2_t triangle_tex_b(const triangle_t* t);
tex2_t triangle_tex_c(const triangle_t* t);

// Translates and scales the triangle to its final position on-screen.
void triangle_position_on_screen(triangle_t *t, int window_width, int window_height);

// comparator used for sorting triangles by their average depth.
bool triangle_less_depth(const void* a, const void* b);

// void triangle_set_tex_coords_from_face(triangle_t* t, mesh_face_t f);

#endif