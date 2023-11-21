//
// Created by ASUS on 21.11.2023.
//

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

void errorExit(errorType_t errorNum, int line)
{
    switch (errorNum)
    {
        case BAD_LEXEM_STRUCT_ERR:
            fprintf(stderr, "ERROR in line %d: Bad lexical construction\n\n", line);
            exit(errorNum);
            break;
        case BAD_SYNTAX_ERR:
            fprintf(stderr, "ERROR in line %d: Bad syntax\n\n", line);
            exit(errorNum);
            break;
        case SEMANTIC_UNDEF_REDEF_FUNC_ERR:
            fprintf(stderr, "ERROR in line %d: You're trying to define something, what has been already declared\n\n", line);
            exit(errorNum);
            break;
        case SEMANTIC_PARAM_COUNT_ERR:
            fprintf(stderr, "ERROR in line %d:  You're trying to pass bad count/type of arguments in function\n\n", line);
            exit(errorNum);
            break;
        case SEMANTIC_UNDEF_VAR_ERR:
            fprintf(stderr, "ERROR in line %d: Undefined variable\n\n",line);
            exit(errorNum);
            break;
        case SEMANTIC_IN_EXPRESSION_TYPES_ERR:
            fprintf(stderr, "ERROR in line %d: You're trying to do an operation with operands with different datatypes\n\n",line);
            exit(errorNum);
            break;
        case SEMANTIC_ANOTHER_ERR:
            fprintf(stderr, "ERROR in line %d: Some semantic error, check it\n\n",line);
            exit(errorNum);
            break;
        case SEMANTIC_IN_EXPRESSION_ERR:
            fprintf(stderr, "ERROR in line %d: Bad usage of expression\n\n",line);
            exit(errorNum);
            break;
        case SEMANTIC_TYPE_COMPATIBILITY_ERR:
            fprintf(stderr, "ERROR in line %d: You're trying to assign something with another type\n\n",line);
            exit(errorNum);
            break;
        case COMPILER_INTERN_ERR:
            exit(errorNum);
            break;
        default:
            break;
    }
}