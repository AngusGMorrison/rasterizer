#ifndef SORT_H
#define SORT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef bool(*comparator)(const void*, const void*);

void insertion_sort(void* arr, size_t len, size_t obj_size, comparator less);

#endif