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
#include "vector.h"
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
	RENDER_MODE_SOLID_WIREFRAME,
	RENDER_MODE_TEXTURE,
	RENDER_MODE_TEXTURE_WIREFRAME,
} render_mode_t;
extern render_mode_t render_mode;

typedef struct display_triangle_t {
	int ax, ay, bx, by, cx, cy;
	int au, av, bu, bv, cu, cv;
} display_triangle_t;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern color_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int g_window_width;
extern int g_window_height;
extern bool g_enable_backface_culling;
extern mat4_t g_projection_matrix;
extern float fov_rads;
extern vec3_t g_camera_position;

bool initialize_window(void);
triangleRenderFunc triangleRendererForMode();
void draw_pixel(int x, int y, color_t color);
void draw_grid(void);
void draw_line(int x0, int y0, int x1, int y1, color_t color);
void draw_triangle(triangle_t t, color_t color);
void fill_triangle(triangle_t t);
// void texture_triangle(display_triangle_t t);
void draw_rectangle(int x, int y, int w, int h, color_t color);
void render_color_buffer(void);
void clear_color_buffer(color_t color);
void destroy_window(void);

#endif