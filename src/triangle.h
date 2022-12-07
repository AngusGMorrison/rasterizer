#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

// face_t stores the indices of 3 vectors in 3D space that represent the vertices a triangular face.
typedef struct {
	int a;
	int b;
	int c;
} face_t;

// triangle_t stores 3 vectors in 2D space that represent the vertices of a projected triangular
// face.
typedef struct {
	vec2_t points[3];
} triangle_t;

#endif