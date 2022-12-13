#include "mesh.h"

mesh_t g_mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0, 0, 0 },
	.scale = { 1.0, 1.0, 1.0 },
	.translation = { 0, 0, 0 },
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{.x = -1, .y = -1, .z = -1 }, // 1
	{.x = -1, .y = 1, .z = -1 }, // 2
	{.x = 1, .y = 1, .z = -1 }, // 3
	{.x = 1, .y = -1, .z = -1 }, // 4
	{.x = 1, .y = 1, .z = 1 }, // 5
	{.x = 1, .y = -1, .z = 1 }, // 6
	{.x = -1, .y = 1, .z = 1 }, // 7
	{.x = -1, .y = -1, .z = 1 } // 8
};

mesh_face_t cube_faces[N_CUBE_FACES] = {
	// front
	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = RED },
	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = RED },
	// right
	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = GREEN },
	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = GREEN },
	// back
	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = BLUE },
	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = BLUE },
	// left
	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = YELLOW },
	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = YELLOW },
	// top
	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = MAGENTA },
	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = MAGENTA },
	// bottom
	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = CYAN },
	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = CYAN },
};

const int MAX_LINE = 512;

// new_mesh_face constructs a mesh_face with a reference to the vertex array of the larger mesh for
// convenience when looking up its vertices.
mesh_face_t new_mesh_face(vec3_t* mesh_vertices) {
	return (mesh_face_t){
		.mesh_vertices = mesh_vertices,
		.color = WHITE
	};
}

vec3_t mesh_face_vertex_a(const mesh_face_t* mf) {
	return mf->mesh_vertices[mf->a - 1];
}

vec3_t mesh_face_vertex_b(const mesh_face_t* mf) {
	return mf->mesh_vertices[mf->b - 1];
}

vec3_t mesh_face_vertex_c(const mesh_face_t* mf) {
	return mf->mesh_vertices[mf->c - 1];
}


int parse_vertex(const char* line, mesh_t* dst) {
	vec3_t v;
	int filled = sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
	if (filled < 3) {
		fprintf(stderr, "failed to parse vertex for line \"%s\"\n", line);
		return -1;
	}

	array_push(dst->vertices, v);
	return 0;
}

// parse_face parses a single face from an obj file.
int parse_face(const char* line, mesh_t* dst) {
	mesh_face_t face = new_mesh_face(dst->vertices);
	mesh_face_t texture;
	mesh_face_t normal;
	int filled = sscanf(
		line,
		"f %d/%d/%d %d/%d/%d %d/%d/%d",
		&face.a, &texture.a, &normal.a,
		&face.b, &texture.b, &normal.b,
		&face.c, &texture.c, &normal.c
	);
	if (filled < 9) {
		fprintf(stderr, "failed to parse face for line \"%s\"\n", line);
		return -1;
	}

	array_push(dst->faces, face);
	return 0;
}

int parse_line(const char* line, mesh_t* dst) {
	int err = 0;
	if (strncmp(line, "v ", 2) == 0) {
		err = parse_vertex(line, dst);
	}
	else if (strncmp(line, "f ", 2) == 0) {
		err = parse_face(line, dst);
	}

	return err;
}

// parse_obj_file takes the path of the obj file and the dynamic arrays vert_out and face_out to
// write vertex and face data to, respectively.
int parse_obj_file(const char* path, mesh_t* dst) {
	FILE* obj = must_fopen(path, "r");
	char line[MAX_LINE];
	while (fgets(line, MAX_LINE, obj) != NULL) {
		int err = parse_line(line, dst);
		if (err) {
			return -1;
		}
	}
	if (!feof(obj)) {
		fprintf(stderr, "failed to read to end of %s\n", path);
		return -1;
	}
	fclose(obj);

	return 0;
}

void load_cube() {
	for (int i = 0; i < N_CUBE_VERTICES; i++) {
		array_push(g_mesh.vertices, cube_vertices[i]);
	}
	for (int i = 0; i < N_CUBE_FACES; i++) {
		array_push(g_mesh.faces, cube_faces[i]);
	}
}

int load_mesh(const char* path) {
	return parse_obj_file(path, &g_mesh);
}

mat4_t mesh_to_world_matrix(const mesh_t* mesh) {
	mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

	// Order matters b/c transformations are centred on the origin: scale -> rotate -> translate.
	mat4_t world_matrix = mat4_mul(&rotation_matrix_z, &scale_matrix);
	world_matrix = mat4_mul(&rotation_matrix_y, &world_matrix);
	world_matrix = mat4_mul(&rotation_matrix_x, &world_matrix);
	return mat4_mul(&translation_matrix, &world_matrix);
}
