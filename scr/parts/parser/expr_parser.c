#include "token.h"
#include "parser.h"
#include "expr_parser.h"
#include "codegen/codegen.h"
#include <stdio.h>
#include <string.h>
#include "../macros.h"
#include "memory.h"

int prec_table[12][12] = {
        {4, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2},
        {1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2},
        {1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2},
        {1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 2, 2},
        {1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 2},
        {1, 1, 1, 1, 1, 4, 2, 2, 1, 1, 2, 2},
        {1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2},
        {1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 4},
        {2, 4, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2},
        {4, 4, 2, 2, 2, 2, 2, 2, 4, 4, 2, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4}
}; // 1 is <, 2 is >, 3 is =, 4 is ERROR

#define TYPE_ERROR 7

bool ignore_right_bracket = false;

token_t *local_lookahead;
token_t *after_local_lookahead;

bool nl_flag = false;
bool first_flag = false;

stack_t *pushdown_stack = NULL;
stack_t *tmp_stack = NULL;

static int map_token(token_t *token, int *bracket_count) {
    switch (token->type) {
        case TOKEN_UNWRAP_NILLABLE:
            return UNWRAP;
        case TOKEN_LOGICAL_NOT:
            return NOT;
        case TOKEN_ADDITION:
        case TOKEN_SUBTRACTION:
            return ADD_SUB;
        case TOKEN_MULTIPLICATION:
        case TOKEN_DIVISION:
            return MUL_DIV;
        case TOKEN_IS_NIL:
            return NIL_COL;
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
        case TOKEN_GREATER_THAN:
        case TOKEN_EQUAL_TO:
        case TOKEN_NOT_EQUAL_TO:
            return REL_OP;
        case TOKEN_LOGICAL_AND:
            return AND;
        case TOKEN_LOGICAL_OR:
            return OR;
        case TOKEN_LEFT_BRACKET:
            (*bracket_count)++;
            return LEFT_BRACKET;
        case TOKEN_IDENTIFIER:
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_REAL_LITERAL:
        case TOKEN_STRING_LITERAL:
        case TOKEN_NIL_LITERAL:
        case TOKEN_TRUE_LITERAL:
        case TOKEN_FALSE_LITERAL:
            return IDENTIFIER;
        case TOKEN_RIGHT_BRACKET:
            if (!ignore_right_bracket) {
                (*bracket_count)--;
                return RIGHT_BRACKET;
            } else {
                if (*bracket_count > 0) {
                    (*bracket_count)--;
                    return RIGHT_BRACKET;
                } else {
                    return DOLLAR;
                }
            }
        default:
            return DOLLAR;
    }
}

static void init_stacks() {
    pushdown_stack = Stack.init();
    tmp_stack = Stack.init();
}

static void destroy_stacks() {
    Stack.destroy(pushdown_stack);
    Stack.destroy(tmp_stack);
}

static void clear_until_top_terminal() {
    expr_elem_t *top = Stack.top(pushdown_stack);
    while (top->type == NON_TERM || top->type == LESS) {
        Stack.push(tmp_stack, top);
        Stack.pop(pushdown_stack);
        top = Stack.top(pushdown_stack);
    }
}

static void transfer_tmp_stack() {
    while (Stack.top(tmp_stack) != NULL) {
        Stack.push(pushdown_stack, Stack.top(tmp_stack));
        Stack.pop(tmp_stack);
    }
}

static void rel_op_check(expr_elem_t *a, expr_elem_t *b) {
    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type) || a->ret_type == bool_type
    || a->ret_type == nil_type || b->ret_type == nil_type) {
        if (a->ret_type == double_type) {
            if (b->ret_type == int_type && b->token != NULL) {
                gen_line("POPS GF@$A\n");
                gen_line("INT2FLOATS\n");
                gen_line("PUSHS GF@$A\n");
            } else {
                fprintf(stderr, "Error: EXPR type mismatch.\n");
                safe_exit(TYPE_ERROR);
            }
        } else if (b->ret_type == double_type) {
            if (a->ret_type == int_type && a->token != NULL) {
                gen_line("INT2FLOATS\n");
            } else {
                fprintf(stderr, "Error: EXPR type mismatch.\n");
                safe_exit(TYPE_ERROR);
            }
        } else {
            fprintf(stderr, "Error: EXPR type mismatch.\n");
            safe_exit(TYPE_ERROR);
        }
    }
}

