#include "macros.h"
#include "source_file.h"
#include "parser.h"
#include "scanner.h"
#include "memory.h"

char error_msg[1000];

int main(void) {

    SourceCode.ctor();
    SourceCode.from_stdin();
    TokenArray.ctor();

    int code = source_code_to_tokens();
    if (code != SUCCESS) {
        fprintf(stderr, "Error: source code to tokens failed.\n");
        safe_exit(LEXICAL_ERROR);
    }

    code = S();
    if (code != SUCCESS) {
        fprintf(stderr, "Error: Parsing failed.\n");
        fprintf(stderr, "Error: %s\n", error_msg);
        safe_exit(SYNTAX_ERROR);
    }

    TokenArray.reset();
    code = S();
    if (code != SUCCESS) {
        fprintf(stderr, "Error: Parsing failed.\n");
        fprintf(stderr, "Error: %s\n", error_msg);
        safe_exit(SYNTAX_ERROR);
    }

    safe_exit(SUCCESS);
}