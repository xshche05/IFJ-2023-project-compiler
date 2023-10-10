#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"


int main(int argc, char **argv) {
    char* file_name = "..\\test.swift";
    file_t *file = File.ctor();
    File.from_file(file_name, file);

    token_array_t *token_array = source_code_to_tokens(file);
    if (token_array == NULL) {
        fprintf(stderr, "Error: Failed to convert source code to tokens.\n");
        printf("Line: %d\n", File.line(file)+1);
        printf("Column: %d\n", File.column(file)+1);
    }
    else {
        printf("Tokens:\nTOTAL: %d\n", token_array->length);
        for (size_t i = 0; i < token_array->length; i++) {
            token_t *token = token_array->array[i];
            token_print(token);
        }
    }
    return 0;
}