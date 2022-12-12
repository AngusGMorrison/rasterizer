#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

#include "math.h"
#include "vector.h"

typedef struct mat4_t {
	float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float a);
mat4_t mat4_make_rotation_y(float a);
mat4_t mat4_make_rotation_z(float a);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul(mat4_t a, mat4_t b);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
vec3_t mat4_project_vec3(mat4_t m, vec3_t v);

#endif