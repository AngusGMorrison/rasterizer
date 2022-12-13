// array.h defines a dynamically sized array and the functions to operate on it.
#ifndef ARRAY_H
#define ARRAY_H

// Push an element onto the array. If sufficient space is unavailable, the array is transparently
// resized.
#define array_push(array, value)                                              \
    do {                                                                      \
        (array) = array_hold((array), 1, sizeof(*(array)));                   \
        (array)[array_len(array) - 1] = (value);                           \
    } while (0);

// array_hold takes no action or resizes the array based on its current capacity and the number and
// size of incoming elements.
void* array_hold(void* array, int count, size_t item_size);

int array_len(void* array);

// array_reset resets the array's length without freeing memory, effectively emptying the array.
void* array_reset(void* array, size_t item_size);

// array_free frees the memory associated with the array, after which it must not be used.
void array_free(void* array);

#endif
