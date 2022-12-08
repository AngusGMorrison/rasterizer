#ifndef ARRAY_H
#define ARRAY_H

#define array_push(array, value)                                              \
    do {                                                                      \
        (array) = array_hold((array), 1, sizeof(*(array)));                   \
        (array)[array_len(array) - 1] = (value);                           \
    } while (0);

void* array_hold(void* array, int count, size_t item_size);
int array_len(void* array);
void* array_clear(void* array, size_t item_size);
void array_free(void* array);

#endif
