#include "token.h"
#include "parser.h"
#include "expr_parser.h"

int table[12][12] = {
        {4,2,2,2,2,2,2,2,4,4,2,2},
        {1,1,2,2,2,2,2,2,1,1,2,2},
        {1,1,2,2,2,2,2,2,1,1,2,2},
        {1,1,1,2,2,2,2,2,1,1,2,2},
        {1,1,1,1,1,2,2,2,1,1,2,2},
        {1,1,1,1,1,4,2,2,1,1,2,2},
        {1,1,1,1,1,1,2,2,1,1,2,2},
        {1,1,1,1,1,1,1,2,1,1,2,2},
        {1,1,1,1,1,1,1,1,1,1,3,4},
        {2,4,2,2,2,2,2,2,4,4,2,2},
        {4,4,2,2,2,2,2,2,4,4,2,2},
        {1,1,1,1,1,1,1,1,1,1,4,4}
}; // 1 is <, 2 is >, 3 is =, 4 is ERROR

int bracket_count = 0;

token_t *local_lookahead;
token_t *after_local_lookahead;

//bool next_nl = false;
//
//static char *tokens_as_str[] = {
//        "TOKEN_IDENTIFIER",
//        "TOKEN_DOUBLE_TYPE",
//        "TOKEN_ELSE",
//        "TOKEN_FUNC",
//        "TOKEN_IF",
//        "TOKEN_INT_TYPE",
//        "TOKEN_LET",
//        "TOKEN_RETURN",
//        "TOKEN_STRING_TYPE",
//        "TOKEN_VAR",
//        "TOKEN_WHILE",
//        "TOKEN_BOOL_TYPE",
//        "TOKEN_FOR",
//        "TOKEN_IN",
//        "TOKEN_BREAK",
//        "TOKEN_CONTINUE",
//        "TOKEN_UNDERSCORE",
//        "TOKEN_ASSIGNMENT",
//        "TOKEN_CLOSED_RANGE",
//        "TOKEN_HALF_OPEN_RANGE",
//        "TOKEN_REAL_LITERAL",
//        "TOKEN_STRING_LITERAL",
//        "TOKEN_NIL_LITERAL",
//        "TOKEN_TRUE_LITERAL",
//        "TOKEN_FALSE_LITERAL",
//        "TOKEN_INTEGER_LITERAL",
//        "TOKEN_ADDITION",
//        "TOKEN_SUBTRACTION",
//        "TOKEN_MULTIPLICATION",
//        "TOKEN_DIVISION",
//        "TOKEN_LESS_THAN",
//        "TOKEN_LESS_THAN_OR_EQUAL_TO",
//        "TOKEN_GREATER_THAN",
//        "TOKEN_GREATER_THAN_OR_EQUAL_TO",
//        "TOKEN_EQUAL_TO",
//        "TOKEN_NOT_EQUAL_TO",
//        "TOKEN_IS_NIL",
//        "TOKEN_UNWRAP_NILLABLE",
//        "TOKEN_LOGICAL_AND",
//        "TOKEN_LOGICAL_OR",
//        "TOKEN_LOGICAL_NOT",
//        "TOKEN_LEFT_BRACKET",
//        "TOKEN_RIGHT_BRACKET",
//        "TOKEN_LEFT_BRACE",
//        "TOKEN_RIGHT_BRACE",
//        "TOKEN_COMMA",
//        "TOKEN_COLON",
//        "TOKEN_SEMICOLON",
//        "TOKEN_ARROW",
//        "TOKEN_NEWLINE",
//        "TOKEN_EOF"
//};
//
//
//typedef enum {
//    INDEX_UNWRAP = 0,            //0
//    INDEX_NOT,               //1
//    INDEX_ADD_SUB,           //2
//    INDEX_MUL_DIV,           //3
//    INDEX_NIL_COL,           //4
//    INDEX_REL_OP,            //5
//    INDEX_AND,               //6
//    INDEX_OR,                //7
//    INDEX_LEFT_BRACKET,      //8
//    INDEX_IDENTIFIER,        //9
//    INDEX_RIGHT_BRACKET,     //10
//    INDEX_DOLLAR,             //11
//    LESS,
//    NON_TERM
//} table_index;
//
//int unwrap = 0;
//int not = 1;
//int add_sub = 2;
//int mul_div = 3;
//int nil_col = 4;
//int rel_op = 5;
//int and = 6;
//int or = 7;
//int left_bracket = 8;
//int identifier = 9;
//int right_bracket = 10;
//int dollar = 11;
//int less = 12;
//int non_term = 13;
//
//stack_t *pushdown_stack = NULL;
//stack_t *tmp_stack = NULL;
//stack_t *token_stack = NULL;
//
//static int map_token(token_t *token) {
//    switch (token->type) {
//        case TOKEN_UNWRAP_NILLABLE:
//            return INDEX_UNWRAP;
//        case TOKEN_LOGICAL_NOT:
//            return INDEX_NOT;
//        case TOKEN_ADDITION:
//        case TOKEN_SUBTRACTION:
//            return INDEX_ADD_SUB;
//        case TOKEN_MULTIPLICATION:
//        case TOKEN_DIVISION:
//            return INDEX_MUL_DIV;
//        case TOKEN_IS_NIL:
//            return INDEX_NIL_COL;
//        case TOKEN_LESS_THAN:
//        case TOKEN_LESS_THAN_OR_EQUAL_TO:
//        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
//        case TOKEN_GREATER_THAN:
//        case TOKEN_EQUAL_TO:
//        case TOKEN_NOT_EQUAL_TO:
//            return INDEX_REL_OP;
//        case TOKEN_LOGICAL_AND:
//            return INDEX_AND;
//        case TOKEN_LOGICAL_OR:
//            return INDEX_OR;
//        case TOKEN_LEFT_BRACKET:
//            if (ignore_right_bracket) {
//                bracket_count++;
//            }
//            return INDEX_LEFT_BRACKET;
//        case TOKEN_IDENTIFIER:
//        case TOKEN_INTEGER_LITERAL:
//        case TOKEN_REAL_LITERAL:
//        case TOKEN_STRING_LITERAL:
//        case TOKEN_NIL_LITERAL:
//        case TOKEN_TRUE_LITERAL:
//        case TOKEN_FALSE_LITERAL:
//            return INDEX_IDENTIFIER;
//        case TOKEN_RIGHT_BRACKET:
//            if (!ignore_right_bracket) {
//                return INDEX_RIGHT_BRACKET;
//            }
//            else {
//                if (bracket_count > 0) {
//                    bracket_count--;
//                    return INDEX_RIGHT_BRACKET;
//                }
//                else {
//                    return INDEX_DOLLAR;
//                }
//            }
//        default:
//            return INDEX_DOLLAR;
//    }
//}
//
//table_index get_top_terminal() {
//    int *top = Stack.top(pushdown_stack);
//    while (*top == non_term || *top == less) {
//        Stack.push(tmp_stack, top);
//        Stack.pop(pushdown_stack);
//        top = Stack.top(pushdown_stack);
//    }
//    while (Stack.top(tmp_stack) != NULL) {
//        Stack.push(pushdown_stack, Stack.top(tmp_stack));
//        Stack.pop(tmp_stack);
//    }
//    return *top;
//}
//
//void clear_until_top_terminal() {
//    int *top = Stack.top(pushdown_stack);
//    while (*top == non_term || *top == less) {
//        Stack.push(tmp_stack, top);
//        Stack.pop(pushdown_stack);
//        top = Stack.top(pushdown_stack);
//    }
//}
//
//void clear_until_less() {
//    int *top = Stack.top(pushdown_stack);
//    while (*top != less) {
//        Stack.push(tmp_stack, top);
//        Stack.pop(pushdown_stack);
//        top = Stack.top(pushdown_stack);
//    }
//    Stack.pop(pushdown_stack);
//}
//
//void clear_tmp_stack() {
//    while (Stack.top(tmp_stack) != NULL) {
//        Stack.pop(tmp_stack);
//    }
//}
//
//void transfer_tmp_stack() {
//    while (Stack.top(tmp_stack) != NULL) {
//        Stack.push(pushdown_stack, Stack.top(tmp_stack));
//        Stack.pop(tmp_stack);
//    }
//}
//
//int parse_expr() {
//    pushdown_stack = Stack.init();
//    tmp_stack = Stack.init();
//    token_stack = Stack.init();
//    Stack.push(pushdown_stack, &dollar);
//
//    bracket_count = 0;
//
//    int *a = malloc(sizeof(int));
//    int *b = malloc(sizeof(int));
//    *a = map_token(lookahead);
//    *b = get_top_terminal();
//
//    while(*a != dollar || *b != dollar) {
//        b = malloc(sizeof(int));
//        if (update_flag) {
//            a = malloc(sizeof(int));
//            *a = map_token(lookahead);
//        }
//        *b = get_top_terminal();
//        update_flag = false;
//        if (*a == INDEX_DOLLAR && *b == INDEX_DOLLAR) {
//            break;
//        }
//
//        int action = table[*b][*a];
//
//        switch (action) {
//            case 1:
//                clear_until_top_terminal();
//                Stack.push(pushdown_stack, &less);
//                transfer_tmp_stack();
//                Stack.push(pushdown_stack, a);
//                Stack.push(token_stack, lookahead);
//                lookahead = TokenArray.next();
//                update_flag = true;
//                break;
//            case 2:
//                clear_until_less();
//                clear_tmp_stack();
//                Stack.push(pushdown_stack, &non_term);
//                break;
//            case 3:
//                Stack.push(pushdown_stack, a);
//                Stack.push(token_stack, lookahead);
//                lookahead = TokenArray.next();
//                update_flag = true;
//                break;
//            default:
//                return SYNTAX_ERROR;
//        }
//    }
//    return SUCCESS;
//}

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

