#include "triangle.h"

triangle_t new_triangle() {
	triangle_t t = {
		.vertices = {0},
		.tex_coords = {0},
		.fill = DEFAULT_FILL_COLOR,
		.avg_depth = 0,
	};
	return t;
}

triangle_t new_triangle_from_face(face_t f, mat4_t projection) {
	triangle_t t = new_triangle();
	t.fill = f.color;
	t.avg_depth = face_avg_depth(f);
	t.vertices[0] = mat4_project_vec3(projection, f.a);
	t.vertices[1] = mat4_project_vec3(projection, f.b);
	t.vertices[2] = mat4_project_vec3(projection, f.c);
	return t;
}

vec2_t triangle_vertex_a(triangle_t t) {
	return t.vertices[0];
}

vec2_t triangle_vertex_b(triangle_t t) {
	return t.vertices[1];
}

vec2_t triangle_vertex_c(triangle_t t) {
	return t.vertices[2];
}

void triangle_scale_to_screen(triangle_t *t,  int window_width, int window_height) {
	for (int i = 0; i < 3; i++) {
		t->vertices[i].x *= window_width / 2.0;
		t->vertices[i].y *= window_height / 2.0;
	}
}

void triangle_translate_to_center(triangle_t* t, int window_width, int window_height) {
	for (int i = 0; i < 3; i++) {
		t->vertices[i].x += window_width / 2.0;
		t->vertices[i].y += window_height / 2.0;
	}
}

void triangle_position_on_screen(triangle_t *t, int window_width, int window_height) {
	triangle_scale_to_screen(t, window_width, window_height);
	triangle_translate_to_center(t, window_width, window_height);
}

bool triangle_less_depth(const void* a, const void* b) {
	triangle_t* ta = (triangle_t*)a;
	triangle_t* tb = (triangle_t*)b;
	return ta->avg_depth < tb->avg_depth;
}

// void triangle_set_tex_coords_from_face(triangle_t* t, mesh_face_t f) {
// 	t->tex_coords[0] = (tex2_t){ f.a_uv.u, f.a_uv.v };
// 	t->tex_coords[1] = (tex2_t){ f.b_uv.u, f.b_uv.v };
// 	t->tex_coords[2] = (tex2_t){ f.c_uv.u, f.c_uv.v };
// }
