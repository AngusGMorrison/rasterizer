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

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 faces, 2 triangles per face

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

typedef struct mesh_t {
	vec3_t* vertices; // dynamic array
	mesh_face_t* faces; // dynamic array
	vec3_t rotation;
	vec3_t scale;
	vec3_t translation;
} mesh_t;

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern mesh_face_t cube_faces[N_CUBE_FACES];
extern mesh_t mesh;

mesh_face_t new_mesh_face();
vec3_t mesh_face_vertex_a(mesh_face_t mf);
vec3_t mesh_face_vertex_b(mesh_face_t mf);
vec3_t mesh_face_vertex_c(mesh_face_t mf);

void load_cube(void);
int load_mesh(char* path);
mat4_t mesh_to_world_matrix(mesh_t* mesh);



#endif