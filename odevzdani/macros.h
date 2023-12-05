/*
 * IFJ Project 2023
 * Macros for error handling and other useful macros
 * Author: Kirill Shchetiniuk (xshche05)
 */

#ifndef IFJ_PRJ_MACROS_H
#define IFJ_PRJ_MACROS_H

//Define error codes
#define SUCCESS 0
#define LEXICAL_ERROR 1

#define SYNTAX_ERROR 2

#define SEMANTIC_ERROR_3 3
#define SEM_NON_DEF_FUNC 3
#define SEM_REDEF_VAR 3

#define SEMANTIC_ERROR_4 4
#define SEM_FUNC_PARAM_SIG 4
#define SEM_FUNC_RET_VAL 4

#define SEMANTIC_ERROR_5 5
#define SEM_USE_OF_NON_INIT_VAR 5
#define SEM_USE_OF_NON_DEF_VAR 5



#define SEMANTIC_ERROR_6 6
#define SEMANTIC_ERROR_7 7
#define SEMANTIC_ERROR_8 8
#define SEMANTIC_ERROR_9 9
#define INTERNAL_ERROR 99

#define min(x, y) ((x < y) ? x : y)

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

extern char error_msg[1000]; // TODO

//Macro for error handling
#define EXIT_ERROR(error_code) \
    do { \
        switch(error_code){ \
            case LEXICAL_ERROR: \
                fprintf(stderr, "Lexical error: incorrect structure of a lexeme."); \
                break; \
            case SYNTAX_ERROR: \
                fprintf(stderr, "Syntactic error: incorrect program syntax."); \
                break; \
            case SEMANTIC_ERROR_3: \
                fprintf(stderr, "Semantic error: undefined function or variable redefinition."); \
                break; \
            case SEMANTIC_ERROR_4: \
                fprintf(stderr, "Semantic error: wrong number/type of parameters or wrong type of return value."); \
                break; \
            case SEMANTIC_ERROR_5: \
                fprintf(stderr, "Semantic error: use of an undefined or uninitialized variable."); \
                break; \
            case SEMANTIC_ERROR_6: \
                fprintf(stderr, "Semantic error: missing/excess expression in the function return command."); \
                break; \
            case SEMANTIC_ERROR_7: \
                fprintf(stderr, "Semantic error: wrong type compatibility in an expression."); \
                break; \
            case SEMANTIC_ERROR_8: \
                fprintf(stderr, "Semantic error: unspecified type of a variable/parameter that cannot be inferred from the expression."); \
                break; \
            case SEMANTIC_ERROR_9: \
                fprintf(stderr, "Semantic error."); \
                break; \
            case INTERNAL_ERROR: \
                fprintf(stderr, "Internal compiler error."); \
                break; \
        } \
        exit(error_code); \
    }while(0)

#define str_eq(str1, str2) (!String.cmp(str1, str2))
#define str_eq_cstr(str1, str2) (!String.cmp_cstr(str1, str2))


#endif //IFJ_PRJ_MACROS_H
