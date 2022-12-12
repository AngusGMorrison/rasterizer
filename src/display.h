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
#include "matrix.h"
#include "triangle.h"

extern const int FPS;
extern const uint32_t FRAME_TARGET_TIME;
extern const int GRID_SPACING_PX;
extern const int VERTEX_RECT_WIDTH_PX;
extern const light_t g_light;

typedef void(*triangleRenderFunc) (triangle_t t);

typedef enum render_mode_t {
	RENDER_MODE_VERTEX,
	RENDER_MODE_WIREFRAME,
	RENDER_MODE_VERTEX_WIREFRAME,
	RENDER_MODE_SOLID,
	RENDER_MODE_SOLID_WIREFRAME
} render_mode_t;
extern render_mode_t render_mode;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern color_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;
extern bool enable_backface_culling;
extern mat4_t projection_matrix;
extern float fov_rads;

bool initialize_window(void);
triangleRenderFunc triangleRendererForMode();
bool shouldCull(vec3_t* vertices);
void draw_pixel(int x, int y, color_t color);
void draw_grid(void);
void draw_line(int x0, int y0, int x1, int y1, color_t color);
void draw_triangle(triangle_t t, color_t color);
void fill_triangle(triangle_t t);
void draw_rectangle(int x, int y, int w, int h, color_t color);
void render_color_buffer(void);
void clear_color_buffer(color_t color);
void destroy_window(void);

#endif