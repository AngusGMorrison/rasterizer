// texture.h provides a structure for working with UV-mapped textures.
#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

extern const int texture_width;
extern const int texture_height;
extern const uint8_t REDBRICK_TEXTURE[];
extern uint32_t* g_mesh_texture;

typedef struct tex2_t {
	float u;
	float v;
} tex2_t;

#endif