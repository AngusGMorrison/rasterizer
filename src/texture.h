#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct tex2_t {
	float u;
	float v;
} tex2_t;

extern const int texture_width;
extern const int texture_height;
extern const uint8_t REDBRICK_TEXTURE[];

#endif