typedef enum {
    int_type = 0,
    double_type,
    string_type,
    bool_type,
    nil_int_type,
    nil_double_type,
    nil_string_type,
    nil_bool_type
} ret_type_enum;

typedef struct {
    expr_elem_enum type;
    token_t *token;
    ret_type_enum ret_type;
} expr_elem_t;

stack_t *pushdown_stack = NULL;
stack_t *tmp_stack = NULL;

static int map_token(token_t *token) {
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
            if (ignore_right_bracket) {
                bracket_count++;
            }
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
                return RIGHT_BRACKET;
            } else {
                if (bracket_count > 0) {
                    bracket_count--;
                    return RIGHT_BRACKET;
                } else {
                    return DOLLAR;
                }
            }
        default:
            return DOLLAR;
    }
}

void init_stacks() {
    pushdown_stack = Stack.init();
    tmp_stack = Stack.init();
}

void destroy_stacks() {
    Stack.destroy(pushdown_stack);
    Stack.destroy(tmp_stack);
}

void clear_tmp_stack() {
    while (Stack.top(tmp_stack) != NULL) {
        Stack.pop(tmp_stack);
    }
}

void clear_until_top_terminal() {
    expr_elem_t *top = Stack.top(pushdown_stack);
    while (top->type == NON_TERM || top->type == LESS) {
        Stack.push(tmp_stack, top);
        Stack.pop(pushdown_stack);
        top = Stack.top(pushdown_stack);
    }
}

