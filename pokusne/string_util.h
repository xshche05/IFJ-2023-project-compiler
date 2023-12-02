#ifndef IFJ_PRJ_STRING_UTIL_H
#define IFJ_PRJ_STRING_UTIL_H

#define STRING_INIT_SIZE 8

/**
 * @brief This structure represents string.
 *
 * @param str - String
 * @param length - Length of string
 * @param allocated - Allocated memory
 */
typedef struct {
    char *str;      // string
    int length;     // length of string
    int allocated;  // allocated memory
} string_t;

// Interface
extern const struct string_interface String;

/**
 * @brief Structure of string interface.
 *
 * @param ctor - Constructor
 * @param copy - Copy string
 * @param dtor - Destructor
 * @param cmp - Compare strings, return 0 if strings are equal, -1 if first string is "smaller", 1 if first string is "bigger"
 * @param clear - Clear string, set length to 0 and first char to '\0'
 * @param add_char - Add char to string
 * @param add_string - Add string to string, concatenate strings
 * @param assign - Assign string to string
 * @param assign_cstr - Assign c string to string
 */
struct string_interface {
    string_t *(*ctor)();
    string_t *(*copy)(string_t *source);
    void (*dtor)(string_t *string);
    int (*cmp)(string_t *string1, string_t *string2);
    int (*cmp_cstr)(string_t *string1, char *string2);
    int (*clear)(string_t *string);
    int (*add_char)(string_t *string, char c);
    int (*add_string)(string_t *dest, string_t *source);
    int (*add_cstr)(string_t *dest, char *source);
    int (*assign)(string_t *dest, string_t *source);
    int (*assign_cstr)(string_t *dest, char *source);
    int (*del_last_char)(string_t *string);
    string_t *(*create)(char *source);
};

#endif //IFJ_PRJ_STRING_UTIL_H
