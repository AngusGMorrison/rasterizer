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

vec2_t vec2_translate(vec2_t point, int dx, int dy);
vec2_t vec3_project(vec3_t point);
vec3_t vec3_translate(vec3_t point, int dx, int dy, int dz);
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
bool vec3_is_origin(vec3_t v);

#endif