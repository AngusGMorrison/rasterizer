// texture.h provides a structure for working with UV-mapped textures.
#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#include "color.h"
#include "upng.h"

extern int g_texture_width;
extern int g_texture_height;
extern color_t* g_mesh_texture;
extern upng_t* png_texture;

typedef struct tex2_t {
	float u;
	float v;
} tex2_t;

void load_png_texture(const char* filename);

#endif