void transfer_tmp_stack() {
    while (Stack.top(tmp_stack) != NULL) {
        Stack.push(pushdown_stack, Stack.top(tmp_stack));
        Stack.pop(tmp_stack);
    }
}

void reduce() {
    expr_elem_t *elem = Stack.top(pushdown_stack);
    expr_elem_t **elems = malloc(sizeof(expr_elem_t *) * 3);
    int i = 0;
    while (elem->type != LESS) {
        elems[i] = elem;
        Stack.pop(pushdown_stack);
        elem = Stack.top(pushdown_stack);
        i++;
    }
    Stack.pop(pushdown_stack);
    expr_elem_t *new_elem = malloc(sizeof(expr_elem_t));
    if (i == 1) // reduce E -> i
    {
        if (elems[0]->token->type == TOKEN_INTEGER_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = int_type;
            new_elem->token = NULL;
            printf("PUSHS int@%d\n", elems[0]->token->attribute.integer);
        }
        if (elems[0]->token->type == TOKEN_REAL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = double_type;
            new_elem->token = NULL;
            printf("PUSHS float@%a\n", elems[0]->token->attribute.real);
        }
        if (elems[0]->token->type == TOKEN_STRING_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = string_type;
            new_elem->token = NULL;
            printf("PUSHS string@%s\n", elems[0]->token->attribute.string->str);
        }
        if (elems[0]->token->type == TOKEN_TRUE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = NULL;
            printf("PUSHS bool@true\n");
        }
        if (elems[0]->token->type == TOKEN_FALSE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = NULL;
            printf("PUSHS bool@false\n");
        }
        if (elems[0]->token->type == TOKEN_NIL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = nil_int_type;
            new_elem->token = NULL;
            printf("PUSHS nil@nil\n");
        }
        if (elems[0]->token->type == TOKEN_IDENTIFIER) { // TODO
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[0]->ret_type;
            new_elem->token = NULL;
            printf("PUSHS %s\n", elems[0]->token->attribute.identifier->str);
        }
    }
    if (i == 2) // E -> !E ; E ->E!
    {
        if (elems[1]->type == NOT) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = NULL;
            printf("NOTS\n");
        } else {
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[0]->ret_type;
            new_elem->token = NULL;
        }
    }
    if (i == 3) // E -> E op E
    {
        token_t *op = elems[1]->token;
        if (op == NULL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[1]->ret_type;
            new_elem->token = NULL;
        }
        else {
            switch (op->type) {
                case TOKEN_ADDITION:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = elems[0]->ret_type;
                    new_elem->token = NULL;
                    printf("ADDS\n");
                    break;
                case TOKEN_SUBTRACTION:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = elems[0]->ret_type;
                    new_elem->token = NULL;
                    printf("SUBS\n");
                    break;
                case TOKEN_MULTIPLICATION:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = elems[0]->ret_type;
                    new_elem->token = NULL;
                    printf("MULS\n");
                    break;
                case TOKEN_DIVISION:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = elems[0]->ret_type;
                    new_elem->token = NULL;
                    printf("DIVS\n");
                    break;
                case TOKEN_LOGICAL_AND:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("ANDS\n");
                    break;
                case TOKEN_LOGICAL_OR:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("ORS\n");
                    break;
                case TOKEN_LESS_THAN:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("LTS\n");
                    break;
                case TOKEN_LESS_THAN_OR_EQUAL_TO:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("GTS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_GREATER_THAN:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("GTS\n");
                    break;
                case TOKEN_GREATER_THAN_OR_EQUAL_TO:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("LTS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_EQUAL_TO:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("EQS\n");
                    break;
                case TOKEN_NOT_EQUAL_TO:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("EQS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_IS_NIL:
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("PUSHS nil@nil\n");
                    printf("EQS\n");
                    break;
                default:
                    break;
            }
        }
    }
    Stack.push(pushdown_stack, new_elem);
}

expr_elem_t *get_top_terminal() {
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

expr_elem_t *next_token() {
    expr_elem_t *elem = malloc(sizeof(expr_elem_t));
    elem->type = map_token(local_lookahead);
    elem->token = local_lookahead;
    elem->ret_type = 0;
    return elem;
}

int parse_expr() {
    init_stacks();
    expr_elem_t *dollar = malloc(sizeof(expr_elem_t));
    dollar->type = DOLLAR;
    dollar->token = NULL;
    dollar->ret_type = 0;
    Stack.push(pushdown_stack, dollar);

    expr_elem_t *a = malloc(sizeof(expr_elem_t));
    expr_elem_t *b;

    a->type = map_token(lookahead);
    a->token = lookahead;
    a->ret_type = 0;

    expr_elem_t *less = malloc(sizeof(expr_elem_t));
    less->type = LESS;
    less->token = NULL;
    less->ret_type = 0;

    while (true) {
        b = get_top_terminal();

        if (a->type == DOLLAR && b->type == DOLLAR) {
            break;
        }

        int action = table[b->type][a->type];

        switch (action) {
            case 1:
                clear_until_top_terminal();
                Stack.push(pushdown_stack, less);
                transfer_tmp_stack();
                Stack.push(pushdown_stack, a);
                local_lookahead = TokenArray.next();
                a = next_token();
                break;
            case 2:
                reduce();
                break;
            case 3:
                Stack.push(pushdown_stack, a);
                local_lookahead = TokenArray.next();
                a = next_token();
                break;
            default:
                return SYNTAX_ERROR;
        }
    }
    return SUCCESS;
}