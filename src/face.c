#include "face.h"

face_t new_face_from_mesh_face(mesh_face_t mf) {
	face_t f = {
		.a = mesh_face_vertex_a(mf),
        .b = mesh_face_vertex_b(mf),
        .c = mesh_face_vertex_c(mf),
        .color = mf.color,
	};

	// y-values must be inverted to account for the fact that models have their
	// greatest value of y at the top, but our color buffer has its greatest value
	// of y at the bottom.
	f.a.y *= -1;
	f.b.y *= -1;
	f.c.y *= -1;

	return f;
}

void face_transform(face_t* f, mat4_t m) {
	f->a = vec3_transform(f->a, m);
	f->b = vec3_transform(f->b, m);
	f->c = vec3_transform(f->c, m);
}

bool face_should_cull(face_t f, vec3_t camera_pos) {
	vec3_t n = face_normal(f);
	vec3_t acam = vec3_sub(camera_pos, f.a);

	if (vec3_dot(n, acam) < 0) {
		return true;
	}
	return false;
}

vec3_t face_normal(face_t f) {
	vec3_t ab = vec3_sub(f.b, f.a);
	vec3_t ac = vec3_sub(f.c, f.a);
	vec3_t cross = vec3_cross(ac, ab);
	return vec3_normalize(cross);
}

void face_illuminate(face_t* f, light_t l) {
	vec3_t n = face_normal(*f);
	vec3_t al = vec3_sub(l.direction, f->a);
	al = vec3_normalize(al);

	// Faces that are aligned with or face away from the light should not be illuminated.
	float intensity = vec3_dot(n, al);
	if (intensity <= 0.0) { 
		intensity = 0.0;
	}

	f->color = color_adjust_intensity(f->color, intensity);
}

float face_avg_depth(face_t f) {
	return (f.a.z + f.b.z + f.c.z) / 3;
}
