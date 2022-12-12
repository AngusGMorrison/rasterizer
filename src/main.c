#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "must.h"
#include "obj_parser.h"
#include "vector.h"


// Global variables for execution status and game loop.
bool is_running = false;
Uint64 prev_frame_time = 0;
triangle_t* triangles_to_render = NULL; // dynamic array of triangles to render

int setup(void) {
	color_buffer = must_malloc(sizeof(color_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	projection_matrix = mat4_make_perspective(fov_rads, window_height / (float)window_width, 0.1, 100.0);
	return load_mesh("assets/f22.obj");
}

void process_keydown(SDL_KeyCode key) {
	switch (key) {
	case SDLK_ESCAPE:
		is_running = false;
		break;
	case SDLK_1:
		render_mode = RENDER_MODE_VERTEX;
		break;
	case SDLK_2:
		render_mode = RENDER_MODE_VERTEX_WIREFRAME;
		break;
	case SDLK_3:
		render_mode = RENDER_MODE_WIREFRAME;
		break;
	case SDLK_4:
		render_mode = RENDER_MODE_SOLID;
		break;
	case SDLK_5:
		render_mode = RENDER_MODE_SOLID_WIREFRAME;
		break;
	case SDLK_c:
		enable_backface_culling = !enable_backface_culling;
		break;
	default:
		// Do nothing.
		break;
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
		process_keydown(event.key.keysym.sym);
		break;
	}
}

void await_frame(void) {
	uint32_t elapsed = SDL_GetTicks64() - prev_frame_time;
	if (elapsed < FRAME_TARGET_TIME) {
		SDL_Delay(FRAME_TARGET_TIME - elapsed);
	}
	prev_frame_time = SDL_GetTicks64();
}

void update_mesh(void) {
	mesh.rotation.x += 0.05;
	mesh.rotation.y += 0.05;
	mesh.rotation.z += 0.05;
	mesh.scale.x += 0.002;
	mesh.scale.y += 0.001;
	mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;
}

void update(void) {
	await_frame();
	array_clear(triangles_to_render, sizeof(triangle_t));

	update_mesh();
	mat4_t world_matrix = mesh_to_world_matrix(&mesh);

	vec3_t vertices[3];
	int faces = array_len(mesh.faces);
	for (int i = 0; i < faces; i++) {
		face_t face = mesh.faces[i];
		vertices[0] = mesh.vertices[face.a - 1];
		vertices[1] = mesh.vertices[face.b - 1];
		vertices[2] = mesh.vertices[face.c - 1];

		for (int j = 0; j < 3; j++) {
			vec4_t conformable = vec4_from_vec3(vertices[j]);
			vec4_t transformed = mat4_mul_vec4(world_matrix, conformable);
			vertices[j] = vec3_from_vec4(transformed);
		}

		if (shouldCull(vertices)) {
			continue;
		}

		triangle_t triangle = {
			.color = face.color
		};
		float total_depth = 0.0;
		for (int j = 0; j < 3; j++) {
			// Project into the canonical view (1x1x1 box - Normalized Device Coordinates).
			vec3_t projected = mat4_project_vec3(projection_matrix, vertices[j]);
			// Scale to screen size.
			projected.x *= window_width / 2.0;
			projected.y *= window_height / 2.0;
			// Translate to centre of screen.
			projected.x += window_width / 2.0;
			projected.y += window_height / 2.0;
			// Save output.
			triangle.points[j] = vec2_from_vec3(projected);
			total_depth += vertices[j].z;
		}
		triangle.avg_depth = total_depth / 3.0;

		array_push(triangles_to_render, triangle);
	}
}

// Render triangles using the painter's algorithm, starting with the deepest triangles and painting
// over them with shallower ones.
void render(void) {
	quick_sort(
		triangles_to_render,
		array_len(triangles_to_render),
		sizeof(triangle_t),
		triangle_less_depth
	);
	triangleRenderFunc renderTriangle = triangleRendererForMode();
	int len = array_len(triangles_to_render);
	for (int i = len - 1; i >= 0; i--) {
		renderTriangle(triangles_to_render[i]);
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