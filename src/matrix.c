#include "matrix.h"

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

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
	vec4_t result;
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
	return result;
}

mat4_t mat4_mul(mat4_t a, mat4_t b) {
	mat4_t m = { {0} };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				m.m[i][k] += a.m[i][j] * b.m[j][k];
			}
		}
	}
	return m;
}

/*
|  (h/w)cot(fov/2))           0                          0  0  |
|                 0  cot(fov/2)                          0  0  |
|                 0           0  zf/(zf-zn)  -(zf*zn)/(zf-zn)  |
|                 0           0           1                 0  |
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

vec3_t mat4_project_vec3(mat4_t m, vec3_t v) {
	vec4_t homogeneous = vec4_from_vec3(v);
	vec4_t projected = mat4_mul_vec4(m, homogeneous);
	return vec3_from_vec4(projected);
}