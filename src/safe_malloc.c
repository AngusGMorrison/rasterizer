#include "safe_malloc.h"

void* safe_malloc(size_t t) {
  void* p = malloc(t);
  if (p == NULL) {
    fprintf(stderr, "failed to allocate %zu bytes\n", t);
    abort();
  }

  return p;
}