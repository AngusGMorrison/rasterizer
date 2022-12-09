// mesh_t is defined in its own file to avoid import conflicts between mesh.c and obj_parser.c.
#ifndef MESH_T_H
#define MESH_T_H

#include "vector.h"
#include "triangle.h"

// mesh_t represents a dynamically sized mesh.
typedef struct mesh_t {
	vec3_t* vertices; // dynamic array
	face_t* faces; // dynamic array
	vec3_t rotation;
} mesh_t;

#endif