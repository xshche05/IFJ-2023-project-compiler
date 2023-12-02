//
// Created by Spagetik on 11.10.2023.
//

#include "lists.h"
#include <stdlib.h>
#include <string.h>

dynamic_array_t *dynamic_array_ctor() {
    dynamic_array_t *array = malloc(sizeof(dynamic_array_t));
    array->array = malloc(sizeof(void *) * 10);
    array->size = 0;
    array->allocated = 10;
    return array;
}

void dynamic_array_dtor(dynamic_array_t *array) {
    if (array == NULL) {
        return;
    }
    for (int i = 0; i < array->size; i++) {
        free(array->array[i]);
    }
    free(array->array);
    free(array);
    array = NULL;
}

int dynamic_array_add(dynamic_array_t *array, void *item) {
    if (array == NULL) {
        return -1;
    }
    if (array->size == array->allocated) {
        void *tmp = realloc(array->array, sizeof(void *) * array->allocated * 2);
        if (tmp == NULL) {
            return -1;
        }
        array->array = tmp;
        array->allocated *= 2;
    }
    array->array[array->size] = item;
    array->size++;
    return 0;
}

int dynamic_array_del(dynamic_array_t *array, int index) {
    if (array == NULL) {
        return -1;
    }
    if (index < 0 || index >= array->size) {
        return -1;
    }
    for (int i = index; i < array->size - 1; i++) {
        array->array[i] = array->array[i + 1];
    }
    array->size--;
    return 0;
}

void* dynamic_array_get(dynamic_array_t *array, int index) {
    if (array == NULL) {
        return NULL;
    }
    if (index < 0 || index >= array->size) {
        return NULL;
    }
    return array->array[index];
}

void dynamic_add_unique_cstr(dynamic_array_t *array, char *item) {
    if (array == NULL) {
        return;
    }
    for (int i = 0; i < array->size; i++) {
        if (strcmp((char*)dynamic_array_get(array, i), item) == 0) {
            return;
        }
    }
    dynamic_array_add(array, item);
}

const struct dynamic_array_interface DynamicArray = {
        .ctor = dynamic_array_ctor,
        .dtor = dynamic_array_dtor,
        .add = dynamic_array_add,
        .add_unique_cstr = dynamic_add_unique_cstr,
        .del = dynamic_array_del,
        .get = dynamic_array_get
};

