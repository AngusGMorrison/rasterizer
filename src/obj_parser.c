#include "obj_parser.h"

const int MAX_LINE = 500;
const char* ORIGIN = "0.000000 0.000000 0.000000";

bool obj_line_is_origin(char* line) {
	return memcmp(line, ORIGIN, 26) == 0;
}

int parse_vertex(char* line, mesh_t* dst) {
	char* ptr;
	vec3_t v = {
		.x = strtof(line, &ptr),
		.y = strtof(ptr, &ptr),
		.z = strtof(ptr, &ptr),
	};
	if (vec3_is_origin(v) && !obj_line_is_origin(line)) {
		fprintf(stderr, "failed to parse vertex for line \"%s\"\n", line);
		return -1;
	}

	array_push(dst->vertices, v);
	return 0;
}

int parse_face(char* line, mesh_t* dst) {
	char* token = strtok(line, " ");
	int vertexIDs[3];
	int i;
	for (i = 0; i < 3 && token != NULL; i++) {
		vertexIDs[i] = atoi(token);
		token = strtok(NULL, " ");
	}
	if (token == NULL && i < 3) {
		fprintf(stderr, "failed to parse face for line \"%s\"\n", line);
		return -1;
	}

	face_t f = {
		.a = vertexIDs[0],
		.b = vertexIDs[1],
		.c = vertexIDs[2],
	};
	array_push(dst->faces, f);
	return 0;
}

int parse_line(char* line, mesh_t* dst) {
	int err = 0;
	if (memcmp(line, "v ", 2) == 0) {
		err = parse_vertex(line + 2, dst);
	}
	else if (memcmp(line, "f ", 2) == 0) {
		err = parse_face(line + 2, dst);
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
