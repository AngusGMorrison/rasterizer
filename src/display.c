#include "display.h"
#include "swap.h"

const int FPS = 30;
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

// display_point_t is a 2D integer vector whose x and y component correspond directly to pixels on
// the screen.
typedef struct display_point_t {
	int x, y;
} display_point_t;

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

// display_triangle_t encapsulates the data required to render a triangle to the screen. It differs
// from triangle_t in that its vertices are integer points corresponding to pixels on-screen.
typedef struct display_triangle_t {
	display_point_t vertices[3];
	color_t fill;
	color_t border;
	tex2_t tex_coords[3];
} display_triangle_t;

display_triangle_t new_display_triangle_from_triangle(const triangle_t* t) {
	return (display_triangle_t) {
		.vertices = {
			new_display_point_from_vec2(triangle_vertex_a(t)),
			new_display_point_from_vec2(triangle_vertex_b(t)),
			new_display_point_from_vec2(triangle_vertex_c(t)),
		},
		.fill = t->fill,
		.border = DEFAULT_BORDER_COLOR,
		.tex_coords = {
			triangle_tex_a(t),
			triangle_tex_b(t),
			triangle_tex_c(t),
		}
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

// Insertion sort a triangle's vertices and their corresponding tex_coords by their y-coordinates.
void display_triangle_sort_vertices_by_y(display_triangle_t* t) {
	for (int i = 1; i < 3; i++) {
		int j;
		display_point_t vert = t->vertices[i];
		tex2_t tex = t-> tex_coords[i];
		for (j = i; j > 0 && vert.y < t->vertices[j-1].y; j--) {
			t->vertices[j] = t->vertices[j-1];
			t->tex_coords[j] = t->tex_coords[j-1];
		}
		t->vertices[j] = vert;
		t->tex_coords[j] = tex;
	}
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

vec2_t new_vec2_from_display_point(display_point_t dp) {
	return (vec2_t) {
		.x = dp.x,
		.y = dp.y,
	};
}

// Return the barycentric weights for point p within the given triangle, t.
vec3_t display_triangle_barycentric_weights(const display_triangle_t* t, vec2_t p) {
	// Find the vectors between each vertex and point p.
	vec2_t a = new_vec2_from_display_point(display_triangle_vertex_a(t));
	vec2_t b = new_vec2_from_display_point(display_triangle_vertex_b(t));
	vec2_t c = new_vec2_from_display_point(display_triangle_vertex_c(t));
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t pb = vec2_sub(b, p);
	vec2_t pc = vec2_sub(c, p);

	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);
	float area_parallelogram_pcb = (pc.x * pb.y - pc.y * pb.x);
	float area_parallelogram_acp = (ac.x * ap.y - ac.y * ap.x);
	printf("a: {%f, %f}, b: {%f, %f}, c: {%f, %f}\n", a.x, a.y, b.x, b.y, c.x, c.y);
	printf("abc: %f, pbc: %f, acp: %f\n", area_parallelogram_abc, area_parallelogram_pcb, area_parallelogram_acp);
	float alpha = area_parallelogram_pcb / area_parallelogram_abc;
	float beta = area_parallelogram_acp / area_parallelogram_abc;
	float gamma = 1.0 - alpha - beta;

	return (vec3_t) { alpha, beta, gamma };
}

// display_rectangle_t encapsulates the data required to render a rectangle to the screen.
typedef struct display_rectangle_t {
	display_point_t point;
	int w, h;
	color_t fill;
	color_t border;
} display_rectangle_t;

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

void draw_pixel(display_point_t p, color_t color) {
	if (p.x < 0 || p.x >= g_window_width || p.y < 0 || p.y >= g_window_height) {
		return;
	}

	g_color_buffer[g_window_width * p.y + p.x] = color;
}

void draw_texel(display_point_t p, const display_triangle_t* t, const uint32_t* texture) {
	vec3_t weights = display_triangle_barycentric_weights(t, new_vec2_from_display_point(p));
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	tex2_t uv_a = t->tex_coords[0];
	tex2_t uv_b = t->tex_coords[1];
	tex2_t uv_c = t->tex_coords[2];

	float interpolated_u = uv_a.u * alpha + uv_b.u * beta + uv_c.u * gamma;
	float interpolated_v = uv_a.v * alpha + uv_b.v * beta + uv_c.v * gamma;
	if (interpolated_u > 1 || interpolated_v > 1) {
		return;
	}

	int texture_x = abs((int)(interpolated_u * texture_width));
	int texture_y = abs((int)(interpolated_v * texture_height));
	if (texture_x >= texture_width || texture_y >= texture_height) {
		return;
	}
	draw_pixel(p, texture[texture_width * texture_y + texture_x]);
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

bool display_triangle_has_zero_height(const display_triangle_t* t) {
	return t->vertices[0].y == t->vertices[1].y && t->vertices[1].y == t->vertices[2].y;
}

// display_triangle_fill fills the given display triangle by sorting its vertices by their
// y-coordinates, then scanning from top to bottom. It draws increasingly wide lines until it
// reaches the middle vertex, which is the triangle's widest point. It then draws increasingly
// narrow lines until reaching the bottom of the triangle.
void display_triangle_fill(display_triangle_t* t) {
	if (display_triangle_has_zero_height(t)) { // triangle is too small to render
		return;
	}

	// Calculate the change in x with respect to y (inverse gradient) for both opposing sides of the
	// triangle. We know that y (representing the current scan line) will increase monotonically –
	// the change in x is our unknown.
	display_triangle_sort_vertices_by_y(t);
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);
	display_point_t m = display_triangle_b_hyp_intercept(t); // the point at which a line projected horizontally from b intercepts ac
	float inv_m_ab = display_point_inv_gradient(a, b);
	float inv_m_bc = display_point_inv_gradient(b, c);
	float inv_m_ca = display_point_inv_gradient(c, a);

	// Fill the triangle from the top to the widest point, at vertex b.
	for (int y = a.y; y < b.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_ab;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		draw_line(
			(display_point_t){x_start, y},
			(display_point_t){x_end, y},
			t->fill
		);
	}

	// Fill the triangle from the widest point, at vertex b, to the bottom.
	for (int y = b.y; y <= c.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_bc;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		draw_line(
			(display_point_t){x_start, y},
			(display_point_t){x_end, y},
			t->fill
		);
	}
}

void display_triangle_fill_wireframe(display_triangle_t* t) {
	display_triangle_fill(t);
	display_triangle_draw(t);
}

// display_triangle_texture textures the given display triangle by sorting its vertices by their
// y-coordinates, then scanning from top to bottom. It textures pixels in increasingly wide lines
// until it reaches the middle vertex, which is the triangle's widest point. It then textures
// increasingly narrow lines until reaching the bottom of the triangle.
//
//             a
//            / \
//           /   \
//          /     \
//         b_----- \ m
//           \_     \
//             \_    \
//               \_   \
//                 \_  \
//                   \_ \
//                      c
//
void display_triangle_texture(display_triangle_t* t, const uint32_t* texture) {
	if (display_triangle_has_zero_height(t)) { // triangle is too small to render
		return;
	}

	// Calculate the change in x with respect to y (inverse gradient) for both opposing sides of the
	// triangle. We know that y (representing the current scan line) will increase monotonically –
	// the change in x is our unknown.
	display_triangle_sort_vertices_by_y(t);
	display_point_t a = display_triangle_vertex_a(t);
	display_point_t b = display_triangle_vertex_b(t);
	display_point_t c = display_triangle_vertex_c(t);
	display_point_t m = display_triangle_b_hyp_intercept(t); // the point at which a line projected horizontally from b intercepts ac
	float inv_m_ab = display_point_inv_gradient(a, b);
	float inv_m_bc = display_point_inv_gradient(b, c);
	float inv_m_ca = display_point_inv_gradient(c, a);

	// Texture the triangle from the top to the widest point, at vertex b.
	for (int y = a.y; y < b.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_ab;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		if (x_end < x_start) { // may occur due to the rotation of the face
			swap_ints(&x_start, &x_end);
		}
		for (int x = x_start; x <= x_end; x++) {
			draw_texel((display_point_t){x, y}, t, texture);
		}
	}

	// Texture the triangle from the widest point, at vertex b, to the bottom.
	for (int y = b.y; y <= c.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_bc;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		if (x_end < x_start) { // may occur due to the rotation of the face
			swap_ints(&x_start, &x_end);
		}
		for (int x = x_start; x <= x_end; x++) {
			draw_texel((display_point_t){x, y}, t, texture);
		}
	}
}

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
	case RENDER_MODE_TEXTURE:
		display_triangle_texture(&dt, g_mesh_texture);
		break;
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
