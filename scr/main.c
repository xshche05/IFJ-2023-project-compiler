#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"
#include "parser/expr_parser.h"

char error_msg[1000];

int main(int argc, char **argv) {
    char* file_name = "../test.swift";
    SourceCode.ctor();
    SourceCode.from_file(file_name);
    TokenArray.ctor();

    int code = source_code_to_tokens();
    if (code != SUCCESS) {
        fprintf(stderr, "Error: source code to tokens failed.\n");
        return LEXICAL_ERROR;
    }
    code = S();
    if (code != SUCCESS) {
        fprintf(stderr, "Error: Parsing failed.\n");
        fprintf(stderr, "Error: %s\n", error_msg);
        return SYNTAX_ERROR;
    }
    return SUCCESS;
}