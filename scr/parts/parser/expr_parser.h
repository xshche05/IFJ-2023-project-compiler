#ifndef IFJ_PRJ_EXPR_PARSER_H
#define IFJ_PRJ_EXPR_PARSER_H


extern bool ignore_right_bracket; // TODO
int parse_expr(char *type);

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