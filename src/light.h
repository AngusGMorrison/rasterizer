#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

// light_t represents an ambient light source with direction only.
typedef struct light_t {
	vec3_t direction;
} light_t;

#endif