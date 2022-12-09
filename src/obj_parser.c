#include "obj_parser.h"

const int MAX_LINE = 512;

int parse_vertex(char* line, mesh_t* dst) {
	vec3_t v;
	int filled = sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
	if (filled < 3) {
		fprintf(stderr, "failed to parse vertex for line \"%s\"\n", line);
		return -1;
	}

	array_push(dst->vertices, v);
	return 0;
}

int parse_face(char* line, mesh_t* dst) {
	face_t face;
	face_t texture;
	face_t normal;
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

int parse_line(char* line, mesh_t* dst) {
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
int parse_obj_file(char* path, mesh_t* dst) {
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
