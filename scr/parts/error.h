//
// Created by ASUS on 21.11.2023.
//

#ifndef IFJ_PRJ_ERROR_H
#define IFJ_PRJ_ERROR_H

typedef enum
{
/*1*/  BAD_LEXEM_STRUCT_ERR = 1,
/*2*/  BAD_SYNTAX_ERR,                      // bad program syntax
/*3*/  SEMANTIC_UNDEF_REDEF_FUNC_ERR,       // undefined function/variable, redefinition and so on
/*4*/  SEMANTIC_PARAM_COUNT_ERR,            // bad count/type of parameters
/*5*/  SEMANTIC_UNDEF_VAR_ERR,              // undefined variable
/*6*/  SEMANTIC_IN_EXPRESSION_ERR,          // bad usage of expression
/*7*/  SEMANTIC_IN_EXPRESSION_TYPES_ERR,    // incompatibility of types in expression(arithmetics, string and so on)
/*8*/  SEMANTIC_TYPE_COMPATIBILITY_ERR,     // incompatibility of types in other cases
/*9*/  SEMANTIC_ANOTHER_ERR,                // another semantic errors
/*99*/ COMPILER_INTERN_ERR = 99             // intern error inside of compiler itself(memory allocation error and so on)
}errorType_t;

void errorExit(errorType_t, int);

#endif //IFJ_PRJ_ERROR_H
