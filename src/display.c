#include "display.h"

const int FPS = 60;
const uint32_t FRAME_TARGET_TIME = 1000 / FPS;
const int GRID_SPACING_PX = 10;
const int VERTEX_RECT_WIDTH_PX = 5;
const float fov_rads = M_PI / 3;
const light_t g_light = {
	{.x = -0.5, .y = -0.5, .z = -1}
};

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
color_t* g_color_buffer = NULL;
SDL_Texture* g_color_buffer_texture = NULL;
int g_window_width = 800;
int g_window_height = 600;
vec3_t g_camera_position = { 0 };
render_mode_t g_render_mode = RENDER_MODE_FILL_WIREFRAME;
bool g_enable_back_face_culling = true;
mat4_t g_projection_matrix = { 0 };

bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	g_window_width = display_mode.w;
	g_window_height = display_mode.h;

	g_window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		g_window_width,
		g_window_height,
		SDL_WINDOW_FULLSCREEN
	);
	if (!g_window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	g_renderer = SDL_CreateRenderer(g_window, -1, 0);
	if (!g_renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	return true;
}

display_point_t new_display_point_from_vec2(vec2_t v) {
	return (display_point_t) {
		.x = (int)v.x,
		.y = (int)v.y,
	};
}

// display_point_less_y is a comparator used to sort display points by their y-coordinates.
bool display_point_less_y(const void* a, const void* b) {
	display_point_t* dp_a = (display_point_t*)a;
	display_point_t* dp_b = (display_point_t*)b;
	return dp_a->y < dp_b->y;
}

float display_point_inv_gradient(display_point_t a, display_point_t b) {
	// Guard against zero division errors, which may occur when all three points of a triangle lie
	// on a line due to float to int truncation.
	if (a.y == b.y) {
		return 0;
	}

	return (b.x - a.x) / (float)(b.y - a.y);
}

void draw_pixel(display_point_t p, color_t color) {
	if (p.x < 0 || p.x >= g_window_width || p.y < 0 || p.y >= g_window_height) {
		return;
	}

	g_color_buffer[g_window_width * p.y + p.x] = color;
}

void draw_line(display_point_t a, display_point_t b, color_t color) {
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	int abs_dx = abs(dx);
	int abs_dy = abs(dy);

	// Select the greater side length, since in order to draw only one pixel per loop iteration, we
	// must treat our line from the perspective of it being <= 45 degrees. Otherwise, y increments >
	// 1 will cause holes in the line.
	int max_side_length = abs_dx >= abs_dy ? abs_dx : abs_dy;
	float x_inc = dx / (float)max_side_length;
	float y_inc = dy / (float)max_side_length;

	float cur_x = a.x;
	float cur_y = a.y;
	for (int i = 0; i <= max_side_length; i++) {
		display_point_t pixel = { round(cur_x), round(cur_y) };
		draw_pixel(pixel, color);
		cur_x += x_inc;
		cur_y += y_inc;
	}
}

void display_rectangle_draw(const display_rectangle_t* r) {
	for (int i = 0; i < r->w; i++) {
		int cur_x = r->point.x + i;
		for (int j = 0; j < r->h; j++) {
			int cur_y = r->point.y + j;
			display_point_t pixel = { cur_x, cur_y };
			draw_pixel(pixel, r->fill);
		}
	}
}

display_triangle_t new_display_triangle_from_triangle(const triangle_t* t) {
	return (display_triangle_t) {
		.vertices = {
			new_display_point_from_vec2(triangle_vertex_a(t)),
			new_display_point_from_vec2(triangle_vertex_b(t)),
			new_display_point_from_vec2(triangle_vertex_c(t)),
		},
		.fill = t->fill,
		.border = DEFAULT_BORDER_COLOR,
	};
}

display_point_t display_triangle_vertex_a(const display_triangle_t* t) {
	return t->vertices[0];
};

display_point_t display_triangle_vertex_b(const display_triangle_t* t) {
	return t->vertices[1];
}

display_point_t display_triangle_vertex_c(const display_triangle_t* t) {
	return t->vertices[2];
}

// display_triangle_sort_vertices_y sorts the display_triangle's vertices by their y-components.
void display_triangle_sort_vertices_y(display_triangle_t* t) {
	insertion_sort(t->vertices, 3, sizeof(display_point_t), &display_point_less_y);
}

void display_triangle_draw(const display_triangle_t* t) {
	draw_line(display_triangle_vertex_a(t), display_triangle_vertex_b(t), t->border);
	draw_line(display_triangle_vertex_b(t), display_triangle_vertex_c(t), t->border);
	draw_line(display_triangle_vertex_c(t), display_triangle_vertex_a(t), t->border);
}

void display_triangle_render_vertices(const display_triangle_t* t) {
	for (int j = 0; j < 3; j++) {
		display_rectangle_t r = {
			.point = t->vertices[j],
			.w = VERTEX_RECT_WIDTH_PX,
			.h = VERTEX_RECT_WIDTH_PX,
			.fill = RED,
			.border = 0,
		};
		display_rectangle_draw(&r);
	}
}

void display_triangle_render_wireframe(display_triangle_t* t) {
	t->border = GREEN;
	display_triangle_draw(t);
}

void display_triangle_render_vertex_wireframe(display_triangle_t* t) {
	display_triangle_render_wireframe(t);
	display_triangle_render_vertices(t);
}

/*
display_triangle_b_hyp_intercept returns the display_point at which a horizontal line projected from
the triangle's middle vertex (by y-value), b, will intercept the hypotenuse, ac, producing two
triangles with a flat bottom and flat top, respectively. The input triangle's vertices must be
sorted by y-value.

                /|
               / |
              /  |
             ----|  <- intercept
             \ . |
              -\ |
                -\
*/
display_point_t display_triangle_b_hyp_intercept(const display_triangle_t* t) {
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);
	display_point_t intercept = {
		.x = ((c.x - a.x) * (b.y - a.y)) / (c.y - a.y) + a.x,
		.y = b.y,
	};

	return intercept;
}

