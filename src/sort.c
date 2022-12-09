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
			memmove(j, j - obj_size, obj_size); // slide any element greater than the current one up one space
		}
		memcpy(j, cur_elem, obj_size); // copy the current element into its final position
	}

	free(cur_elem);
}
