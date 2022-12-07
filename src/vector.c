#include <math.h>

#include "vector.h"

const float fov_factor = 640;

// project receives a 3D vector and returns projected 2D point.
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
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
