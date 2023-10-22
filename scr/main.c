#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"
#include "parser/expr_parser.h"


int main(int argc, char **argv) {
    char* file_name = "../test.swift";
    File.ctor();
    File.from_file(file_name);
    //File.print();
    TokenArray.ctor();

    int code = source_code_to_tokens();
    code = parse();
    printf("Parse code: %d\n", code);
    if (code != 0) {
        fprintf(stderr, "Error: Failed to convert source code to tokens.\n");
        printf("Line: %d\n", File.line()+1);
        printf("Column: %d\n", File.column()+1);
        File.back_step();
        printf("Char: '%c'\n", File.getc());
        printf("Tokens:\nTOTAL: %d\n", TokenArray.total());
    }
    else {
        printf("Tokens:\nTOTAL: %d\n", TokenArray.total());
        for (int i = 0; i < TokenArray.total(); i++) {
            token_t *token = TokenArray.next();
            Token.print(token);
        }
    }

    File.dtor();
    TokenArray.dtor();
    return 0;
}