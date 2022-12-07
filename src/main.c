#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "safe_malloc.h"
#include "vector.h"

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

	// for (int i = 0; i < N_POINTS; i++) {
	// 	vec3_t transformed = vec3_rotate_x(cube_points[i], cube_rotation.x);
	// 	transformed = vec3_rotate_y(transformed, cube_rotation.y);
	// 	transformed = vec3_rotate_z(transformed, cube_rotation.z);
	// 	transformed.z -= camera_position.z;
	// 	projected_points[i] = project(transformed);
	// }
}

void render(void) {
	draw_grid();
	// for (int i = 0; i < N_POINTS; i++) {
	// 	vec2_t projected_point = projected_points[i];
	// 	draw_rectangle(
	// 		projected_point.x + window_width / 2,
	// 		projected_point.y + window_height / 2,
	// 		4,
	// 		4,
	// 		0xFFFFFF00
	// 	);
	// }
	// render_color_buffer();
	// clear_color_buffer(0xFF000000);

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