static void log_op_check(expr_elem_t *a, expr_elem_t *b) {
    if (a->ret_type != bool_type || a->ret_type != b->ret_type) {
        fprintf(stderr, "Error: EXPR type mismatch.\n");
        safe_exit(TYPE_ERROR);
    }
    if (a->ret_type > 3 || b->ret_type > 3) {
        fprintf(stderr, "Error: Cant or nillable vars.\n");
        safe_exit(TYPE_ERROR);
    }
}

static void ar_op_check(expr_elem_t *a, expr_elem_t *b, expr_elem_t *new_elem, expr_elem_t **elems) {
    if (a->ret_type != b->ret_type
        && !(a->ret_type == int_type && b->ret_type == double_type)
        && !(a->ret_type == double_type && b->ret_type == int_type)) {
        fprintf(stderr, "Error: EXPR type mismatch.\n");
        safe_exit(TYPE_ERROR); // TODO FIX COND
    }
    new_elem->type = NON_TERM;
    if (a->ret_type == int_type && b->ret_type == int_type) {
        new_elem->ret_type = int_type;
    } else if (a->ret_type == double_type && b->ret_type == double_type) {
        new_elem->ret_type = double_type;
    } else if (a->ret_type == int_type && b->ret_type == double_type) {
        if (a->token == NULL) {
            fprintf(stderr, "ERROR: cant retype VAR (a)\n");
            safe_exit(TYPE_ERROR);
        }
        new_elem->ret_type = double_type;
        gen_line("INT2FLOATS\n");
    } else if (a->ret_type == double_type && b->ret_type == int_type) {
        if (b->token == NULL) {
            fprintf(stderr, "ERROR: cant retype VAR (b)\n");
            safe_exit(TYPE_ERROR);
        }
        new_elem->ret_type = double_type;
        gen_line("POPS GF@$A\n");
        gen_line("INT2FLOATS\n");
        gen_line("PUSHS GF@$A\n");
    } else {
        new_elem->ret_type = elems[0]->ret_type;
    }
    new_elem->token = (token_t *) ((size_t) a->token * (size_t) b->token);
}

static void add_op_check(expr_elem_t *a, expr_elem_t *b) {
    if (a->ret_type == bool_type) {
        fprintf(stderr, "Error: EXPR type mismatch.\n");
        safe_exit(TYPE_ERROR);
    }
    if (a->ret_type > 3 || b->ret_type > 3) {
        fprintf(stderr, "Error: Cant add nillable vars.\n");
        safe_exit(TYPE_ERROR);
    }
}

static void other_ar_op_check(expr_elem_t *a, expr_elem_t *b) {
    if (a->ret_type == string_type || a->ret_type == bool_type) {
        fprintf(stderr, "Error: EXPR type mismatch.\n");
        safe_exit(TYPE_ERROR);
    }
    if (a->ret_type > 3 || b->ret_type > 3) {
        fprintf(stderr, "Error: Cant sub nillable vars.\n");
        safe_exit(TYPE_ERROR);
    }
}

static void eq_op_check(expr_elem_t *a, expr_elem_t *b) {
    if (!(a->ret_type == b->ret_type || a->ret_type == nil_type || b->ret_type == nil_type ||
          a->ret_type - 4 == b->ret_type || b->ret_type - 4 == a->ret_type)) {
        if ((a->ret_type == double_type || a->ret_type == nil_double_type) && b->ret_type == int_type) {
            if (b->token == NULL) {
                fprintf(stderr, "ERROR: cant retype VAR (b)\n");
                safe_exit(TYPE_ERROR);
            }
            gen_line("POPS GF@$A\n");
            gen_line("INT2FLOATS\n");
            gen_line("PUSHS GF@$A\n");
        } else if ((b->ret_type == double_type || b->ret_type == nil_double_type) && a->ret_type == int_type) {
            if (a->token == NULL) {
                fprintf(stderr, "ERROR: cant retype VAR (a)\n");
                safe_exit(TYPE_ERROR);
            }
            gen_line("INT2FLOATS\n");
        } else {
            fprintf(stderr, "Error: EXPR type mismatch.\n");
            safe_exit(TYPE_ERROR);
        }
    }
}

