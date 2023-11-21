#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

void errorExit(errorType_t errorNum)
{
    switch (errorNum)
    {
        case BAD_LEXEM_STRUCT_ERR:
            fprintf(stderr, "ERROR: Bad lexical construction\n\n");
            exit(errorNum);
            break;
        case BAD_SYNTAX_ERR:
            fprintf(stderr, "ERROR: Bad syntax\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_UNDEF_REDEF_FUNC_ERR:
            fprintf(stderr, "ERROR: You're trying to define something, that has been already declared\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_PARAM_COUNT_ERR:
            fprintf(stderr, "ERROR:  You're trying to pass bad count/type of arguments in function\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_UNDEF_VAR_ERR:
            fprintf(stderr, "ERROR: Undefined variable\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_IN_EXPRESSION_TYPES_ERR:
            fprintf(stderr, "ERROR: You're trying to do an operation with operands with different data types\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_ANOTHER_ERR:
            fprintf(stderr, "ERROR: Some semantic error, check it\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_IN_EXPRESSION_ERR:
            fprintf(stderr, "ERROR: Bad usage of expression\n\n");
            exit(errorNum);
            break;
        case SEMANTIC_TYPE_COMPATIBILITY_ERR:
            fprintf(stderr, "ERROR: You're trying to assign something with another type\n\n");
            exit(errorNum);
            break;
        case COMPILER_INTERN_ERR:
            exit(errorNum);
            break;
        default:
            break;
    }
}