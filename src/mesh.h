// mesh.h provides functions to read a 3D object from an .obj file and the data structures to
// represent it.
#ifndef MESH_H
#define MESH_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "color.h"
#include "must.h"
#include "texture.h"
#include "vector.h"

/*
Constants
*/

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 faces, 2 triangles per face


/*
Structs
*/

// mesh_face_t stores the indices of 3 vectors in 3D space that represent the vertices a triangular
// face.
typedef struct mesh_face_t {
	int a;
	int b;
	int c;
	vec3_t* mesh_vertices;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	color_t color;
} mesh_face_t;

// mesh_t represents a whole 3D object and its position in space.
typedef struct mesh_t {
	vec3_t* vertices; // dynamic array
	mesh_face_t* faces; // dynamic array
	vec3_t rotation;
	vec3_t scale;
	vec3_t translation;
} mesh_t;

// Global mesh.
extern mesh_t g_mesh;

/*
Functions
*/

// Getters to expedite vertex lookup.
vec3_t mesh_face_vertex_a(const mesh_face_t* mf);
vec3_t mesh_face_vertex_b(const mesh_face_t* mf);
vec3_t mesh_face_vertex_c(const mesh_face_t* mf);

// Load a cube from hard-coded vertices and texture data.
void load_cube(void);

// Load a mesh from the given .obj file.
int load_mesh(const char* path);

// Returns a matrix representing the current scale, position and orientation of the mesh in space,
// which can be used to transform each face ready for projection onto the viewing plane.
mat4_t mesh_to_world_matrix(const mesh_t* mesh);



#endif