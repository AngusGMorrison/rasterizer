#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "float.h"
#include "triangle.h"

extern const int FPS;
extern const uint32_t FRAME_TARGET_TIME;
extern const int GRID_SPACING_PX;
extern const int VERTEX_RECT_WIDTH_PX;

extern const uint32_t BLACK;
extern const uint32_t GREEN;
extern const uint32_t RED;
extern const uint32_t WHITE;
extern const uint32_t YELLOW;

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
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;
extern bool enable_backface_culling;

bool initialize_window(void);
triangleRenderFunc triangleRendererForMode();
bool shouldCull(vec3_t* vertices);
void draw_pixel(int x, int y, uint32_t color);
void draw_grid(void);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(triangle_t t, uint32_t color);
void fill_triangle(triangle_t t, uint32_t color);
void draw_rectangle(int x, int y, int w, int h, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);

#endif