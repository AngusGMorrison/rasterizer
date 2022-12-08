#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "mesh_t.h"
#include "must.h"
#include "triangle.h"
#include "vector.h"

extern const int MAX_LINE;

int parse_obj_file(char* path, mesh_t* dest);

#endif