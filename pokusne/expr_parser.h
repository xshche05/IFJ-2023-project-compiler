#ifndef IFJ_PRJ_EXPR_PARSER_H
#define IFJ_PRJ_EXPR_PARSER_H

#include "new_symtable.h"
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

// if (a->ret_type != b->ret_type
//                    && (a->ret_type != int_type && b->ret_type != double_type)
//                    && (a->ret_type != double_type && b->ret_type != int_type)) {
//                        fprintf(stderr, "Error: EXPR type mismatch.\n");
//                        exit(TYPE_ERROR);
//                    }
//                    new_elem->type = NON_TERM;
//                    if (a->ret_type == int_type && b->ret_type == int_type) {
//                        new_elem->ret_type = int_type;
//                    } else if (a->ret_type == double_type && b->ret_type == double_type) {
//                        new_elem->ret_type = double_type;
//                    } else if (a->ret_type == int_type && b->ret_type == double_type) {
//                        if (a->token == NULL) {
//                            fprintf(stderr, "ERROR: cant retype VAR (a)");
//                            exit(TYPE_ERROR);
//                        }
//                        new_elem->ret_type = double_type;
//                        // TODO retype A int to double
//                    } else if (a->ret_type == double_type && b->ret_type == int_type) {
//                        if (b->token == NULL) {
//                            fprintf(stderr, "ERROR: cant retype VAR (b)");
//                            exit(TYPE_ERROR);
//                        }
//                        new_elem->ret_type = double_type;
//                        // TODO retype B int to double
//                    } else {
//                        new_elem->ret_type = elems[0]->ret_type;
//                    }
//                    new_elem->token = (token_t*) ((int)a->token * (int)b->token);