static void reduce() {
    expr_elem_t *elem = Stack.top(pushdown_stack);
    expr_elem_t **elems = safe_malloc(sizeof(expr_elem_t *) * 3);
    int i = 0;
    while (elem->type != LESS) {
        elems[i] = elem;
        Stack.pop(pushdown_stack);
        elem = Stack.top(pushdown_stack);
        i++;
    }
    Stack.pop(pushdown_stack);
    expr_elem_t *new_elem = safe_malloc(sizeof(expr_elem_t));
    if (collect_funcs) {
        new_elem->type = NON_TERM;
        new_elem->ret_type = void_type;
        new_elem->token = NULL;
        Stack.push(pushdown_stack, new_elem);
        return;
    }
    if (i == 1) // reduce E -> i
    {
        if (elems[0]->token == NULL && elems[0]->type == IDENTIFIER) {
            new_elem->type = NON_TERM;
            if (elems[0]->ret_type == void_type) {
                fprintf(stderr, "Error: EXPR Void type operand.\n");
                safe_exit(TYPE_ERROR);
            }
            new_elem->ret_type = elems[0]->ret_type;
            new_elem->token = NULL;
            gen_line("PUSHS GF@$RET\n");
        } else if (elems[0]->token->type == TOKEN_INTEGER_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = int_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS int@%d\n", elems[0]->token->attribute.integer);
        } else if (elems[0]->token->type == TOKEN_REAL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = double_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS float@%a\n", elems[0]->token->attribute.real);
        } else if (elems[0]->token->type == TOKEN_STRING_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = string_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS string@%s\n", elems[0]->token->attribute.string->str);
        } else if (elems[0]->token->type == TOKEN_TRUE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS bool@true\n");
        } else if (elems[0]->token->type == TOKEN_FALSE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS bool@false\n");
        } else if (elems[0]->token->type == TOKEN_NIL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = nil_type;
            new_elem->token = (token_t *) 1;
            gen_line("PUSHS nil@nil\n");
        } else if (elems[0]->token->type == TOKEN_IDENTIFIER) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[0]->ret_type;
            new_elem->token = NULL;
            // check if var is defined
            varData_t *varData = get_var(elems[0]->token->attribute.identifier);
            if (varData == NULL) varData = get_let(elems[0]->token->attribute.identifier);
            if (!varData->isInited) {
                fprintf(stderr, "Error: EXPR undefined variable.\n");
                safe_exit(SEMANTIC_ERROR_5);
            }
            // TODO push var with frame and scope
            if (varData->minInitScope > get_scope()) {
                fprintf(stderr, "Error: variable not initialized in this scope\n");
                safe_exit(SEMANTIC_ERROR_5);
            }
            gen_line("PUSHS %s\n", gen_var_name(varData->name->str, varData->scope));
        } else {
            fprintf(stderr, "Error: EXPR syntax error\n");
            safe_exit(SYNTAX_ERROR);
        }
    } else if (i == 2) { // E -> !E ; E -> E!
        if (elems[1]->type == NOT && elems[0]->type == NON_TERM) {
            if (elems[0]->ret_type != bool_type) {
                fprintf(stderr, "Error: EXPR type mismatch.\n");
                safe_exit(TYPE_ERROR);
            }
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = NULL;
            gen_line("NOTS\n");
        } else if (elems[1]->type == NON_TERM && elems[0]->type == UNWRAP) {
            if (elems[1]->ret_type < 4) {
                fprintf(stderr, "Error: cant unwrap non-nil\n");
                safe_exit(TYPE_ERROR);
            } else if (elems[1]->ret_type == nil_type) {
                fprintf(stderr, "Error: cant unwrap nil\n");
                safe_exit(TYPE_ERROR);
            }
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[1]->ret_type - 4;
            new_elem->token = NULL;
        } else {
            fprintf(stderr, "Error: EXPR syntax error\n");
            safe_exit(SYNTAX_ERROR);
        }
    } else if (i == 3) { // E -> E op E; E -> (E)
        char *label_end;
        char *label_false;
        char *label_true;
        token_t *op = elems[1]->token;
        if (elems[0]->type == RIGHT_BRACKET && elems[1]->type == NON_TERM && elems[2]->type == LEFT_BRACKET) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[1]->ret_type;
            new_elem->token = elems[1]->token;
        } else {
            expr_elem_t *a = elems[0];
            expr_elem_t *b = elems[2];
            if (a->type != NON_TERM || b->type != NON_TERM) {
                fprintf(stderr, "Error: EXPR syntax error\n");
                safe_exit(SYNTAX_ERROR);
            }
            switch (op->type) {
                case TOKEN_ADDITION:
                    ar_op_check(a, b, new_elem, elems);
                    add_op_check(a, b);
                    if (new_elem->ret_type == string_type) {
                        gen_line("POPS GF@$B\n");
                        gen_line("POPS GF@$A\n");
                        gen_line("CONCAT GF@$RET GF@$A GF@$B\n");
                        gen_line("PUSHS GF@$RET\n");
                    } else {
                        gen_line("ADDS\n");
                    }
                    break;
                case TOKEN_SUBTRACTION:
                    ar_op_check(a, b, new_elem, elems);
                    other_ar_op_check(a, b);
                    gen_line("SUBS\n");
                    break;
                case TOKEN_MULTIPLICATION:
                    ar_op_check(a, b, new_elem, elems);
                    other_ar_op_check(a, b);
                    gen_line("MULS\n");
                    break;
                case TOKEN_DIVISION:
                    ar_op_check(a, b, new_elem, elems);
                    other_ar_op_check(a, b);
                    if (a->ret_type == int_type && b->ret_type == int_type) {
                        gen_line("IDIVS\n");
                    } else {
                        gen_line("DIVS\n");
                    }
                    break;
                case TOKEN_LOGICAL_AND:
                    log_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    label_end = gen_unique_label("and_end");
                    label_false = gen_unique_label("and_false");
                    gen_line("POPS GF@$B\n");
                    gen_line("POPS GF@$A\n");
                    gen_line("JUMPIFEQ %s GF@$A bool@false\n", label_false);
                    gen_line("JUMPIFEQ %s GF@$B bool@false\n", label_false);
                    gen_line("PUSHS bool@true\n");
                    gen_line("JUMP %s\n", label_end);
                    gen_line("LABEL %s\n", label_false);
                    gen_line("PUSHS bool@false\n");
                    gen_line("LABEL %s\n", label_end);
//                    gen_line("ANDS\n");
                    break;
                case TOKEN_LOGICAL_OR:
                    log_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    label_end = gen_unique_label("or_end");
                    label_true = gen_unique_label("or_true");
                    gen_line("POPS GF@$B\n");
                    gen_line("POPS GF@$A\n");
                    gen_line("JUMPIFEQ %s GF@$A bool@true\n", label_true);
                    gen_line("JUMPIFEQ %s GF@$B bool@true\n", label_true);
                    gen_line("PUSHS bool@false\n");
                    gen_line("JUMP %s\n", label_end);
                    gen_line("LABEL %s\n", label_true);
                    gen_line("PUSHS bool@true\n");
                    gen_line("LABEL %s\n", label_end);
//                    gen_line("ORS\n");
                    break;
                case TOKEN_LESS_THAN:
                    rel_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("LTS\n");
                    break;
                case TOKEN_LESS_THAN_OR_EQUAL_TO:
                    rel_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("GTS\n");
                    gen_line("NOTS\n");
                    break;
                case TOKEN_GREATER_THAN:
                    rel_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("GTS\n");
                    break;
                case TOKEN_GREATER_THAN_OR_EQUAL_TO:
                    rel_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("LTS\n");
                    gen_line("NOTS\n");
                    break;
                case TOKEN_EQUAL_TO:
                    eq_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("EQS\n");
                    break;
                case TOKEN_NOT_EQUAL_TO:
                    eq_op_check(a, b);
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    gen_line("EQS\n");
                    gen_line("NOTS\n");
                    break;
                case TOKEN_IS_NIL:
                    if (b->ret_type > 3) {
                        if (!(b->ret_type - 4 == a->ret_type || b->ret_type == nil_type ||
                              a->ret_type == b->ret_type) || a->ret_type > 3) {
                            if ((b->ret_type == double_type || b->ret_type == nil_double_type) && a->ret_type == int_type
                            && a->token != NULL) {
                                gen_line("INT2FLOATS\n");
                            } else {
                                fprintf(stderr, "Error: EXPR type mismatch.\n");
                                safe_exit(TYPE_ERROR);
                            }
                        }
                        new_elem->type = NON_TERM;
                        new_elem->ret_type = a->ret_type;
                        new_elem->token = NULL;
                        gen_line("CALL $??op\n");
                        gen_line("PUSHS GF@$RET\n");
                    } else {
                        fprintf(stderr, "Warning: Right side of ?? operator is never used\n");
                        if (b->ret_type != a->ret_type && a->ret_type != nil_type && b->ret_type != nil_type) {
                            fprintf(stderr, "Error: EXPR type mismatch.\n");
                            safe_exit(TYPE_ERROR);
                        }
                        new_elem->type = NON_TERM;
                        new_elem->ret_type = b->ret_type;
                        new_elem->token = NULL;
                        gen_line("POPS GF@$D\n");
                    }
                    break;
                default:
                    fprintf(stderr, "Error: EXPR unknown operator.\n");
                    safe_exit(SEMANTIC_ERROR_9);
            }
        }
    } else {
        fprintf(stderr, "Error: EXPR syntax error\n");
        safe_exit(SYNTAX_ERROR);
    }
    for (int j = 0; j < i; ++j) {
        safe_free(elems[j]);
    }
    safe_free(elems);
    Stack.push(pushdown_stack, new_elem);
}

