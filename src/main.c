#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "safe_malloc.h"
#include "vector.h"

const int N_POINTS = 9 * 9 * 9;
const float fov_factor = 120;

vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

bool is_running = false;

void setup(void) {
	color_buffer = safe_malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	int point_count = 0;
	for (float x = -1; x <= 1; x += 0.25) {
		for (float y = -1; y <= 1; y += 0.25) {
			for (float z = -1; z <= 1; z += 0.25) {
				vec3_t point = { x, y, z };
				cube_points[point_count++] = point;
			}
		}
	}
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

// project receives a 3D vector and returns projected 2D point.
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = fov_factor * point.x,
		.y = fov_factor * point.y
	};
	return projected_point;
}

void update(void) {
	for (int i = 0; i < N_POINTS; i++) {
		projected_points[i] = project(cube_points[i]);
	}
}

void render(void) {
	draw_grid();
	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		draw_rectangle(
			projected_point.x + window_width / 2,
			projected_point.y + window_height / 2,
			4,
			4,
			0xFFFFFF00
		);
	}
	render_color_buffer();
	clear_color_buffer(0xFF000000);

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