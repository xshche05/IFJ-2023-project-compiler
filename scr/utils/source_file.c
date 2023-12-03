#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

#include "source_file.h"

file_t *sourceFile = NULL;

static int file_init(void) {
    if (sourceFile != NULL) {
        fprintf(stderr, "Error: sourceFile already initialized.\n");
        return -1;
    }
    sourceFile = safe_malloc(sizeof(file_t));
    sourceFile->file_name = NULL;
    sourceFile->lines = safe_malloc(sizeof(string_t*) * 8);
    sourceFile->line_allocated = 8;
    sourceFile->line_count = 0;
    sourceFile->current_line = 0;
    sourceFile->current_position = 0;
    return 0;
}

static void file_dtor(void) {
    if (sourceFile == NULL) {
        return;
    }
    for (int i = 0; i < sourceFile->line_count; i++) {
        String.dtor(sourceFile->lines[i]);
    }
    safe_free(sourceFile->file_name);
    safe_free(sourceFile->lines);
    safe_free(sourceFile);
    sourceFile = NULL;
}

static int file_add_line(string_t *line) {
    if (sourceFile == NULL || line == NULL) {
        return -1;
    }
    if (sourceFile->line_count == sourceFile->line_allocated) {
        void* tmp = safe_realloc(sourceFile->lines, sizeof(string_t) * sourceFile->line_allocated * 2);
        sourceFile->lines = tmp;
        sourceFile->line_allocated *= 2;
    }
    sourceFile->lines[sourceFile->line_count] = line;
    sourceFile->line_count++;
    return 0;
}

static int file_load_from_file(char *file_name) {
    if (sourceFile == NULL) {
        return -1;
    }
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: sourceFile %s not found.\n", file_name);
        return -1;
    }
    string_t *line = String.ctor();
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            file_add_line(line);
            line = String.ctor();
            if (line == NULL) {
                return -1;
            }
            continue;
        }
        String.add_char(line, (char) c);
    }
    SourceCode.add_line(line);
    fclose(f);
    sourceFile->file_name = safe_malloc(sizeof(char) * (strlen(file_name) + 1));
    strcpy(sourceFile->file_name, file_name);
    return 0;
}

static char file_getc(void) {
    if (sourceFile == NULL) {
        return -1;
    }
    if (sourceFile->current_line >= sourceFile->line_count) {
        return -1;
    }
    if (sourceFile->current_position >= sourceFile->lines[sourceFile->current_line]->length) {
        sourceFile->current_line++;
        sourceFile->current_position = 0;
        return '\n';
    }
    char c = sourceFile->lines[sourceFile->current_line]->str[sourceFile->current_position];
    sourceFile->current_position++;
    return c;
}

static void file_back_step(void) {
    if (sourceFile == NULL) {
        return;
    }
    if (sourceFile->current_position == 0) {
        sourceFile->current_line--;
        sourceFile->current_position = sourceFile->lines[sourceFile->current_line]->length;
        return;
    }
    sourceFile->current_position--;
}

static int file_line(void) {
    if (sourceFile == NULL) {
        return -1;
    }
    return sourceFile->current_line;
}

static int file_column(void) {
    if (sourceFile == NULL) {
        return -1;
    }
    return sourceFile->current_position - 1;
}

static void file_print(void) {
    if (sourceFile == NULL) {
        return;
    }
    for (int i = 0; i < sourceFile->line_count; i++) {
        fprintf(stderr, "%s\n", sourceFile->lines[i]->str);
    }
}

static char* print_current_line(void) {
    if (sourceFile == NULL) {
        return "NONE";
    }
    if (sourceFile->current_line >= sourceFile->line_count) {
        return "NONE";
    }
    return sourceFile->lines[sourceFile->current_line]->str;
}

static int load_stdin(void) {
    if (sourceFile == NULL) {
        return -1;
    }
    string_t *line = String.ctor();
    if (line == NULL) {
        return -1;
    }
    int c;
    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            file_add_line(line);
            line = String.ctor();
            if (line == NULL) {
                return -1;
            }
            continue;
        }
        String.add_char(line, (char) c);
    }
    SourceCode.add_line(line);
    return 0;
}

const struct file_interface SourceCode = {
        .ctor = file_init,
        .dtor = file_dtor,
        .add_line = file_add_line,
        .from_file = file_load_from_file,
        .from_stdin = load_stdin,
        .file_getc = file_getc,
        .back_step = file_back_step,
        .line = file_line,
        .column = file_column,
        .print = file_print,
        .print_line = print_current_line
};
