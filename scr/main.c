#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"


int main(int argc, char **argv) {
    char* file_name = "..\\test.swift";
    File.ctor();
    File.from_file(file_name);

    token_array_ctor();
    int code = source_code_to_tokens();
    if (code != 0) {
        fprintf(stderr, "Error: Failed to convert source code to tokens.\n");
        printf("Line: %d\n", File.line()+1);
        printf("Column: %d\n", File.column()+1);
    }
    else {
        printf("Tokens:\nTOTAL: %d\n", tokens->length);
        for (int i = 0; i < tokens->length; i++) {
            token_t *token = tokens->array[i];
            token_print(token);
        }
    }
    return 0;
}