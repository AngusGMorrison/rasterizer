// must.h provides wrapper functions that terminate the program if their underlying operation fails.
#ifndef MUST_H
#define MUST_H

#include <stdio.h>
#include <stdlib.h>

// Open the file at path or abort.
FILE* must_fopen(const char* path, char* mode);

// Allocate the requested memory or abort.
void* must_malloc(size_t t);

#endif