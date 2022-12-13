#include <stdio.h>
#include "must.h"

FILE* must_fopen(const char* path, char* mode) {
	FILE* f = fopen(path, mode);
	if (f == NULL) {
		fprintf(stderr, "failed to open %s in mode %s\n", path, mode);
		abort();
	}

	return f;
}

void* must_malloc(size_t t) {
	void* p = malloc(t);
	if (p == NULL) {
		fprintf(stderr, "failed to allocate %zu bytes\n", t);
		abort();
	}

	return p;
}
