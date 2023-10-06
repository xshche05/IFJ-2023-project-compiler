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

file_t *file_init();

void file_dtor(file_t *file);

int file_add_line(file_t *file, string_t *line);

int file_load(char *file_name, file_t *file);

char file_getc(file_t *file);

void file_print(file_t *file);

#endif //IFJ_PRJ_FILE_UTIL_H
