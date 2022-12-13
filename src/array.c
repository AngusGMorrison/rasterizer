#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

// Returns the pointer to the array prelude containing its capacity and length.
#define ARRAY_RAW_DATA(array) ((int*)(array) - 2)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_LEN(array) (ARRAY_RAW_DATA(array)[1])

void* array_hold(void* array, int count, size_t item_size) {
	if (array == NULL) {
		// Initialize the array.
		int raw_size = (sizeof(int) * 2) + (item_size * count);
		int* base = (int*)malloc(raw_size);
		base[0] = count;  // capacity
		base[1] = count;  // len
		return base + 2;
	} else if (ARRAY_LEN(array) + count <= ARRAY_CAPACITY(array)) {
		// Push elements without reallocating.
		ARRAY_LEN(array) += count;
		return array;
	} else {
		// Reallocate the array to a new block of memory which is the greater of the requested
		// capacity or twice the current capacity.
		int needed_size = ARRAY_LEN(array) + count;
		int float_curr = ARRAY_CAPACITY(array) * 2;
		int capacity = needed_size > float_curr ? needed_size : float_curr;
		int len = needed_size;
		int raw_size = sizeof(int) * 2 + item_size * capacity;
		int* base = (int*)realloc(ARRAY_RAW_DATA(array), raw_size);
		base[0] = capacity;
		base[1] = len;
		return base + 2;
	}
}

int array_len(void* array) {
	return (array != NULL) ? ARRAY_LEN(array) : 0;
}

void* array_reset(void* array, size_t item_size) {
	if (array == NULL) {
		return NULL;
	}
	int* raw = ARRAY_RAW_DATA(array);
	raw[1] = 0; // reset len
	return array;
}

void array_free(void* array) {
	if (array != NULL) {
		free(ARRAY_RAW_DATA(array));
	}
}