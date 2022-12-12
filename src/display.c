#include "display.h"

const int FPS = 60;
const uint32_t FRAME_TARGET_TIME = 1000 / FPS;
const int GRID_SPACING_PX = 10;
const int VERTEX_RECT_WIDTH_PX = 5;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
color_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;
vec3_t camera_position = { 0, 0, 0 };
render_mode_t render_mode = RENDER_MODE_SOLID_WIREFRAME;
bool enable_backface_culling = true;
mat4_t projection_matrix;
float fov_rads = M_PI / 3;

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

void render_triangle_vertices(triangle_t t) {
	for (int j = 0; j < 3; j++) {
		vec2_t point = t.points[j];
		draw_rectangle(point.x, point.y, VERTEX_RECT_WIDTH_PX, VERTEX_RECT_WIDTH_PX, RED);
	}
}

void render_triangle_vertex_wireframe(triangle_t t) {
	draw_triangle(t, WHITE);
	render_triangle_vertices(t);
}

void render_triangle_wireframe(triangle_t t) {
	draw_triangle(t, GREEN);
}

void render_triangle_solid(triangle_t t) {
	fill_triangle(t);
}

void render_triangle_solid_wireframe(triangle_t t) {
	render_triangle_solid(t);
	draw_triangle(t, BLACK);
}

triangleRenderFunc triangleRendererForMode() {
	switch (render_mode) {
	case RENDER_MODE_VERTEX:
		return &render_triangle_vertices;
		break;
	case RENDER_MODE_WIREFRAME:
		return &render_triangle_wireframe;
		break;
	case RENDER_MODE_VERTEX_WIREFRAME:
		return &render_triangle_vertex_wireframe;
		break;
	case RENDER_MODE_SOLID:
		return &render_triangle_solid;
		break;
	default:
		return &render_triangle_solid_wireframe;
	}
}

bool shouldCull(vec3_t* vertices) {
	if (!enable_backface_culling) {
		return false;
	}

	vec3_t a = vertices[0];
	vec3_t b = vertices[1];
	vec3_t c = vertices[2];
	vec3_t ab = vec3_normalize(vec3_sub(b, a));
	vec3_t ac = vec3_normalize(vec3_sub(c, a));
	vec3_t n = vec3_normalize(vec3_cross(ab, ac));
	vec3_t acam = vec3_sub(camera_position, a);

	if (vec3_dot(n, acam) <= 0) {
		return true;
	}
	return false;
}


// TODO: Convert all draw methods to use vecs and triangles until the last possible moment.

void draw_pixel(int x, int y, color_t color) {
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

void draw_line(int x0, int y0, int x1, int y1, color_t color) {
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

void draw_triangle(triangle_t t, color_t color) {
	draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, color);
	draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, color);
	draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, color);
}

// triangle_horizontal_b_hyp_intercept returns the vec2 at which a horizontal line projected from
// the triangle's middle vertex (by y-value), b, will intercept the hypoteneuse ac, producing two
// triangles with a flat bottom and flat top, respectively.
//
// t.points must be sorted by y-value.
vec2_t triangle_horizontal_b_hyp_intercept(const triangle_t* t) {
	vec2_t a = t->points[0];
	vec2_t b = t->points[1];
	vec2_t c = t->points[2];

	vec2_t v = {
		.x = ((c.x - a.x) * (b.y - a.y)) / (c.y - a.y) + a.x,
		.y = b.y,
	};

	return v;
}

/*
fill_flat_bottom_triangle renders a filled, flat-bottomed triangle by scanning from top to bottom
and calculating the difference in the x positions of the opposing slopes.

			 0
			/ \
		l  /   \  r
		  /_____\
		 1       2
*/
void fill_flat_bottom_triangle(const triangle_t* t) {
	// Calculate the change in x with respect to y (inverse gradient) for both sloped sides of the
	// flat-bottomed triangle. We know that y (representing the current scan line) will increase
	// monotonically – the change in x is our unknown.
	float inv_m_left = vec2_inv_gradient(t->points[0], t->points[1]);
	float inv_m_right = vec2_inv_gradient(t->points[2], t->points[0]);
	float x_start = t->points[0].x;
	float x_end = x_start;


	// Fill the triangle from top to bottom.
	float max_width = fabsf(t->points[2].x - t->points[1].x);
	for (int y = t->points[0].y; y <= (int)t->points[2].y; y++) {
		draw_line(x_start, y, x_end, y, t->color);

		x_start += inv_m_left;
		x_end += inv_m_right;
		// Prevent x_start from escaping the bounds of the triangle when inv_m is large.
		if (fabsf(x_end - x_start) > max_width) {
			x_start = t->points[1].x;
			x_end = t->points[2].x;
		}
	}
}

/*
fill_flat_top_triangle renders a filled, flat-topped triangle by scanning from bottom to top
and calculating the difference in the x positions of the opposing slopes.

		 0 _____ 1
		  \     /
		l  \   /  r
			\ /
			 2
*/
void fill_flat_top_triangle(const triangle_t* t) {
	// Calculate the change in x with respect to y (inverse gradient) for both sloped sides of the
	// flat-topped triangle. We know that y (representing the current scan line) will decrease
	// monotonically – the change in x is our unknown.
	float inv_m_left = vec2_inv_gradient(t->points[2], t->points[0]);
	float inv_m_right = vec2_inv_gradient(t->points[1], t->points[2]);
	float x_start = t->points[2].x;
	float x_end = x_start;

	// Fill the triangle from bottom to top. This loop draws one line fewer than
	// fill_flat_bottom_triangle to avoid double-rendering the join between top and bottom.
	float max_width = fabsf(t->points[1].x - t->points[0].x);
	for (int y = t->points[2].y; y > (int)t->points[0].y; y--) {
		draw_line(x_start, y, x_end, y, t->color);

		// We're moving "against" the gradient, so subtract m on each iteration.
		x_start -= inv_m_left;
		x_end -= inv_m_right;
		// Prevent x_start from escaping the bounds of the triangle when inv_m is large.
		if (fabsf(x_end - x_start) > max_width) {
			x_start = t->points[0].x;
			x_end = t->points[1].x;
		}
	}
}

// TODO: Pass by pointer
void fill_triangle(triangle_t t) {
	triangle_sort_vertices_y(&t);
	vec2_t a = t.points[0];
	vec2_t b = t.points[1];
	vec2_t c = t.points[2];

	// If the input triangle is already flat-topped or flat-bottomed, perform only the required
	// operation. Otherwise, attempting to fill the zero-height opposite half will cause a zero
	// division error.
	if (float_approx_equal(a.y, b.y)) {
		fill_flat_top_triangle(&t);
		return;
	}
	if (float_approx_equal(b.y, c.y)) {
		fill_flat_bottom_triangle(&t);
		return;
	}

	// Calculate the point at the intercept of the horizontal line originating at b with ac, using
	// triangle similarity.
	vec2_t m = triangle_horizontal_b_hyp_intercept(&t);
	triangle_t flat_bottom = {
		.points = {a, b, m},
		.color = t.color
	};
	triangle_t flat_top = {
		.points = {b, m, c},
		.color = t.color
	};

	fill_flat_bottom_triangle(&flat_bottom);
	fill_flat_top_triangle(&flat_top);
}

void draw_rectangle(int x, int y, int w, int h, color_t color) {
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
		(window_width * sizeof(color_t))
	);
	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);
}

void clear_color_buffer(color_t color) {
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
