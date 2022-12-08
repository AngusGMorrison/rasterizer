#ifndef MUST_H
#define MUST_H

#include <stdio.h>
#include <stdlib.h>

FILE* must_fopen(char* path, char* mode);
void* must_malloc(size_t t);

#endif