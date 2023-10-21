#ifndef IFJ_PRJ_FILE_UTIL_H
#define IFJ_PRJ_FILE_UTIL_H

#include <stdio.h>
#include "string_util.h"

/**
 * @brief This structure represents source code file.
 *
 * @param file_name - Name of file
 * @param lines - Array of lines
 * @param line_allocated - Number of allocated lines
 * @param line_count - Number of lines
 * @param current_line - Index of current line
 * @param current_position - Index of current position in current line
 */
typedef struct {
    char *file_name;
    string_t **lines;
    int line_allocated;
    int line_count;
    int current_line;
    int current_position;
} file_t;

extern file_t *file;

extern const struct file_interface File;

/**
 * @brief Structure of file interface.
 *
 * @param ctor - Constructor
 * @param dtor - Destructor
 * @param add_line - Add line to file, line must be allocated
 * @param from_file - Load text from file
 * @param from_stdin - Load text from stdin
 * @param line - Get current line (starts from 0)
 * @param column - Get current column (starts from 0)
 * @param getc - Get char from file and move pointer
 * @param back_step - Back step in file, move pointer back for one char
 * @param print_line - Print line
 * @param print - Print full file
 */
struct file_interface {
    int (*ctor)(void);
    void (*dtor)(void);
    int (*add_line)(string_t *line);
    int (*from_file)(char *file_name);
    int (*from_stdin)(void);
    int (*line)(void);
    int (*column)(void);
    char (*getc)(void);
    void (*back_step)(void);
    char* (*print_line)(void);
    void (*print)(void);
};

#endif //IFJ_PRJ_FILE_UTIL_H