/*
display_triangle_fill_flat_bottom renders a filled, flat-bottomed triangle by scanning from top to
bottom and calculating the difference in the x positions of the opposing slopes.

             a
            / \
        l  /   \  r
          /_____\
         b       c
*/
void display_triangle_fill_flat_bottom(const display_triangle_t* t) {
	// Calculate the change in x with respect to y (inverse gradient) for both sloped sides of the
	// flat-bottomed triangle. We know that y (representing the current scan line) will increase
	// monotonically – the change in x is our unknown.
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);
	float inv_m_left = display_point_inv_gradient(a, b);
	float inv_m_right = display_point_inv_gradient(c, a);
	float x_start = a.x;
	float x_end = x_start;
	int max_width = abs(c.x - b.x);

	// Fill the triangle from top to bottom.
	for (int y = a.y; y <= c.y; y++) {
		draw_line(
			(display_point_t){(int)x_start, y},
			(display_point_t){(int)x_end, y},
			t->fill
		);

		x_start += inv_m_left;
		x_end += inv_m_right;
		// Prevent x_start from escaping the bounds of the triangle when inv_m is large.
		if (fabsf(x_end - x_start) > max_width) {
			x_start = b.x;
			x_end = c.x;
		}
	}
}

/*
display_triangle_fill_flat_top renders a filled, flat-topped triangle by scanning from bottom to top
and calculating the difference in the x positions of the opposing slopes.

		 a _____ b
		  \     /
		l  \   /  r
			\ /
			 c
*/
void display_triangle_fill_flat_top(const display_triangle_t* t) {
	// Calculate the change in x with respect to y (inverse gradient) for both sloped sides of the
	// flat-topped triangle. We know that y (representing the current scan line) will decrease
	// monotonically – the change in x is our unknown.
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);
	float inv_m_left = display_point_inv_gradient(c, a);
	float inv_m_right = display_point_inv_gradient(b, c);
	float x_start = c.x;
	float x_end = x_start;
	int max_width = abs(b.x - a.x);

	// Fill the triangle from bottom to top. This loop draws one line fewer than
	// fill_flat_bottom_triangle to avoid double-rendering the join between top and bottom.
	for (int y = t->vertices[2].y; y > (int)t->vertices[0].y; y--) {
		draw_line(
			(display_point_t){(int)x_start, y},
			(display_point_t){(int)x_end, y},
			t->fill
		);

		// We're moving "against" the gradient, so subtract m on each iteration.
		x_start -= inv_m_left;
		x_end -= inv_m_right;
		// Prevent x_start from escaping the bounds of the triangle when inv_m is large.
		if (fabsf(x_end - x_start) > max_width) {
			x_start = a.x;
			x_end = b.x;
		}
	}
}