static expr_elem_t *get_top_terminal() {
    expr_elem_t *top = Stack.top(pushdown_stack);
    while (top->type == NON_TERM || top->type == LESS) {
        Stack.push(tmp_stack, top);
        Stack.pop(pushdown_stack);
        top = Stack.top(pushdown_stack);
    }
    while (Stack.top(tmp_stack) != NULL) {
        Stack.push(pushdown_stack, Stack.top(tmp_stack));
        Stack.pop(tmp_stack);
    }
    return top;
}

static expr_elem_t *next_token(int *bracket_count) {
    bool is_last_on_line = false;
    if (local_lookahead != NULL) {
        is_last_on_line = local_lookahead->has_newline_after;
    }

    local_lookahead = after_local_lookahead;
    after_local_lookahead = TokenArray.next();

    token_t *current = local_lookahead;
    token_t *next = after_local_lookahead;

    // if is_last_on_line, then prefetch action, if error action, then return DOLLAR
    if (is_last_on_line && !first_flag) {
        expr_elem_t *a = safe_malloc(sizeof(expr_elem_t));
        a->type = map_token(current, bracket_count);
        expr_elem_t *b = get_top_terminal();
        int action = prec_table[b->type][a->type];
        if (action == 4) {
            expr_elem_t *dollar = safe_malloc(sizeof(expr_elem_t));
            dollar->type = DOLLAR;
            dollar->token = NULL;
            dollar->ret_type = 0;
            return dollar;
        }
        if (a->type == LEFT_BRACKET) {
            (*bracket_count)--;
        }
        if (a->type == RIGHT_BRACKET) {
            (*bracket_count)++;
        }
        safe_free(a);
    }

    if (current->type == TOKEN_IDENTIFIER) {
        if (next->type == TOKEN_LEFT_BRACKET) {
            lookahead = TokenArray.next();
            bool tmp = ignore_right_bracket;
            bool tmp2 = first_flag;
            ignore_right_bracket = true;
            stack_t *tmp_1 = pushdown_stack;
            stack_t *tmp_2 = tmp_stack;
            CALL_PARAM_LIST(false, current->attribute.identifier->str);
            pushdown_stack = tmp_1;
            tmp_stack = tmp_2;
            ignore_right_bracket = tmp;
            first_flag = tmp2;
            funcData_t *funcData;
            if (!collect_funcs) {
                funcData = get_func(current->attribute.identifier);
                gen_line("CALL %s\n", current->attribute.identifier->str);
            }
            local_lookahead = lookahead;
            after_local_lookahead = TokenArray.next();
            expr_elem_t *elem = safe_malloc(sizeof(expr_elem_t));
            elem->type = IDENTIFIER;
            elem->token = NULL;
            if (collect_funcs) {
                elem->ret_type = none_type;
            } else {
                elem->ret_type = funcData->returnType;
            }
            return elem;
        }
    }
    expr_elem_t *elem = safe_malloc(sizeof(expr_elem_t));
    elem->type = map_token(local_lookahead, bracket_count);
    if (collect_funcs) return elem;
    elem->token = local_lookahead;
    if (elem->type == IDENTIFIER) {
        if (elem->token->type == TOKEN_IDENTIFIER) {
            varData_t *varData = get_var(elem->token->attribute.identifier);
            if (varData == NULL) {
                letData_t *letData = get_let(elem->token->attribute.identifier);
                elem->ret_type = letData->type;
            } else {
                elem->ret_type = varData->type;
            }
        }
    } else {
        elem->ret_type = none_type;
    }
    return elem;
}

