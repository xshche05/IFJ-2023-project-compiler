/*
 * IFJ Project 2023
 * Implementation of module for safe memory management
 * Author: Kirill Shchetiniuk (xshche05)
 */

#ifndef IFJ_PRJ_MEMORY_H
#define IFJ_PRJ_MEMORY_H

#include <stdlib.h>

void *safe_malloc(size_t size);

void *safe_realloc(void *ptr, size_t size);

void safe_free(void *ptr);

void safe_exit(int code);

void free_all();

#endif //IFJ_PRJ_MEMORY_H
