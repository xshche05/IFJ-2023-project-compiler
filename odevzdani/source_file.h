/*
 * IFJ Project 2023
 * Implementation of source file module for reading source code
 * Author: Kirill Shchetiniuk (xshche05)
 */


#ifndef IFJ_PRJ_SOURCE_FILE_H
#define IFJ_PRJ_SOURCE_FILE_H

#include <stdio.h>
#include "string_util.h"

/**
 * @brief This structure represents source code sourceFile.
 *
 * @param file_name - Name of sourceFile
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

extern file_t *sourceFile;

extern const struct file_interface SourceCode;

/**
 * @brief Structure of sourceFile interface.
 *
 * @param ctor - Constructor
 * @param dtor - Destructor
 * @param add_line - Add line to sourceFile, line must be allocated
 * @param from_file - Load text from sourceFile
 * @param from_stdin - Load text from stdin
 * @param line - Get current line (starts from 0)
 * @param column - Get current column (starts from 0)
 * @param getc - Get char from sourceFile and move pointer
 * @param back_step - Back step in sourceFile, move pointer back for one char
 * @param print_line - Print line
 * @param print - Print full sourceFile
 */
struct file_interface {
    int (*ctor)(void);
    void (*dtor)(void);
    int (*add_line)(string_t *line);
    int (*from_file)(char *file_name);
    int (*from_stdin)(void);
    int (*line)(void);
    int (*column)(void);
    char (*file_getc)(void);
    void (*back_step)(void);
    char* (*print_line)(void);
    void (*print)(void);
};

#endif //IFJ_PRJ_SOURCE_FILE_H
