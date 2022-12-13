#ifndef FACE_H
#define FACE_H

#include <stdbool.h>

#include "color.h"
#include "light.h"
#include "mesh.h"
#include "texture.h"
#include "vector.h"

typedef struct face_t {
	vec3_t a, b, c;
	color_t color;
} face_t;

face_t new_face_from_mesh_face(mesh_face_t mf);
void face_transform(face_t* f, mat4_t m);
bool face_should_cull(face_t f, vec3_t camera_pos);
vec3_t face_normal(face_t f);
void face_illuminate(face_t* f, light_t l);
float face_avg_depth(face_t face);

#endif