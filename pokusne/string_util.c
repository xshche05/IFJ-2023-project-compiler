#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string_util.h"

/**
 * @brief This function initializes string.
 *
 * @return Pointer to string_t structure.
 */
static string_t *string_ctor() {
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

/**
 * @brief This function destroys string.
 *
 * @param string - Pointer to string_t structure.
 */
static void string_dtor(string_t *string) {
    if (string == NULL) return;
    if (string->str == NULL) return;
    free(string->str);
    free(string);
    string = NULL;
}

/**
 * @brief This function extends memory for string.
 *
 * @param string - Pointer to string_t structure.
 * @param add_size - Size of memory to add.
 * @return 0 if success, -1 otherwise.
 */
static int string_extend_memory(string_t *string, int add_size) {
    char *new_string = realloc(string->str, sizeof(char) * (string->allocated + add_size * 3 / 2));
    if (new_string == NULL) {
        fprintf(stderr, "Error: Failed to reallocate memory for string.\n");
        return -1;
    }
    string->str = new_string;
    string->allocated += add_size * 3 / 2;
    return 0;
}

/**
 * @brief This function adds char to string.
 *
 * @param string - Pointer to string_t structure.
 * @param c - Char to add.
 * @return 0 if success, -1 otherwise.
 */
static int string_add_char(string_t *string, char c) {
    if (string->length + 1 >= string->allocated) {
        if (string_extend_memory(string, 8) != 0) {
            return -1;
        }
    }
    string->str[string->length] = c;
    string->str[++string->length] = '\0';
    return 0;
}

/**
* @brief This function adds string to string.
*
* @param dest - Pointer to destination string_t structure.
* @param source - Pointer to source string_t structure.
* @return 0 if success, -1 otherwise.
*/
static int string_add_string(string_t *dest, string_t *source) {
    if (dest->length + source->length + 1 >= dest->allocated) {
        if (string_extend_memory(dest, source->length) != 0) {
            return -1;
        }
    }
    strcat(dest->str, source->str);
    dest->length += source->length;
    return 0;
}

/**
 * @brief This function assigns string to string.
 *
 * @param dest - Pointer to destination string_t structure.
 * @param source - Pointer to source string_t structure.
 * @return 0 if success, -1 otherwise.
 */
static int string_assign(string_t *dest, string_t *source) {
    if (dest->allocated < source->length + 1) {
        if (string_extend_memory(dest, source->length) != 0) {
            return -1;
        }
    }
    strcpy(dest->str, source->str);
    dest->length = source->length;
    return 0;
}

/**
 * @brief This function assigns c string to string.
 *
 * @param dest - Pointer to destination string_t structure.
 * @param source - Pointer to source c string.
 * @return 0 if success, -1 otherwise.
 */
static int string_assign_cstr(string_t *dest, char *source) {
    int length = (int)strlen(source);
    if (dest->allocated < length + 1) {
        if (string_extend_memory(dest, length) != 0) {
            return -1;
        }
    }
    strcpy(dest->str, source);
    dest->length = length;
    return 0;
}

/**
 * @brief This function copies string.
 *
 * @param source - Pointer to source string_t structure.
 * @return Pointer to copied string_t structure.
 */
static string_t *string_copy(string_t *source) {
    string_t *dest = string_ctor();
    if (dest == NULL) {
        return NULL;
    }
    if (string_assign(dest, source) != 0) {
        string_dtor(dest);
        return NULL;
    }
    return dest;
}

/**
 * @brief This function clears string. Set length to 0 and first char to '\0'.
 *
 * @param string - Pointer to string_t structure.
 * @return 0 if success, -1 otherwise.
 */
static int string_clear(string_t *string) {
    string->str[0] = '\0';
    string->length = 0;
    return 0;
}

/**
 * @brief This function compares two strings.
 *
 * @param string1 - Pointer to first string_t structure.
 * @param string2 - Pointer to second string_t structure.
 * @return 0 if strings are equal, -1 if first string is "smaller", 1 if first string is "bigger".
 */
static int string_compare(string_t *string1, string_t *string2) {
    return strcmp(string1->str, string2->str);
}

/**
 * @brief This function compares string_t and c string.
 *
 * @param string1 - Pointer to string_t structure.
 * @param string2 - Pointer to c string.
 * @return 1, -1 or 0 like strcmp.
 */
static int string_compare_cstr(string_t *string1, char *string2) {
    return strcmp(string1->str, string2);
}

/**
 * @brief This function adds c string to string_t.
 * @param dest - Pointer to destination string_t structure.
 * @param source - Pointer to source c string.
 * @return zero if success, non-zero otherwise.
 */
static int string_add_cstr(string_t *dest, char *source) {
    int length = (int)strlen(source);
    if (dest->length + length + 1 >= dest->allocated) {
        if (string_extend_memory(dest, length) != 0) {
            return -1;
        }
    }
    strcat(dest->str, source);
    dest->length += length;
    return 0;
}

/**
 * @brief This function creates string from c string.
 * @param source - Pointer to source c string.
 * @return pointer to string_t structure if success, NULL otherwise.
 */
static string_t *string_create(char *source) {
    string_t *string = string_ctor();
    if (string == NULL) {
        return NULL;
    }
    if (string_assign_cstr(string, source) != 0) {
        string_dtor(string);
        return NULL;
    }
    return string;
}

static int string_del_last_char(string_t *string) {
    if (string->length == 0) {
        return -1;
    }
    string->str[--string->length] = '\0';
    return 0;
}

const struct string_interface String = {
        .ctor = string_ctor,
        .copy = string_copy,
        .dtor = string_dtor,
        .clear = string_clear,
        .cmp = string_compare,
        .cmp_cstr = string_compare_cstr,
        .add_char = string_add_char,
        .del_last_char = string_del_last_char,
        .add_string = string_add_string,
        .add_cstr = string_add_cstr,
        .assign = string_assign,
        .assign_cstr = string_assign_cstr,
        .create = string_create,
};