#ifdef DEBUG
static void DEBUG_print_push_down_stack() {
    stack_t *temp = Stack.init();
    expr_elem_t *elem = Stack.top(pushdown_stack);
    while (elem != NULL) {
        Stack.push(temp, elem);
        Stack.pop(pushdown_stack);
        switch (elem->type) {
            case NON_TERM:
                DEBUG_PRINT("NON_TERM ");
                break;
            case LESS:
                DEBUG_PRINT("LESS ");
                break;
            case DOLLAR:
                DEBUG_PRINT("DOLLAR ");
                break;
            case UNWRAP:
                DEBUG_PRINT("UNWRAP ");
                break;
            case NOT:
                DEBUG_PRINT("NOT ");
                break;
            case ADD_SUB:
                DEBUG_PRINT("ADD_SUB ");
                break;
            case MUL_DIV:
                DEBUG_PRINT("MUL_DIV ");
                break;
            case NIL_COL:
                DEBUG_PRINT("NIL_COL ");
                break;
            case REL_OP:
                DEBUG_PRINT("REL_OP ");
                break;
            case AND:
                DEBUG_PRINT("AND ");
                break;
            case OR:
                DEBUG_PRINT("OR ");
                break;
            case LEFT_BRACKET:
                DEBUG_PRINT("LEFT_BRACKET ");
                break;
            case IDENTIFIER:
                DEBUG_PRINT("IDENTIFIER ");
                break;
            case RIGHT_BRACKET:
                DEBUG_PRINT("RIGHT_BRACKET ");
                break;
            default:
                break;
        }
        elem = Stack.top(pushdown_stack);
    }

    while (Stack.top(temp) != NULL) {
        Stack.push(pushdown_stack, Stack.top(temp));
        Stack.pop(temp);
    }

    DEBUG_PRINT("\n")
}
#endif

