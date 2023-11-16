#include "macros.h"
#include "utils/utils.h"
#include "parts/parts.h"
#include "parser/expr_parser.h"


int main(int argc, char **argv) {
    char* file_name = "../test.swift";
    SourceCode.ctor();
    SourceCode.from_file(file_name);
    //SourceCode.print();
    TokenArray.ctor();

    int code = source_code_to_tokens();

//    for (int i = 0; i < TokenArray.total(); i++) {
//        Token.print(TokenArray.next());
//    }

    if (code != 0) {
        return code;
    }
    code = S();
    printf("code: %d\n", code);
    SourceCode.dtor();
    TokenArray.dtor();
    return 0;
}