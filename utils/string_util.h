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

/**
 * @brief This function initializes string.
 *
 * @return Pointer to string_t structure.
 */
string_t *string_init();

/**
 * @brief This function destroys string.
 *
 * @param string - Pointer to string_t structure.
 * @return Pointer to string_t structure.
 */
void string_dtor(string_t *string);

/**
 * @brief This function adds char to string.
 *
 * @param string - Pointer to string_t structure.
 * @param c - Char to add.
 * @return 0 if success, -1 otherwise.
 */
int string_add_char(string_t *string, char c);

/**
* @brief This function adds string to string.
*
* @param dest - Pointer to destination string_t structure.
* @param source - Pointer to source string_t structure.
* @return 0 if success, -1 otherwise.
*/
int string_add_string(string_t *dest, string_t *source);

/**
 * @brief This function extends memory for string.
 *
 * @param string - Pointer to string_t structure.
 * @param add_size - Size of memory to add.
 * @return 0 if success, -1 otherwise.
 */
int string_extend_memory(string_t *string, int add_size);

/**
 * @brief This function assigns string to string.
 *
 * @param dest - Pointer to destination string_t structure.
 * @param source - Pointer to source string_t structure.
 * @return 0 if success, -1 otherwise.
 */
int string_assign(string_t *dest, string_t *source);

/**
 * @brief This function assigns c string to string.
 *
 * @param dest - Pointer to destination string_t structure.
 * @param source - Pointer to source c string.
 * @return 0 if success, -1 otherwise.
 */
int string_assign_cstr(string_t *dest, char *source);

/**
 * @brief This function copies string.
 *
 * @param source - Pointer to source string_t structure.
 * @return Pointer to copied string_t structure.
 */
string_t *string_copy(string_t *source);

int string_clear(string_t *string);


#endif //IFJ_PRJ_STRING_UTIL_H
