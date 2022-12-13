#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "mesh.h"
#include "must.h"
#include "vector.h"


// Global variables for execution status and game loop.
bool g_is_running = false;
Uint64 g_prev_frame_time = 0;
triangle_t* g_triangles_to_render = NULL; // dynamic array of triangles to render

int setup(void) {
	g_color_buffer = must_malloc(sizeof(color_t) * g_window_width * g_window_height);
	g_color_buffer_texture = SDL_CreateTexture(
		g_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		g_window_width,
		g_window_height
	);
	g_projection_matrix = mat4_make_perspective(fov_rads, g_window_height / (float)g_window_width, 0.1, 100.0);
	return load_mesh("assets/f22.obj");
}

void process_keydown(SDL_KeyCode key) {
	switch (key) {
	case SDLK_ESCAPE:
		g_is_running = false;
		break;
	case SDLK_1:
		g_render_mode = RENDER_MODE_VERTEX;
		break;
	case SDLK_2:
		g_render_mode = RENDER_MODE_VERTEX_WIREFRAME;
		break;
	case SDLK_3:
		g_render_mode = RENDER_MODE_WIREFRAME;
		break;
	case SDLK_4:
		g_render_mode = RENDER_MODE_FILL;
		break;
	case SDLK_5:
		g_render_mode = RENDER_MODE_FILL_WIREFRAME;
		break;
	// case SDLK_6:
	// 	g_render_mode = RENDER_MODE_TEXTURE;
	// 	break;
	// case SDLK_7:
	// 	g_render_mode = RENDER_MODE_TEXTURE_WIREFRAME;
	// 	break;
	case SDLK_c:
		g_enable_back_face_culling = !g_enable_back_face_culling;
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
		g_is_running = false;
		break;
	case SDL_KEYDOWN:
		process_keydown(event.key.keysym.sym);
		break;
	}
}

// Wait until the target time has elapsed before rendering the next frame to maintain a constant
// frame rate.
void await_frame(void) {
	uint32_t elapsed = SDL_GetTicks64() - g_prev_frame_time;
	if (elapsed < FRAME_TARGET_TIME) {
		SDL_Delay(FRAME_TARGET_TIME - elapsed);
	}
	g_prev_frame_time = SDL_GetTicks64();
}

void update_mesh(void) {
	g_mesh.rotation.x += 0.05;
	// g_mesh.rotation.y += 0.05;
	// g_mesh.rotation.z += 0.05;
	// g_mesh.scale.x += 0.002;
	// g_mesh.scale.y += 0.001;
	// g_mesh.translation.x += 0.01;
	g_mesh.translation.z = 5.0;
}

// Create a new set of triangles to render based on the latest position of the mesh.
void update(void) {
	await_frame();
	array_reset(g_triangles_to_render, sizeof(triangle_t));

	update_mesh();
	mat4_t world_matrix = mesh_to_world_matrix(&g_mesh);

	int n_mesh_faces = array_len(g_mesh.faces);
	for (int i = 0; i < n_mesh_faces; i++) {
		face_t face = new_face_from_mesh_face(&g_mesh.faces[i]);
		face_transform(&face, &world_matrix);
		if (g_enable_back_face_culling && face_should_cull(&face, g_camera_position)) {
			continue;
		}

		face_illuminate(&face, &g_light);
		triangle_t triangle = new_triangle_from_face(&face, &g_projection_matrix);
		triangle_position_on_screen(&triangle, g_window_width, g_window_height);
		// triangle_set_tex_coords_from_face(&triangle, mesh_face);

		array_push(g_triangles_to_render, triangle);
	}
}

void render_triangles_to_color_buffer(void) {
	int len = array_len(g_triangles_to_render);
	quick_sort(g_triangles_to_render, len, sizeof(triangle_t), triangle_less_depth);
	for (int i = len - 1; i >= 0; i--) {
		render_triangle(&g_triangles_to_render[i]);
	}
}

// Render triangles using the painter's algorithm, starting with the deepest triangles and painting
// over them with shallower ones.
void render(void) {
	render_triangles_to_color_buffer();
	render_color_buffer();
	clear_color_buffer(BLACK);
	SDL_RenderPresent(g_renderer);
}

void free_resources(void) {
	array_free(g_mesh.faces);
	array_free(g_mesh.vertices);
	free(g_color_buffer);
}

int main(void) {
	g_is_running = initialize_window();

	int err = setup();
	if (err) {
		fprintf(stderr, "fatal error in setup\n");
		abort();
	}

	while (g_is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}