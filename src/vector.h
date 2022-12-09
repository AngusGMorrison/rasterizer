#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdbool.h>
typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

float vec2_magnitude(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_scale(vec2_t v, float s);
vec2_t vec2_sdiv(vec2_t v, float s);
float vec2_dot(vec2_t a, vec2_t b);
vec2_t vec2_translate(vec2_t v, int dx, int dy);
vec2_t vec2_normalize(vec2_t v);

float vec3_magnitude(vec3_t v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_sprod(vec3_t v, float s);
vec3_t vec3_sdiv(vec3_t v, float s);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);
vec2_t vec3_project(vec3_t v);
vec3_t vec3_translate(vec3_t v, int dx, int dy, int dz);
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
vec3_t vec3_normalize(vec3_t v);

#endif