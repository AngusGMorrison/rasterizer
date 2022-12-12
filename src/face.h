#ifndef FACE_H
#define FACE_H

#include <stdbool.h>

#include "color.h"
#include "light.h"
#include "vector.h"

// face_t stores the indices of 3 vectors in 3D space that represent the vertices a triangular face.
typedef struct face_t {
	int a;
	int b;
	int c;
	bool has_normal;
	vec3_t normal;
	color_t color;
} face_t;

face_t new_face();
vec3_t face_normal(vec3_t* vertices);
color_t face_illuminate(vec3_t* vertices, light_t l, color_t color);

#endif