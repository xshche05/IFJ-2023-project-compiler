#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"


int main(int argc, char **argv) {
    char* file_name = "..\\test.swift";
    file_t *file = file_init();
    file_load(file_name, file);

    token_array_t *token_array = source_code_to_tokens(file);
    if (token_array == NULL) {
        fprintf(stderr, "Error: Failed to convert source code to tokens.\n");
    }
    return 0;
}