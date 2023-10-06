#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string_util.h"


string_t *string_init() {
    string_t *string = (string_t *) malloc(sizeof(string_t));
    if (string == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for string.\n");
        return NULL;
    }

    string->str = malloc(sizeof(char) * STRING_INIT_SIZE);
    if (string->str == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for string.\n");
        return NULL;
    }

    string->str[0] = '\0';
    string->length = 0;
    string->allocated = STRING_INIT_SIZE;

    return string;
}

void string_dtor(string_t *string) {
    free(string->str);
    free(string);
    string = NULL;
}

int string_extend_memory(string_t *string, int add_size) {
    char *new_string = realloc(string->str, sizeof(char) * (string->allocated + add_size * 3 / 2));
    if (new_string == NULL) {
        fprintf(stderr, "Error: Failed to reallocate memory for string.\n");
        return -1;
    }
    string->str = new_string;
    string->allocated += add_size * 3 / 2;
    return 0;
}

int string_add_char(string_t *string, char c) {
    if (string->length + 1 >= string->allocated) {
        if (string_extend_memory(string, 8) != 0) {
            return -1;
        }
    }
    string->str[string->length] = c;
    string->str[++string->length] = '\0';
    return 0;
}

int string_add_string(string_t *dest, string_t *source) {
    if (dest->length + source->length + 1 >= dest->allocated) {
        if (string_extend_memory(dest, source->length) != 0) {
            return -1;
        }
    }
    strcat(dest->str, source->str);
    dest->length += source->length;
    return 0;
}

int string_assign(string_t *dest, string_t *source) {
    if (dest->allocated < source->length + 1) {
        if (string_extend_memory(dest, source->length) != 0) {
            return -1;
        }
    }
    strcpy(dest->str, source->str);
    dest->length = source->length;
    return 0;
}

int string_assign_cstr(string_t *dest, char *source) {
    int length = strlen(source);
    if (dest->allocated < length + 1) {
        if (string_extend_memory(dest, length) != 0) {
            return -1;
        }
    }
    strcpy(dest->str, source);
    dest->length = length;
    return 0;
}

string_t *string_copy(string_t *source) {
    string_t *dest = string_init();
    if (dest == NULL) {
        return NULL;
    }
    if (string_assign(dest, source) != 0) {
        string_dtor(dest);
        return NULL;
    }
    return dest;
}