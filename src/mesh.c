#include "mesh.h"

mesh_t mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0, 0, 0 },
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

face_t cube_faces[N_CUBE_FACES] = {
	// front
	{.a = 1, .b = 2, .c = 3, .color = RED },
	{.a = 1, .b = 3, .c = 4, .color = RED },
	// right
	{.a = 4, .b = 3, .c = 5, .color = GREEN },
	{.a = 4, .b = 5, .c = 6, .color = GREEN },
	// back
	{.a = 6, .b = 5, .c = 7, .color = BLUE },
	{.a = 6, .b = 7, .c = 8, .color = BLUE },
	// left
	{.a = 8, .b = 7, .c = 2, .color = YELLOW },
	{.a = 8, .b = 2, .c = 1, .color = YELLOW },
	// top
	{.a = 2, .b = 7, .c = 5, .color = MAGENTA },
	{.a = 2, .b = 5, .c = 3, .color = MAGENTA },
	// bottom
	{.a = 6, .b = 8, .c = 1, .color = CYAN },
	{.a = 6, .b = 1, .c = 4, .color = CYAN },
};

void load_cube() {
	for (int i = 0; i < N_CUBE_VERTICES; i++) {
		array_push(mesh.vertices, cube_vertices[i]);
	}
	for (int i = 0; i < N_CUBE_FACES; i++) {
		array_push(mesh.faces, cube_faces[i]);
	}
}

int load_mesh(char* path) {
	return parse_obj_file(path, &mesh);
}