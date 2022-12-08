#ifndef MESH_H
#define MESH_H

#include <stdio.h>

#include "array.h"
#include "mesh_t.h"
#include "obj_parser.h"
#include "triangle.h"
#include "vector.h"

#define N_CUBE_VERTICES 8
extern vec3_t cube_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES (6 * 2) // 6 faces, 2 triangles per face
extern face_t cube_faces[N_CUBE_FACES];

extern mesh_t mesh;

void load_cube(void);
int load_mesh(char* path);

#endif