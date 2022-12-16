#include "triangle.h"

triangle_t new_triangle() {
	triangle_t t = {
		.vertices = {0},
		.tex_coords = {0},
		.fill = DEFAULT_FILL_COLOR,
		.border = DEFAULT_BORDER_COLOR,
		.avg_depth = 0,
	};
	return t;
}

triangle_t new_triangle_from_face(const face_t* f, const mat4_t* projection) {
	triangle_t t = new_triangle();
	t.fill = f->color;
	t.avg_depth = face_avg_depth(f);

	for (int i = 0; i < 3; i++) {
		t.vertices[i] = mat4_project_vec3(projection, &f->vertices[i]);
		t.tex_coords[i] = f->tex_coords[i];
	}

	return t;
}

vec2_t triangle_vertex_a(const triangle_t* t) {
	return t->vertices[0];
}

vec2_t triangle_vertex_b(const triangle_t* t) {
	return t->vertices[1];
}

vec2_t triangle_vertex_c(const triangle_t* t) {
	return t->vertices[2];
}

tex2_t triangle_tex_a(const triangle_t* t) {
	return t->tex_coords[0];
}

tex2_t triangle_tex_b(const triangle_t* t) {
	return t->tex_coords[1];
}

tex2_t triangle_tex_c(const triangle_t* t) {
	return t->tex_coords[2];
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

bool triangle_has_zero_height(const triangle_t* t) {
	return fabsf(t->vertices[0].y - t->vertices[1].y) < 1 && fabsf(t->vertices[1].y - t->vertices[2].y) < 1;
}

// Insertion sort a triangle's vertices and their corresponding tex_coords by their y-coordinates.
void triangle_sort_vertices_by_y(triangle_t* t) {
	for (int i = 1; i < 3; i++) {
		int j;
		vec2_t vert = t->vertices[i];
		tex2_t tex = t-> tex_coords[i];
		for (j = i; j > 0 && vert.y < t->vertices[j-1].y; j--) {
			t->vertices[j] = t->vertices[j-1];
			t->tex_coords[j] = t->tex_coords[j-1];
		}
		t->vertices[j] = vert;
		t->tex_coords[j] = tex;
	}
}

void triangle_truncate_vertices(triangle_t* t) {
	for (int i = 0; i < 3; i++) {
		vec2_t v = t->vertices[i];
		v.x = (int)v.x;
		v.y = (int)v.y;
		t->vertices[i] = v; 
	}
}

vec2_t triangle_b_hyp_intercept(const triangle_t* t) {
	vec2_t a = triangle_vertex_a(t);
	vec2_t b = triangle_vertex_b(t);
	vec2_t c = triangle_vertex_c(t);
	vec2_t intercept = {
		.x = ((c.x - a.x) * (b.y - a.y) / (c.y - a.y)) + a.x,
		.y = b.y,
	};

	return intercept;
}


// Return the barycentric weights for point p within the given triangle, t.
vec3_t triangle_barycentric_weights(const triangle_t* t, vec2_t p) {
	// Find the vectors between each vertex and point p.
	vec2_t a = triangle_vertex_a(t);
	vec2_t b = triangle_vertex_b(t);
	vec2_t c = triangle_vertex_c(t);
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t pb = vec2_sub(b, p);
	vec2_t pc = vec2_sub(c, p);

	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);
	float area_parallelogram_pcb = (pc.x * pb.y - pc.y * pb.x);
	float area_parallelogram_acp = (ac.x * ap.y - ac.y * ap.x);
	float alpha = area_parallelogram_pcb / area_parallelogram_abc;
	float beta = area_parallelogram_acp / area_parallelogram_abc;
	float gamma = 1.0 - alpha - beta;

	return (vec3_t) { alpha, beta, gamma };
}
