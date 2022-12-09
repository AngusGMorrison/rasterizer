#include "vector.h"

const float fov_factor = 640;

float vec2_magnitude(vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

float vec2_gradient(vec2_t a, vec2_t b) {
	return (b.y - a.y) / (b.x - a.x);
}

float vec2_inv_gradient(vec2_t a, vec2_t b) {
	return (b.x - a.x) / (b.y - a.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

vec2_t vec2_scale(vec2_t v, float s) {
	v.x *= s;
	v.y *= s;
	return v;
}

vec2_t vec2_sdiv(vec2_t v, float s) {
	v.x /= s;
	v.y /= s;
	return v;
}

float vec2_dot(vec2_t a, vec2_t b) {
	float prod = 0;
	prod += a.x * b.x;
	prod += a.y * b.y;
	return prod;
}

vec2_t vec2_translate(vec2_t point, int dx, int dy) {
	point.x += dx;
	point.y += dy;
	return point;
}

vec2_t vec2_normalize(vec2_t v) {
	return vec2_sdiv(v, vec2_magnitude(v));
}

// vec2_less_y is a comparator returning true if the y-component of a is less than that of b.
bool vec2_less_y(const void* a, const void* b) {
	vec2_t* vec_a = (vec2_t*)a;
	vec2_t* vec_b = (vec2_t*)b;

	return vec_a->y < vec_b->y;
}


float vec3_magnitude(vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// vec3_project receives a 3D vector and returns projected 2D point.
vec2_t vec3_project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

vec3_t vec3_translate(vec3_t point, int dx, int dy, int dz) {
	point.x += dx;
	point.y += dy;
	point.z += dz;
	return point;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	vec3_t rotated = {
		.x = v.x,
		.y = v.y * cos_a - v.z * sin_a,
		.z = v.y * sin_a + v.z * cos_a
	};
	return rotated;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	vec3_t rotated = {
		.x = v.x * cos_a - v.z * sin_a,
		.y = v.y,
		.z = v.x * sin_a + v.z * cos_a
	};
	return rotated;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	vec3_t rotated = {
		.x = v.x * cos_a - v.y * sin_a,
		.y = v.x * sin_a + v.y * cos_a,
		.z = v.z
	};
	return rotated;
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

vec3_t vec3_sprod(vec3_t v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

vec3_t vec3_sdiv(vec3_t v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
	vec3_t prod = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return prod;
}

float vec3_dot(vec3_t a, vec3_t b) {
	float prod = 0;
	prod += a.x * b.x;
	prod += a.y * b.y;
	prod += a.z * b.z;
	return prod;
}

vec3_t vec3_normalize(vec3_t v) {
	return vec3_sdiv(v, vec3_magnitude(v));
}