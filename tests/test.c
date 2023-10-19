//
// Created by Spagetik on 10.10.2023.
//

#include <stdio.h>
#include "utils.h"
#include "parts.h"

char* file_names[] = {
        "../tests/test_codes/double.swift",
        "../tests/test_codes/example1.swift",
        "../tests/test_codes/example2.swift",
        "../tests/test_codes/example3.swift",
        "../tests/test_codes/fun.swift",
        "../tests/test_codes/hello.swift",
        "../tests/test_codes/substr.swift",
        "../tests/test_codes/visibility.swift",
};


int main() {
    for (int i = 0; i < 8; i++) {
        File.ctor();
        File.from_file(file_names[i]);

        printf("%s\n\n",file_names[i]);

        TokenArray.ctor();
        int code = source_code_to_tokens();
        if (code != 0) {
            fprintf(stderr, "Error: Failed to convert source code to tokens.\n");
            printf("Line: %d\n", File.line()+1);
            printf("Column: %d\n", File.column()+1);
            return 1;
        }
        else {
            printf("Tokens:\nTOTAL: %d\n", TokenArray.total());
            for (int j = 0; j < TokenArray.total(); j++) {
                token_t *token = TokenArray.next();
                Token.print(token);
            }
        }
        File.dtor();
        TokenArray.dtor();
        printf("\n\n\n");
    }
    return 0;
}