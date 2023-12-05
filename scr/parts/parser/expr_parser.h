#ifndef IFJ_PRJ_EXPR_PARSER_H
#define IFJ_PRJ_EXPR_PARSER_H

#include "symtable.h"
#include "parser.h"
#include "token.h"

typedef enum {
    UNWRAP = 0,        //0
    NOT,               //1
    MUL_DIV,           //2
    ADD_SUB,           //3
    NIL_COL,           //4
    REL_OP,            //5
    AND,               //6
    OR,                //7
    LEFT_BRACKET,      //8
    IDENTIFIER,        //9
    RIGHT_BRACKET,     //10
    DOLLAR,            //11
    LESS,              //12
    NON_TERM           //13
} expr_elem_enum;

typedef type_t ret_type_enum;

typedef struct {
    expr_elem_enum type;
    token_t *token;
    ret_type_enum ret_type;
} expr_elem_t;

extern bool ignore_right_bracket;

int parse_expr(type_t *type, bool *is_literal);

#endif //IFJ_PRJ_EXPR_PARSER_H