#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "sort.h"
#include "vector.h"

// face_t stores the indices of 3 vectors in 3D space that represent the vertices a triangular face.
typedef struct face_t {
	int a;
	int b;
	int c;
} face_t;

// triangle_t stores 3 vectors in 2D space that represent the vertices of a projected triangular
// face.
typedef struct triangle_t {
	vec2_t points[3];
} triangle_t;

void triangle_sort_vertices_y(triangle_t* t);

#endif