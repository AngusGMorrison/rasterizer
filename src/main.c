#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "mesh.h"
#include "safe_malloc.h"
#include "vector.h"

triangle_t triangles_to_render[N_MESH_FACES];
vec3_t camera_position = { 0, 0, -5 };
vec3_t cube_rotation = { 0, 0, 0 };

bool is_running = false;
Uint64 prev_frame_time = 0;

void setup(void) {
	color_buffer = safe_malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			is_running = false;
		}
		break;
	}
}

void wait_for_next_frame(void) {
	uint32_t elapsed = SDL_GetTicks64() - prev_frame_time;
	if (elapsed < FRAME_TARGET_TIME) {
		SDL_Delay(FRAME_TARGET_TIME - elapsed);
	}
	prev_frame_time = SDL_GetTicks64();
}

void update(void) {
	wait_for_next_frame();

	cube_rotation.x += 0.05;
	cube_rotation.y += 0.05;
	cube_rotation.z += 0.05;

	vec3_t vertices[3];
	for (int i = 0; i < N_MESH_FACES; i++) {
		face_t face = mesh_faces[i];
		vertices[0] = mesh_vertices[face.a - 1];
		vertices[1] = mesh_vertices[face.b - 1];
		vertices[2] = mesh_vertices[face.c - 1];

		triangle_t triangle;
		for (int j = 0; j < 3; j++) {
			vec3_t transformed = vec3_rotate_x(vertices[j], cube_rotation.x);
			transformed = vec3_rotate_y(transformed, cube_rotation.y);
			transformed = vec3_rotate_z(transformed, cube_rotation.z);
			transformed = vec3_translate_by(transformed, 0, 0, camera_position.z);
			vec2_t projected = vec3_project(transformed);
			triangle.points[j] = vec2_translate_by(projected, window_width / 2, window_height / 2);
		}

		triangles_to_render[i] = triangle;
	}
}

void render(void) {
	draw_grid();

	for (int i = 0; i < N_MESH_FACES; i++) {
		draw_triangle(triangles_to_render[i], GREEN);
	}
	render_color_buffer();
	clear_color_buffer(BLACK);

	SDL_RenderPresent(renderer);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}