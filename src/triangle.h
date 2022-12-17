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
	// Depsite being 2D, the triangle's vertices store depth information for perspective-correct
	// texture mapping.
	vec4_t vertices[3];
	tex2_t tex_coords[3];
	color_t fill;
	color_t border;
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
const vec4_t* triangle_vertex_a(const triangle_t* t);
const vec4_t* triangle_vertex_b(const triangle_t* t);
const vec4_t* triangle_vertex_c(const triangle_t* t);

// Getters for named texture coordinates.
tex2_t triangle_tex_a(const triangle_t* t);
tex2_t triangle_tex_b(const triangle_t* t);
tex2_t triangle_tex_c(const triangle_t* t);

// Translates and scales the triangle to its final position on-screen.
void triangle_position_on_screen(triangle_t *t, int window_width, int window_height);

// comparator used for sorting triangles by their average depth.
bool triangle_less_depth(const void* a, const void* b);

// Returns true if the triangle's dimensions make it possible to render.
bool triangle_is_renderable(const triangle_t* t);

// Sorts the triangle's vertices by their y-coordinates in-place.
void triangle_sort_vertices_by_y(triangle_t* t);

// Truncates floating point values in the triangle's position vectors.
void triangle_truncate_xy_components(triangle_t* t);

/*
triangle_b_hyp_intercept returns the vector at which a horizontal line projected from
the triangle's middle vertex (by y-value), b, will intercept the hypotenuse, ac, producing two
triangles with a flat bottom and flat top, respectively. The input triangle's vertices must be
sorted by y-value.

                /|
               / |
              /  |
             ----|  <- intercept
             \_  |
               \ |
                \|
*/
vec2_t triangle_b_hyp_intercept(const triangle_t* t);

// Returns the barycentric weights alpha, beta and gamma as a vec3, which can be used to map p to
// its UV coordinates.
vec3_t triangle_barycentric_weights(const triangle_t* t, vec2_t p);

#endif