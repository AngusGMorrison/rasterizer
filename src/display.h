// display.h provides constants, global variables, types and functions for initializing and
// rendering the scene.
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "color.h"
#include "face.h"
#include "float.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"

/*
Constants, global variables and enums
*/

// Target framerate.
extern const int FPS;

// Target time in which to render a single frame.
extern const uint32_t FRAME_TARGET_TIME;

// Dimensions of the rendered vertex.
extern const int VERTEX_RECT_WIDTH_PX;

// Global light source.
extern const light_t g_light;

// Vertical field of view in radians.
extern const float fov_rads;

typedef enum render_mode_t {
	RENDER_MODE_VERTEX,
	RENDER_MODE_WIREFRAME,
	RENDER_MODE_VERTEX_WIREFRAME,
	RENDER_MODE_FILL,
	RENDER_MODE_FILL_WIREFRAME,
	RENDER_MODE_TEXTURE,
	RENDER_MODE_TEXTURE_WIREFRAME,
} render_mode_t;

// Global render mode.
extern render_mode_t g_render_mode;

// The SDL rendering window.
extern SDL_Window* g_window;

// The SDL renderer.
extern SDL_Renderer* g_renderer;

// Buffer to which all updates are written before being copied to the SDL_Texture.
extern color_t* g_color_buffer;

// The buffer used by the renderer to update the screen.
extern SDL_Texture* g_color_buffer_texture;

// Screen width.
extern int g_window_width;

// Screen height.
extern int g_window_height;

// Toggle back-face culling.
extern bool g_enable_back_face_culling;

// Global matrix to project the 3D scene onto the 2D screen.
extern mat4_t g_projection_matrix;

// Position of the camera.
extern vec3_t g_camera_position;

/*
Functions
*/

// Initialize the global SDL window.
bool initialize_window(void);

// Render a triangle to the global color buffer according to the current render mode.
void render_triangle(const triangle_t* t);

// Render the color buffer to the global SDL texture.
void render_color_buffer(void);

// Clear the color buffer with the specified color.
void clear_color_buffer(color_t color);

// Free all resources associated with the SDL window.
void destroy_window(void);

#endif