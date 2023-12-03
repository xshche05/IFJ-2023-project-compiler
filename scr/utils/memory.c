//
// Created by Spagetik on 02-Dec-23.
//

#include "memory.h"
#include "lists.h"
#include <stdio.h>

dynamic_array_t *allocated_pointers = NULL;

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        safe_exit(99);
    }
    if (allocated_pointers == NULL) {
        allocated_pointers = DynamicArray.ctor();
    }
    DynamicArray.add(allocated_pointers, ptr);
    return ptr;
}

void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        safe_exit(99);
    }
    DynamicArray.del_item(allocated_pointers, ptr);
    DynamicArray.add(allocated_pointers, new_ptr);
    return new_ptr;
}

void safe_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    DynamicArray.del_item(allocated_pointers, ptr);
    free(ptr);
}

void free_all() {
    if (allocated_pointers == NULL) {
        return;
    }
    int size = allocated_pointers->size;
    for (int i = 0; i < size; i++) {
        free(allocated_pointers->array[i]);
    }
    for (int i = 0; i < size; i++) {
        DynamicArray.del(allocated_pointers, i);
    }
    DynamicArray.dtor(allocated_pointers);
    allocated_pointers = NULL;
}

void safe_exit(int code) {
    free_all();
    exit(code);
}