void display_triangle_fill(display_triangle_t* t) {
	display_triangle_sort_vertices_y(t);
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);

	// If the input triangle is already flat-topped or flat-bottomed, perform only the required
	// operation. Otherwise, attempting to fill the zero-height opposite half will cause a zero
	// division error.
	if (a.y == b.y) {
		display_triangle_fill_flat_top(t);
		return;
	}
	if (b.y == c.y) {
		display_triangle_fill_flat_bottom(t);
		return;
	}

	// Calculate the point at the intercept of the horizontal line originating at b with ac, using
	// triangle similarity.
	display_point_t m = display_triangle_b_hyp_intercept(t);
	display_triangle_t flat_bottom = {
		.vertices = {a, b, m},
		.fill = t->fill
	};
	display_triangle_t flat_top = {
		.vertices = {b, m, c},
		.fill = t->fill
	};

	display_triangle_fill_flat_bottom(&flat_bottom);
	display_triangle_fill_flat_top(&flat_top);
}

void display_triangle_fill_wireframe(display_triangle_t* t) {
	display_triangle_fill(t);
	display_triangle_draw(t);
}

// void render_triangle_textured(triangle_t t) {
// 	texture_triangle(t);
// }

// void render_triangle_textured_wireframe(triangle_t t) {
// 	texture_triangle(t);
// 	display_triangle_draw(t, BLACK);
// }

void render_triangle(const triangle_t* t) {
	display_triangle_t dt = new_display_triangle_from_triangle(t);
	switch (g_render_mode) {
	case RENDER_MODE_VERTEX:
		display_triangle_render_vertices(&dt);
		break;
	case RENDER_MODE_WIREFRAME:
		display_triangle_render_wireframe(&dt);
		break;
	case RENDER_MODE_VERTEX_WIREFRAME:
		display_triangle_render_vertex_wireframe(&dt);
		break;
	case RENDER_MODE_FILL:
		display_triangle_fill(&dt);
		break;
	// case RENDER_MODE_TEXTURE:
	// 	return &render_triangle_textured;
	// 	break;
	// case RENDER_MODE_TEXTURE_WIREFRAME:
	// 	return &render_triangle_textured_wireframe;
	// 	break;
	default:
		display_triangle_fill_wireframe(&dt);
	}
}

// render_color_buffer copies the contents to the global color buffer to the SDL texture.
void render_color_buffer(void) {
	SDL_UpdateTexture(
		g_color_buffer_texture,
		NULL,
		g_color_buffer,
		(g_window_width * sizeof(color_t))
	);
	SDL_RenderCopy(
		g_renderer,
		g_color_buffer_texture,
		NULL,
		NULL
	);
}

void clear_color_buffer(color_t color) {
	int pixels = g_window_height * g_window_width;
	for (int i = 0; i < pixels; i++) {
		g_color_buffer[i] = color;
	}
}

void destroy_window(void) {
	SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
}
