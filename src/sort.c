#include <stdlib.h>

#include "sort.h"

void swap(char* i, char* j, char* temp, size_t obj_size) {
	memcpy(temp, i, obj_size);
	memcpy(i, j, obj_size);
	memcpy(j, temp, obj_size);
}

char* partition(char* lo, char* hi, size_t obj_size, comparator less) {
	char* pivot = lo;
	char* temp = must_malloc(obj_size);

	char* i = lo;
	char* j = hi + obj_size;
	while (true) {
		i += obj_size;
		while (less(i, pivot)) {
			if (i == hi) {
				break;
			}
			i += obj_size;
		}

		j -= obj_size;
		while (less(pivot, j)) {
			if (j == lo) {
				break;
			}
			j -= obj_size;
		}

		if (i >= j) {
			break;
		}

		swap(i, j, temp, obj_size);
	}
	swap(lo, j, temp, obj_size);

	free(temp);

	return j;
}

void qsort_internal(char* lo, char* hi, size_t obj_size, comparator less) {
	if (lo >= hi) {
		return;
	}

	char* pivot = partition(lo, hi, obj_size, less);
	qsort_internal(lo, pivot - obj_size, obj_size, less);
	qsort_internal(pivot + obj_size, hi, obj_size, less);
}

// Returns an integer in the range [lo,hi).
int randIntInRange(int lo, int hi) {
	return lo + rand() % (hi - lo);
}

void shuffle(char* arr, size_t len, size_t obj_size) {
	char* temp = must_malloc(obj_size);
	for (int i = 0; i < len; i++) {
		int swapIdx = randIntInRange(i, len);
		char* swapPtr = arr + swapIdx * obj_size;
		swap(arr, swapPtr, temp, obj_size);
	}
	free(temp);
}

void quick_sort(void* arr, size_t len, size_t obj_size, comparator less) {
	char* charr = (char*)arr;
	char* hi = charr + ((len - 1) * obj_size); // index of the last element
	shuffle(charr, len, obj_size); // randomize the array to avoid worst-case performance
	qsort_internal(charr, hi, obj_size, less);
}
