//
// Created by Spagetik on 11.10.2023.
//

#ifndef IFJ_PRJ_LISTS_H
#define IFJ_PRJ_LISTS_H

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
    int (*del)(dynamic_array_t *array, int index);
    void *(*get)(dynamic_array_t *array, int index);
};

#endif //IFJ_PRJ_LISTS_H
