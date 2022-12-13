#include "vector.h"

vec2_t vec2_from_vec3(const vec3_t* v) {
	vec2_t result = { v->x, v->y };
	return result;
}

float vec2_magnitude(vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

float vec2_gradient(vec2_t a, vec2_t b) {
	return (b.y - a.y) / (b.x - a.x);
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


float vec3_magnitude(const vec3_t* v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

vec3_t vec3_from_vec4(const vec4_t* v) {
	vec3_t result = { v->x, v->y, v->z };
	if (v->w != 0.0) {
		result.x /= v->w;
		result.y /= v->w;
		result.z /= v->w;
	}
	return result;
}

vec3_t vec3_translate(const vec3_t* v, int dx, int dy, int dz) {
	return (vec3_t) {
		.x = v->x + dx,
		.y = v->y + dy,
		.z = v->z + dz,
	};
}

vec3_t vec3_rotate_x(const vec3_t* v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	return (vec3_t) {
		.x = v->x,
		.y = v->y * cos_a - v->z * sin_a,
		.z = v->y * sin_a + v->z * cos_a
	};
}

vec3_t vec3_rotate_y(const vec3_t* v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	return (vec3_t) {
		.x = v->x * cos_a - v->z * sin_a,
		.y = v->y,
		.z = v->x * sin_a + v->z * cos_a
	};
}

vec3_t vec3_rotate_z(const vec3_t* v, float angle) {
	float sin_a = sin(angle);
	float cos_a = cos(angle);
	return (vec3_t) {
		.x = v->x * cos_a - v->y * sin_a,
		.y = v->x * sin_a + v->y * cos_a,
		.z = v->z
	};
}

vec3_t vec3_add(const vec3_t* a, const vec3_t* b) {
	return (vec3_t) {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z
	};
}

vec3_t vec3_sub(const vec3_t* a, const vec3_t* b) {
	return (vec3_t) {
		.x = a->x - b->x,
		.y = a->y - b->y,
		.z = a->z - b->z
	};
}

vec3_t vec3_sprod(const vec3_t* v, float s) {
	return (vec3_t) {
		.x = v->x * s,
		.y = v->y * s,
		.z = v->z * s
	};
}

vec3_t vec3_sdiv(const vec3_t* v, float s) {
	return (vec3_t) {
		.x = v->x / s,
		.y = v->y / s,
		.z = v->z / s
	};
}

vec3_t vec3_cross(const vec3_t* a, const vec3_t* b) {
	return (vec3_t) {
		.x = a->y * b->z - a->z * b->y,
		.y = a->z * b->x - a->x * b->z,
		.z = a->x * b->y - a->y * b->x
	};
}

float vec3_dot(const vec3_t* a, const vec3_t* b) {
	float prod = 0;
	prod += a->x * b->x;
	prod += a->y * b->y;
	prod += a->z * b->z;
	return prod;
}

vec3_t vec3_normalize(const vec3_t* v) {
	return vec3_sdiv(v, vec3_magnitude(v));
}

vec3_t vec3_transform(const vec3_t* v, const mat4_t* m) {
	vec4_t homogeneous = vec4_from_vec3(v);
	vec4_t transformed = mat4_mul_vec4(m, &homogeneous);
	return vec3_from_vec4(&transformed);
}

vec4_t vec4_from_vec3(const vec3_t* v) {
	return (vec4_t) { v->x, v->y, v->z, 1.0 };
}

mat4_t mat4_identity(void) {
	mat4_t m = { {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	} };
	return m;
}

/*
| sx  0  0  0 |
|  0 sy  0  0 |
|  0  0 sz  0 |
|  0  0  0  0 |
*/
mat4_t mat4_make_scale(float sx, float sy, float sz) {
	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	return m;
}

/*
|  1  0  0 tx |
|  0  1  0 ty |
|  0  0  1 tz |
|  0  0  0  1 |
*/
mat4_t mat4_make_translation(float tx, float ty, float tz) {
	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;
	return m;
}

/*
|  1       0       0  0  |
|  0  cos(a) -sin(a)  0  |
|  0  sin(a)  cos(a)  0  |
|  0       0       0  1  |
*/
mat4_t mat4_make_rotation_x(float a) {
	float cosa = cos(a);
	float sina = sin(a);

	mat4_t m = mat4_identity();
	m.m[1][1] = cosa;
	m.m[1][2] = -sina;
	m.m[2][1] = sina;
	m.m[2][2] = cosa;
	return m;
}

/*
|   cos(a)  0  sin(a)  0  |
|        0  1       0  0  |
|  -sin(a)  0  cos(a)  0  |
|        0  0       0  1  |
*/
mat4_t mat4_make_rotation_y(float a) {
	float cosa = cos(a);
	float sina = sin(a);

	mat4_t m = mat4_identity();
	m.m[0][0] = cosa;
	m.m[0][2] = sina;
	m.m[2][0] = -sina;
	m.m[2][2] = cosa;
	return m;
}

/*
|  cos(a) -sin(a)  0  0  |
|  sin(a)  cos(a)  1  0  |
|       0       0  1  0  |
|       0       0  0  1  |
*/
mat4_t mat4_make_rotation_z(float a) {
	float cosa = cos(a);
	float sina = sin(a);

	mat4_t m = mat4_identity();
	m.m[0][0] = cosa;
	m.m[0][1] = -sina;
	m.m[1][0] = sina;
	m.m[1][1] = cosa;
	return m;
}

vec4_t mat4_mul_vec4(const mat4_t* m, const vec4_t* v) {
	return (vec4_t) {
		.x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w,
		.y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w,
		.z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w,
		.w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w
	};
}

mat4_t mat4_mul(const mat4_t* a, const mat4_t* b) {
	mat4_t m = { {0} };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				m.m[i][k] += a->m[i][j] * b->m[j][k];
			}
		}
	}
	return m;
}

/*
|  (h/w)cot(fov/2))            0                          0  0  |
|                 0  cot(fov/2)                          0  0  |
|                 0            0  zf/(zf-zn)  -(zf*zn)/(zf-zn)  |
|                 0            0           1                 0  |
*/
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
	mat4_t m = { {0} };
	float cot_half_fov = 1 / tan(fov / 2);
	float dz = zfar - znear;
	m.m[0][0] = aspect * cot_half_fov;
	m.m[1][1] = cot_half_fov;
	m.m[2][2] = zfar / dz;
	m.m[2][3] = (-zfar * znear) / dz;
	m.m[3][2] = 1;
	return m;
}

vec2_t mat4_project_vec3(const mat4_t* m, const vec3_t* v) {
	vec4_t homogeneous = vec4_from_vec3(v);
	vec4_t projected = mat4_mul_vec4(m, &homogeneous);
	vec3_t reduced = vec3_from_vec4(&projected);
	return vec2_from_vec3(&reduced);
}