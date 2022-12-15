#include "swap.h"

void swap_ints(int* f1, int* f2) {
	int temp = *f1;
	*f1 = *f2;
	*f2 = temp;
}
