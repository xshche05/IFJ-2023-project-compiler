#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_util.h"


file_t *file_init() {
    file_t *file = malloc(sizeof(file_t));
    if (file == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    file->file_name = NULL;
    file->lines = malloc(sizeof(string_t*) * 8);
    if (file->lines == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    file->line_allocated = 8;
    file->line_count = 0;
    file->current_line = 0;
    file->current_position = 0;
    return file;
}

void file_dtor(file_t *file) {
    if (file == NULL) {
        return;
    }
    for (size_t i = 0; i < file->line_count; i++) {
        String.dtor(file->lines[i]);
    }
    free(file->file_name);
    free(file->lines);
    free(file);
    file = NULL;
}

int file_add_line(file_t *file, string_t *line) {
    if (file == NULL || line == NULL) {
        return -1;
    }
    if (file->line_count == file->line_allocated) {
        void* tmp = realloc(file->lines, sizeof(string_t)*file->line_allocated*2);
        if (tmp == NULL) {
            return -1;
        }
        file->lines = tmp;
        file->line_allocated *= 2;
    }
    file->lines[file->line_count] = line;
    file->line_count++;
    return 0;
}

int file_load_from_file(char *file_name, file_t *file) {
    if (file == NULL) {
        return -1;
    }
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: file %s not found.\n", file_name);
        return -1;
    }
    string_t *line = String.ctor();
    if (line == NULL) {
        return -1;
    }
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            file_add_line(file, line);
            line = String.ctor();
            if (line == NULL) {
                return -1;
            }
            continue;
        }
        String.add_char(line, (char) c);
    }
    file_add_line(file, line);
    fclose(f);
    file->file_name = malloc(sizeof(char) * (strlen(file_name) + 1));
    strcpy(file->file_name, file_name);
    return 0;
}

char file_getc(file_t *file) {
    if (file == NULL) {
        return -1;
    }
    if (file->current_line >= file->line_count) {
        return -1;
    }
    if (file->current_position >= file->lines[file->current_line]->length) {
        file->current_line++;
        file->current_position = 0;
        return '\n';
    }
    char c = file->lines[file->current_line]->str[file->current_position];
    file->current_position++;
    return c;
}

void file_back_step(file_t *file) {
    if (file == NULL) {
        return;
    }
    if (file->current_position == 0) {
        file->current_line--;
        file->current_position = file->lines[file->current_line]->length;
        return;
    }
    file->current_position--;
}

int file_line(file_t *file) {
    if (file == NULL) {
        return -1;
    }
    return file->current_line;
}

int file_column(file_t *file) {
    if (file == NULL) {
        return -1;
    }
    return file->current_position-1;
}

void file_print(file_t *file) {
    if (file == NULL) {
        return;
    }
    for (size_t i = 0; i < file->line_count; i++) {
        printf("%s\n", file->lines[i]->str);
    }
}

const struct file_interface File = {
        .ctor = file_init,
        .dtor = file_dtor,
        .add_line = file_add_line,
        .from_file = file_load_from_file,
        .from_stdin = NULL,
        .getc = file_getc,
        .back_step = file_back_step,
        .line = file_line,
        .column = file_column,
        .print = file_print
};