int parse_expr(type_t *ret_type, bool *is_literal) {
    pushdown_stack = NULL;
    tmp_stack = NULL;
    int bracket_count = 0;
    init_stacks();
    DEBUG_PRINT("EXPR\n");
    expr_elem_t *dollar = safe_malloc(sizeof(expr_elem_t));
    dollar->type = DOLLAR;
    dollar->token = NULL;
    dollar->ret_type = none_type;
    Stack.push(pushdown_stack, dollar);

    TokenArray.prev();

    after_local_lookahead = TokenArray.next();

    expr_elem_t *a;
    expr_elem_t *b;

    first_flag = true;
    a = next_token(&bracket_count);
    if (a->type == DOLLAR) {
        fprintf(stderr, "Error: EXPR syntax error.\n");
        return SYNTAX_ERROR;
    }
    first_flag = false;

    expr_elem_t *less = safe_malloc(sizeof(expr_elem_t));
    less->type = LESS;
    less->token = NULL;
    less->ret_type = 0;

    while (true) {
        #ifdef DEBUG
        DEBUG_print_push_down_stack();
        #endif
        b = get_top_terminal();
        if (a->type == DOLLAR && b->type == DOLLAR) {
            break;
        }
        int action = prec_table[b->type][a->type];
        switch (action) {
            case 1:
                clear_until_top_terminal();
                Stack.push(pushdown_stack, less);
                transfer_tmp_stack();
                Stack.push(pushdown_stack, a);
                a = next_token(&bracket_count);
                break;
            case 2:
                reduce();
                break;
            case 3:
                Stack.push(pushdown_stack, a);
                a = next_token(&bracket_count);
                break;
            default:
                fprintf(stderr, "Error: EXPR syntax error.\n");
                return SYNTAX_ERROR;
        }
    }
    lookahead = TokenArray.prev();
    expr_elem_t *result = Stack.top(pushdown_stack);
    *ret_type = result->ret_type;
    *is_literal = result->token != NULL;
    safe_free(less);
    if (result != dollar) {
        safe_free(result);
    }
    safe_free(dollar);
    safe_free(a);
    destroy_stacks();
    DEBUG_PRINT("EXPR END\n")
    return SUCCESS;
}