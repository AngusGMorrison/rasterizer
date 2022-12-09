#include "float.h"

const float EQUALITY_THRESHOLD = 1e-5;

bool float_approx_equal(float a, float b) {
	return fabsf(a - b) < EQUALITY_THRESHOLD;
}