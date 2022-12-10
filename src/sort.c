#include "sort.h"

// Generic insertion sort. Handles all input arrays as arrays of bytes, requiring the size of the
// elements to offset the correct number of bytes when iterating, comparing and swapping.
void insertion_sort(void* arr, size_t len, size_t obj_size, comparator less) {
	char* charr = (char*)arr; // convert arr to an array of bytes
	char* cur_elem = malloc(obj_size); // since we don't know its type, ther current element must also be stored as an array of bytes
	char* max = charr + (len * obj_size); // end of the array
	char* i;
	char* j;

	for (i = charr + obj_size; i < max; i += obj_size) {
		memcpy(cur_elem, i, obj_size); // save the current element
		for (j = i; j > charr && less(cur_elem, j - obj_size); j -= obj_size) {
			memcpy(j, j - obj_size, obj_size); // slide any element greater than the current one up one space
		}
		memcpy(j, cur_elem, obj_size); // copy the current element into its final position
	}

	free(cur_elem);
}

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

void quick_sort(void* arr, size_t len, size_t obj_size, comparator less) {
	char* charr = (char*)arr;
	char* hi = charr + ((len - 1) * obj_size); // index of the last element
	qsort_internal(charr, hi, obj_size, less);
	// TODO: Shuffle.
}
