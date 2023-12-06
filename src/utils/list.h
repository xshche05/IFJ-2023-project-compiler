/*
 * IFJ Project 2023
 * Implementation of dynamic array
 * Author: Kirill Shchetiniuk (xshche05)
 */

#ifndef IFJ_PRJ_LIST_H
#define IFJ_PRJ_LIST_H

#include <stdbool.h>

typedef struct {
    void** array;
    int size;
    int allocated;
} dynamic_array_t;

extern const struct dynamic_array_interface DynamicArray;

struct dynamic_array_interface {
    dynamic_array_t *(*ctor)(void);
    void (*dtor)(dynamic_array_t *array);
    int (*add)(dynamic_array_t *array, void *item);
    void (*add_unique_cstr)(dynamic_array_t *array, char *item);
    int (*del)(dynamic_array_t *array, int index);
    void *(*get)(dynamic_array_t *array, int index);
    int (*del_item)(dynamic_array_t *array, void *item);
    bool (*is_in_array)(dynamic_array_t *array, void *item);
};

#endif //IFJ_PRJ_LIST_H
