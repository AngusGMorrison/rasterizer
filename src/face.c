#include "face.h"

face_t new_face_from_mesh_face(const mesh_face_t* mf) {
	face_t f = {
		.vertices = {
			mesh_face_vertex_a(mf),
			mesh_face_vertex_b(mf),
			mesh_face_vertex_c(mf)
		},
		.tex_coords = {
			mesh_face_tex_a(mf),
			mesh_face_tex_b(mf),
			mesh_face_tex_c(mf)
		},
        .color = mf->color,
	};

	// y-values must be inverted to account for the fact that models have their
	// greatest value of y at the top, but our color buffer has its greatest value
	// of y at the bottom.
	for (int i = 0; i < 3; i++) {
		f.vertices[i].y *= -1;
		f.tex_coords[i].v = 1 - f.tex_coords[i].v;
	}

	return f;
}

vec3_t face_vertex_a(const face_t* f) {
	return f->vertices[0];
}
vec3_t face_vertex_b(const face_t* f) {
	return f->vertices[1];
}
vec3_t face_vertex_c(const face_t* f) {
	return f->vertices[2];
}

// tex2_t face_tex_a(const face_t* f) {
// 	return f->tex_coords[0];
// }
// tex2_t face_tex_b(const face_t* f) {
// 	return f->tex_coords[1];
// }
// tex2_t face_tex_c(const face_t* f) {
// 	return f->tex_coords[2];
// }

void face_transform(face_t* f, const mat4_t* m) {
	f->vertices[0] = vec3_transform(&f->vertices[0], m);
	f->vertices[1] = vec3_transform(&f->vertices[1], m);
	f->vertices[2] = vec3_transform(&f->vertices[2], m);
}

bool face_should_cull(const face_t* f, vec3_t camera_pos) {
	vec3_t n = face_normal(f);
	vec3_t a = face_vertex_a(f);
	vec3_t acam = vec3_sub(&camera_pos, &a);

	if (vec3_dot(&n, &acam) < 0) {
		return true;
	}
	return false;
}

vec3_t face_normal(const face_t* f) {
	vec3_t a = face_vertex_a(f);
	vec3_t b = face_vertex_b(f);
	vec3_t c = face_vertex_c(f);
	vec3_t ab = vec3_sub(&b, &a);
	vec3_t ac = vec3_sub(&c, &a);
	vec3_t cross = vec3_cross(&ac, &ab);
	return vec3_normalize(&cross);
}

void face_illuminate(face_t* f, const light_t* l) {
	vec3_t n = face_normal(f);
	vec3_t a = face_vertex_a(f);
	vec3_t al = vec3_sub(&l->direction, &a);
	al = vec3_normalize(&al);

	// Faces that are aligned with or face away from the light should not be illuminated.
	float intensity = vec3_dot(&n, &al);
	if (intensity <= 0.0) { 
		intensity = 0.0;
	}

	f->color = color_adjust_intensity(f->color, intensity);
}

float face_avg_depth(const face_t* f) {
	vec3_t a = face_vertex_a(f);
	vec3_t b = face_vertex_b(f);
	vec3_t c = face_vertex_c(f);

	return (a.z + b.z + c.z) / 3;
}
