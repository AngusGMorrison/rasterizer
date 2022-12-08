#include "display.h"

const int FPS = 60;
const uint32_t FRAME_TARGET_TIME = 1000 / FPS;
const int GRID_SPACING_PX = 10;

const uint32_t BLACK = 0xFF000000;
const uint32_t YELLOW = 0xFFFFFF00;
const uint32_t GREEN = 0xFF00FF00;


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_FULLSCREEN
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	return true;
}

void draw_pixel(int x, int y, uint32_t color) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}

	color_buffer[window_width * y + x] = color;
}

void draw_grid(void) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			if (y % GRID_SPACING_PX == 0 || x % GRID_SPACING_PX == 0) {
				draw_pixel(x, y, BLACK);
			}
		}
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int abs_dx = abs(dx);
	int abs_dy = abs(dy);

	// Select the greater side length, since in order to draw only one pixel per loop iteration, we
	// must treat our line from the perspective of it being <= 45 degrees. Otherwise, y increments >
	// 1 will cause holes in the line.
	int max_side_length = abs_dx >= abs_dy ? abs_dx : abs_dy;
	float x_inc = dx / (float)max_side_length;
	float y_inc = dy / (float)max_side_length;

	float cur_x = x0;
	float cur_y = y0;
	for (int i = 0; i <= max_side_length; i++) {
		draw_pixel(round(cur_x), round(cur_y), color);
		cur_x += x_inc;
		cur_y += y_inc;
	}
}

void draw_triangle(triangle_t t, uint32_t color) {
	fprintf(stdout, "drawing triangle (%.6f,%.6f), (%.6f,%.6f), (%.6f,%.6f)\n", t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y);
	draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, color);
	draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, color);
	draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, color);
}

void draw_rectangle(int x, int y, int w, int h, uint32_t color) {
	for (int i = 0; i < w; i++) {
		int currentX = x + i;
		for (int j = 0; j < h; j++) {
			int currentY = y + j;
			draw_pixel(currentX, currentY, color);
		}
	}
}

void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);
}

void clear_color_buffer(uint32_t color) {
	int pixels = window_height * window_width;
	for (int i = 0; i < pixels; i++) {
		color_buffer[i] = color;
	}
}

void destroy_window(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
