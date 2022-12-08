#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "mesh.h"
#include "must.h"
#include "obj_parser.h"
#include "vector.h"


// Global variables for execution status and game loop.
bool is_running = false;
Uint64 prev_frame_time = 0;
vec3_t camera_position = { 0, 0, -5 };
triangle_t* triangles_to_render = NULL; // dynamic array of triangles to render

int setup(void) {
	color_buffer = must_malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	return load_mesh("assets/f22.obj");
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
	array_clear(triangles_to_render, sizeof(triangle_t));

	mesh.rotation.x += 0.05;
	mesh.rotation.y += 0.05;
	mesh.rotation.z += 0.05;

	vec3_t vertices[3];
	int faces = array_len(mesh.faces);
	for (int i = 0; i < faces; i++) {
		face_t face = mesh.faces[i];
		vertices[0] = mesh.vertices[face.a - 1];
		vertices[1] = mesh.vertices[face.b - 1];
		vertices[2] = mesh.vertices[face.c - 1];

		triangle_t triangle;
		for (int j = 0; j < 3; j++) {
			vec3_t transformed = vec3_rotate_x(vertices[j], mesh.rotation.x);
			transformed = vec3_rotate_y(transformed, mesh.rotation.y);
			transformed = vec3_rotate_z(transformed, mesh.rotation.z);
			transformed = vec3_translate(transformed, 0, 0, camera_position.z);
			vec2_t projected = vec3_project(transformed);
			triangle.points[j] = vec2_translate(projected, window_width / 2, window_height / 2);
		}

		array_push(triangles_to_render, triangle);
	}
}

void render(void) {
	draw_grid();

	int len = array_len(triangles_to_render);
	for (int i = 0; i < len; i++) {
		draw_triangle(triangles_to_render[i], GREEN);
	}
	render_color_buffer();
	clear_color_buffer(BLACK);

	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	array_free(mesh.faces);
	array_free(mesh.vertices);
	free(color_buffer);
}

int main(void) {
	is_running = initialize_window();

	int err = setup();
	if (err) {
		fprintf(stderr, "fatal error in setup\n");
		abort();
	}

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}