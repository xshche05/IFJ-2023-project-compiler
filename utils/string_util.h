#ifndef IFJ_PRJ_STRING_UTIL_H
#define IFJ_PRJ_STRING_UTIL_H

#define STRING_INIT_SIZE 8

typedef struct {
    char *str;      // string
    int length;     // length of string
    int allocated;  // allocated memory
} string_t;


string_t *string_init();

void string_dtor(string_t *string);

int string_add_char(string_t *string, char c);

int string_add_string(string_t *dest, string_t *source);

int string_extend_memory(string_t *string, int add_size);

int string_assign(string_t *dest, string_t *source);

int string_assign_cstr(string_t *dest, char *source);

string_t *string_copy(string_t *source);


#endif //IFJ_PRJ_STRING_UTIL_H
