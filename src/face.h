// face.h provides a structure and functions for working with planes in 3D space.
#ifndef FACE_H
#define FACE_H

#include <stdbool.h>

#include "color.h"
#include "light.h"
#include "mesh.h"
#include "texture.h"
#include "vector.h"

/*
Structs
*/

// face_t represents a triangular plane in 3D space.
typedef struct face_t {
	vec3_t vertices[3];
	tex2_t tex_coords[3];
	color_t color;
} face_t;


/*
Functions
*/

// Construct a new face from its mesh counterpart.
face_t new_face_from_mesh_face(const mesh_face_t* mf);

// Getters to expedite vertex lookup.
vec3_t face_vertex_a(const face_t* f);
vec3_t face_vertex_b(const face_t* f);
vec3_t face_vertex_c(const face_t* f);

// Getters to expedite tex lookup.
vec3_t face_tex_a(const face_t* f);
vec3_t face_tex_b(const face_t* f);
vec3_t face_tex_c(const face_t* f);

// Transform the face by the given matrix.
void face_transform(face_t* f, const mat4_t* m);

// Returns true if the face is out of view and should be culled.
bool face_should_cull(const face_t* f, vec3_t camera_pos);

// Returns the normal to the face.
vec3_t face_normal(const face_t* f);

// Updates the face's color based on its illumination by the given light sources.
void face_illuminate(face_t* f, const light_t* l);

// Returns the average of the z-components of the face's vertices.
float face_avg_depth(const face_t* face);

#endif