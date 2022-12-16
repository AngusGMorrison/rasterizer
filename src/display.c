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

void draw_pixel(int x, int y, color_t color) {
	if (x < 0 || x >= g_window_width || y < 0 || y >= g_window_height) {
		return;
	}

	g_color_buffer[g_window_width * y + x] = color;
}

void draw_texel(int x, int y, const triangle_t* t, const uint32_t* texture) {
	// Calculate the UV coordinates for the point based on its position relative to the vertices of
	// the triangle.
	vec3_t weights = triangle_barycentric_weights(t, (vec2_t){x, y});
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;
	tex2_t uv_a = t->tex_coords[0];
	tex2_t uv_b = t->tex_coords[1];
	tex2_t uv_c = t->tex_coords[2];
	float interpolated_u = uv_a.u * alpha + uv_b.u * beta + uv_c.u * gamma;
	float interpolated_v = uv_a.v * alpha + uv_b.v * beta + uv_c.v * gamma;
	
	// Map the UV coordinates to a pixel in the texture.
	int texture_x = abs((int)(interpolated_u * texture_width));
	int texture_y = abs((int)(interpolated_v * texture_height));
	if (texture_x >= texture_width || texture_y >= texture_height) {
		return;
	}
	draw_pixel(x, y, texture[texture_width * texture_y + texture_x]);
}

void draw_line(vec2_t a, vec2_t b, color_t color) {
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
		draw_pixel(round(cur_x), round(cur_y), color);
		cur_x += x_inc;
		cur_y += y_inc;
	}
}

void draw_rectangle(vec2_t p, int w, int h, color_t color) {
	for (int i = 0; i < w; i++) {
		int cur_x = p.x + i;
		for (int j = 0; j < h; j++) {
			int cur_y = p.y + j;
			draw_pixel(cur_x, cur_y, color);
		}
	}
}

void draw_triangle(const triangle_t* t) {
	draw_line(triangle_vertex_a(t), triangle_vertex_b(t), t->border);
	draw_line(triangle_vertex_b(t), triangle_vertex_c(t), t->border);
	draw_line(triangle_vertex_c(t), triangle_vertex_a(t), t->border);
}

void render_triangle_vertices(const triangle_t* t) {
	for (int j = 0; j < 3; j++) {
		draw_rectangle(t->vertices[j], VERTEX_RECT_WIDTH_PX, VERTEX_RECT_WIDTH_PX, DEFAULT_VERTEX_COLOR);
	}
}

void wireframe_triangle(triangle_t* t) {
	t->border = GREEN;
	draw_triangle(t);
}

void wireframe_triangle_with_vertices(triangle_t* t) {
	wireframe_triangle(t);
	render_triangle_vertices(t);
}

// fill_triangle fills the given display triangle by sorting its vertices by their
// y-coordinates, then scanning from top to bottom. It draws increasingly wide lines until it
// reaches the middle vertex, which is the triangle's widest point. It then draws increasingly
// narrow lines until reaching the bottom of the triangle.
void fill_triangle(triangle_t* t) {
	if (!triangle_is_renderable(t)) { // triangle is too small to render
		return;
	}

	// Calculate the change in x with respect to y (inverse gradient) for both opposing sides of the
	// triangle. We know that y (representing the current scan line) will increase monotonically –
	// the change in x is our unknown.
	triangle_sort_vertices_by_y(t);
	vec2_t a = triangle_vertex_a(t);
	vec2_t b = triangle_vertex_b(t);
	vec2_t c = triangle_vertex_c(t);
	vec2_t m = triangle_b_hyp_intercept(t); // the point at which a line projected horizontally from b intercepts ac
	float inv_m_ab = vec2_inv_gradient(a, b);
	float inv_m_bc = vec2_inv_gradient(b, c);
	float inv_m_ca = vec2_inv_gradient(c, a);

	// Fill the triangle from the top to the widest point, at vertex b.
	for (int y = a.y; y < b.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_ab;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		draw_line(
			(vec2_t){x_start, y},
			(vec2_t){x_end, y},
			t->fill
		);
	}

	// Fill the triangle from the widest point, at vertex b, to the bottom.
	for (int y = b.y; y <= c.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_bc;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		draw_line(
			(vec2_t){x_start, y},
			(vec2_t){x_end, y},
			t->fill
		);
	}
}

void fill_triangle_with_wireframe(triangle_t* t) {
	fill_triangle(t);
	draw_triangle(t);
}

// texture_triangle textures the given display triangle by sorting its vertices by their
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
void texture_triangle(triangle_t* t, const uint32_t* texture) {
	// Calculate the change in x with respect to y (inverse gradient) for both opposing sides of the
	// triangle. We know that y (representing the current scan line) will increase monotonically –
	// the change in x is our unknown.
	triangle_sort_vertices_by_y(t);
	vec2_t a = triangle_vertex_a(t);
	vec2_t b = triangle_vertex_b(t);
	vec2_t c = triangle_vertex_c(t);
	vec2_t m = triangle_b_hyp_intercept(t); // the point at which a line projected horizontally from b intercepts ac
	float inv_m_ab = vec2_inv_gradient(a, b);
	float inv_m_bc = vec2_inv_gradient(b, c);
	float inv_m_ca = vec2_inv_gradient(c, a);

	// Texture the triangle from the top to the widest point, at vertex b.
	for (int y = a.y; y < b.y; y++) {
		int x_start = b.x + (y - b.y) * inv_m_ab;
		int x_end = m.x + (y - m.y) * inv_m_ca;
		if (x_end < x_start) { // may occur due to the rotation of the face
			swap_ints(&x_start, &x_end);
		}
		for (int x = x_start; x <= x_end; x++) {
			draw_texel(x, y, t, texture);
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
			draw_texel(x, y, t, texture);
		}
	}
}

void texture_triangle_with_wireframe(triangle_t* t, const uint32_t* texture) {
	t->border = WHITE;
	texture_triangle(t, texture);
	draw_triangle(t);
}

// render_triangle renders the given triangle to the screen based on the current rendering mode.
void render_triangle(triangle_t* t) {
	if (!triangle_is_renderable(t)) {
		return;
	}

	// Truncating floating point vertices to integers at the outset avoids a host of downstream
	// floating-point issues when drawing to the screen.
	triangle_truncate_vertices(t);
	switch (g_render_mode) {
	case RENDER_MODE_VERTEX:
		render_triangle_vertices(t);
		break;
	case RENDER_MODE_WIREFRAME:
		wireframe_triangle(t);
		break;
	case RENDER_MODE_VERTEX_WIREFRAME:
		wireframe_triangle_with_vertices(t);
		break;
	case RENDER_MODE_FILL:
		fill_triangle(t);
		break;
	case RENDER_MODE_TEXTURE:
		texture_triangle(t, g_mesh_texture);
		break;
	case RENDER_MODE_TEXTURE_WIREFRAME:
		return texture_triangle_with_wireframe(t, g_mesh_texture);
		break;
	default:
		fill_triangle_with_wireframe(t);
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
