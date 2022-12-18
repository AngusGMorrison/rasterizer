#include <stdio.h>
#include <stdlib.h>

#include "texture.h"

int g_texture_width = 64;
int g_texture_height = 64;

color_t* g_mesh_texture = NULL;
upng_t* png_texture = NULL;

void load_png_texture(const char* filename) {
	png_texture = upng_new_from_file(filename);
	if (png_texture == NULL) {
		fprintf(stderr, "failed to load PNG texture\n");
		abort();
	}

	upng_decode(png_texture);
	upng_error err = upng_get_error(png_texture);
	if (err != UPNG_EOK) {
		fprintf(stderr, "failed to load PNG texture: code %d\n", err);
		abort();
	}

	g_mesh_texture = (color_t*)upng_get_buffer(png_texture);
	g_texture_width = upng_get_width(png_texture);
	g_texture_height = upng_get_height(png_texture);
}