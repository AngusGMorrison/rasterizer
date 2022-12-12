# include "face.h"

face_t new_face() {
	face_t f = {
		.has_normal = false,
		.color = WHITE
	};
	return f;
}

vec3_t face_normal(vec3_t* vertices) {
	vec3_t a = vertices[0];
	vec3_t b = vertices[1];
	vec3_t c = vertices[2];
	vec3_t ab = vec3_normalize(vec3_sub(b, a));
	vec3_t ac = vec3_normalize(vec3_sub(c, a));
	return vec3_normalize(vec3_cross(ab, ac));
}


color_t face_illuminate(vec3_t* vertices, light_t l, color_t color) {
	vec3_t n = face_normal(vertices);
	vec3_t a = vec3_normalize(vertices[0]);
	vec3_t nl = vec3_normalize(l.direction);
	vec3_t al = vec3_normalize(vec3_sub(nl, a));
	float dot = vec3_dot(n, al);
	if (dot <= 0.0) { // faces that are aligned with or face away from the light should not be illuminated
		dot = 0.0;
	}
	return color_illuminate(color, dot);
}