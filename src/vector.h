#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct vec2_t {
	float x;
	float y;
} vec2_t;

typedef struct vec3_t {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct vec4_t {
	float x, y, z, w;
} vec4_t;

typedef struct mat4_t {
	float m[4][4];
} mat4_t;

vec2_t vec2_from_vec3(const vec3_t* v);
vec2_t vec2_from_vec4(const vec4_t* v);
bool vec2_eq(vec2_t a, vec2_t b);
float vec2_magnitude(vec2_t v);
float vec2_gradient(vec2_t a, vec2_t b);
float vec2_inv_gradient(vec2_t a, vec2_t b);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_scale(vec2_t v, float s);
vec2_t vec2_sdiv(vec2_t v, float s);
float vec2_dot(vec2_t a, vec2_t b);
vec2_t vec2_translate(vec2_t v, int dx, int dy);
vec2_t vec2_normalize(vec2_t v);
bool vec2_less_y(const void* a, const void* b);

vec3_t vec3_from_vec4(const vec4_t* v);
float vec3_magnitude(const vec3_t* v);
vec3_t vec3_add(const vec3_t* a, const vec3_t* b);
vec3_t vec3_sub(const vec3_t* a, const vec3_t* b);
vec3_t vec3_sprod(const vec3_t* v, float s);
vec3_t vec3_sdiv(const vec3_t* v, float s);
vec3_t vec3_cross(const vec3_t* a, const vec3_t* b);
float vec3_dot(const vec3_t* a, const vec3_t* b);
vec3_t vec3_translate(const vec3_t* v, int dx, int dy, int dz);
vec3_t vec3_rotate_x(const vec3_t* v, float angle);
vec3_t vec3_rotate_y(const vec3_t* v, float angle);
vec3_t vec3_rotate_z(const vec3_t* v, float angle);
vec3_t vec3_normalize(const vec3_t* v);
vec3_t vec3_transform(const vec3_t* v, const mat4_t* m);

vec4_t vec4_from_vec3(const vec3_t* v);

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float a);
mat4_t mat4_make_rotation_y(float a);
mat4_t mat4_make_rotation_z(float a);
vec4_t mat4_mul_vec4(const mat4_t* m, const vec4_t* v);
mat4_t mat4_mul(const mat4_t* a, const mat4_t* b);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
vec4_t mat4_project_vec3(const mat4_t* m, const vec3_t* v);

#endif