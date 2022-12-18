#include "mesh.h"

mesh_t g_mesh = {
	.vertices = NULL,
	.faces = NULL,
	.tex_coords = NULL,
	.rotation = { 0, 0, 0 },
	.scale = { 1.0, 1.0, 1.0 },
	.translation = { 0, 0, 0 },
};

const int MAX_LINE = 512;

// new_mesh_face constructs a mesh_face with a reference to the vertex array of the larger mesh for
// convenience when looking up its vertices.
mesh_face_t new_mesh_face(const vec3_t* mesh_vertices, const tex2_t* mesh_tex_coords) {
	return (mesh_face_t){
		.mesh_vertices = mesh_vertices,
		.mesh_tex_coords = mesh_tex_coords,
		.color = DEFAULT_FILL_COLOR
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

tex2_t mesh_face_tex_a(const mesh_face_t* mf) {
	return mf->mesh_tex_coords[mf->a_uv - 1];
}

tex2_t mesh_face_tex_b(const mesh_face_t* mf) {
	return mf->mesh_tex_coords[mf->b_uv - 1];
}

tex2_t mesh_face_tex_c(const mesh_face_t* mf) {
	return mf->mesh_tex_coords[mf->c_uv - 1];
}

int parse_uv(const char* line, mesh_t* dst) {
	tex2_t tex;
	int filled = sscanf(line, "vt %f %f", &tex.u, &tex.v);
	if (filled < 2) {
		fprintf(stderr, "failed to parse UV data for line \"%s\"\n", line);
		return -1;
	}

	array_push(dst->tex_coords, tex);
	return 0;
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
	mesh_face_t face = new_mesh_face(dst->vertices, dst->tex_coords);
	mesh_face_t normal;
	int filled = sscanf(
		line,
		"f %d/%d/%d %d/%d/%d %d/%d/%d",
		&face.a, &face.a_uv, &normal.a,
		&face.b, &face.b_uv, &normal.b,
		&face.c, &face.c_uv, &normal.c
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
	}  else if (strncmp(line, "vt ", 3) == 0) {
		err = parse_uv(line, dst);
	} else if (strncmp(line, "f ", 2) == 0) {
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
