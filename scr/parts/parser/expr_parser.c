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

#define TYPE_ERROR 7

int bracket_count = 0;

bool ignore_right_bracket = false;

token_t *local_lookahead;
token_t *after_local_lookahead;

#define type_check() if (a->ret_type != b->ret_type \
&& (a->ret_type != int_type && b->ret_type != double_type) \
&& (a->ret_type != double_type && b->ret_type != int_type)) { \
fprintf(stderr, "Error: EXPR type mismatch.\n"); \
exit(TYPE_ERROR); \
} \
new_elem->type = NON_TERM; \
if (a->ret_type == int_type && b->ret_type == int_type) { \
new_elem->ret_type = int_type; \
} else if (a->ret_type == double_type && b->ret_type == double_type) { \
new_elem->ret_type = double_type;\
} else if (a->ret_type == int_type && b->ret_type == double_type) {\
if (a->token == NULL) {\
fprintf(stderr, "ERROR: cant retype VAR (a)");\
exit(TYPE_ERROR);\
}\
new_elem->ret_type = double_type;\
\
} else if (a->ret_type == double_type && b->ret_type == int_type) {\
if (b->token == NULL) {\
fprintf(stderr, "ERROR: cant retype VAR (b)");\
exit(TYPE_ERROR);\
}\
new_elem->ret_type = double_type;\
\
} else {\
new_elem->ret_type = elems[0]->ret_type;\
}\
new_elem->token = (token_t*) ((int)a->token * (int)b->token);\


bool nl_flag = false;

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
    nil_bool_type,
    nil_type
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
        if (elems[0]->token == NULL){
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[0]->ret_type;
            new_elem->token = NULL;
            printf("PUSHS GF@$RET\n");
        }
        else if (elems[0]->token->type == TOKEN_INTEGER_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = int_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS int@%d\n", elems[0]->token->attribute.integer);
        }
        else if (elems[0]->token->type == TOKEN_REAL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = double_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS float@%a\n", elems[0]->token->attribute.real);
        }
        else if (elems[0]->token->type == TOKEN_STRING_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = string_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS string@%s\n", elems[0]->token->attribute.string->str);
        }
        else if (elems[0]->token->type == TOKEN_TRUE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS bool@true\n");
        }
        else if (elems[0]->token->type == TOKEN_FALSE_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS bool@false\n");
        }
        else if (elems[0]->token->type == TOKEN_NIL_LITERAL) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = nil_type;
            new_elem->token = (token_t *) 1;
            printf("PUSHS nil@nil\n");
        }
        else if (elems[0]->token->type == TOKEN_IDENTIFIER) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = int_type; // TODO get type from symtable
            new_elem->token = NULL;
            printf("PUSHS %s\n", elems[0]->token->attribute.identifier->str);
        }
    }
    else if (i == 2) // E -> !E ; E ->E!
    {
        if (elems[1]->type == NOT) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = bool_type;
            new_elem->token = NULL;
            printf("NOTS\n");
        } else {
            if (elems[0]->ret_type < 4) {
                fprintf(stderr, "Error: EXPR type mismatch.\n");
                exit(TYPE_ERROR);
            }
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[0]->ret_type - 4;
            new_elem->token = NULL;
        }
    }
    else if (i == 3) // E -> E op E
    {
        token_t *op = elems[1]->token;
        if (op == NULL || op == (token_t *) 1) {
            new_elem->type = NON_TERM;
            new_elem->ret_type = elems[1]->ret_type;
            new_elem->token = elems[1]->token;
        }
        else {
            expr_elem_t *a = elems[0];
            expr_elem_t *b = elems[2];
//            DEBUG_PRINT("a: %d, b: %d\n", a->ret_type, b->ret_type);
            switch (op->type) {
                case TOKEN_ADDITION:
                    type_check()
                    //DEBUG_PRINT("E: %d, A: %d, B: %d\n", (int)new_elem->token, (int)a->token, (int)b->token);
                    if (new_elem->ret_type == string_type){
                        printf("POPS GF@$B\n");
                        printf("POPS GF@$A\n");
                        printf("CONCAT GF@$RET GF@$A GF@$B\n");
                        printf("PUSHS GF@$RET\n");
                    } else {
                        printf("ADDS\n");
                    }
                    break;
                case TOKEN_SUBTRACTION:
                    type_check()
                    if (a->ret_type == string_type || a->ret_type == bool_type) {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    printf("SUBS\n");
                    break;
                case TOKEN_MULTIPLICATION:
                    type_check()
                    if (a->ret_type == string_type || a->ret_type == bool_type) {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    printf("MULS\n");
                    break;
                case TOKEN_DIVISION:
                    type_check()
                    if (a->ret_type == string_type || a->ret_type == bool_type) {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    printf("DIVS\n");
                    break;
                case TOKEN_LOGICAL_AND:
                    if (a->ret_type != bool_type || a->ret_type != b->ret_type) {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("ANDS\n");
                    break;
                case TOKEN_LOGICAL_OR:
                    if (a->ret_type != bool_type || a->ret_type != b->ret_type)  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("ORS\n");
                    break;
                case TOKEN_LESS_THAN:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type) || a->ret_type == bool_type)  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("LTS\n");
                    break;
                case TOKEN_LESS_THAN_OR_EQUAL_TO:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type) || a->ret_type == bool_type)  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("GTS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_GREATER_THAN:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type) || a->ret_type == bool_type)  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("GTS\n");
                    break;
                case TOKEN_GREATER_THAN_OR_EQUAL_TO:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type) || a->ret_type == bool_type)  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("LTS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_EQUAL_TO:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type))  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("EQS\n");
                    break;
                case TOKEN_NOT_EQUAL_TO:
                    if (a->ret_type != b->ret_type || (a->ret_type > 3 && a->ret_type != nil_type))  {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    new_elem->ret_type = bool_type;
                    new_elem->token = NULL;
                    printf("EQS\n");
                    printf("NOTS\n");
                    break;
                case TOKEN_IS_NIL:
                    if (a->ret_type < 4 || !(a->ret_type - 4 == b->ret_type || b->ret_type - 4 == a->ret_type)) {
                        fprintf(stderr, "Error: EXPR type mismatch.\n");
                        exit(TYPE_ERROR);
                    }
                    new_elem->type = NON_TERM;
                    if (a->ret_type > 3) {
                        new_elem->ret_type = a->ret_type - 4;
                    } else {
                        new_elem->ret_type = a->ret_type;
                    }
                    new_elem->token = NULL;
                    printf("???????\n");
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

bool first_flag = false;

expr_elem_t *next_token() {
    token_t *current = local_lookahead;
    token_t *next = after_local_lookahead;
    bool is_last_on_line = current->has_newline_after;

    if (!first_flag) {
        local_lookahead = after_local_lookahead;
        after_local_lookahead = TokenArray.next();
    }

    // if is_last_on_line, then prefetch action, if error action, then return DOLLAR
    if (is_last_on_line && !first_flag) {
        expr_elem_t *a = malloc(sizeof(expr_elem_t));
        a->type = map_token(next);
        expr_elem_t *b = get_top_terminal();
        int action = table[b->type][a->type];
        if (action == 4) {
//            local_lookahead = after_local_lookahead;
//            after_local_lookahead = TokenArray.next();
            expr_elem_t *dollar = malloc(sizeof(expr_elem_t));
            dollar->type = DOLLAR;
            dollar->token = NULL;
            dollar->ret_type = 0;
            return dollar;
        }
    }

    if (current->type == TOKEN_IDENTIFIER) {
        if (next->type == TOKEN_LEFT_BRACKET) {
            lookahead = TokenArray.next();
            bool tmp = ignore_right_bracket;
            ignore_right_bracket = true;
            stack_t *tmp_1 = pushdown_stack;
            stack_t *tmp_2 = tmp_stack;
            CALL_PARAM_LIST();
            pushdown_stack = tmp_1;
            tmp_stack = tmp_2;
            ignore_right_bracket = tmp;
            printf("CALL %s\n", current->attribute.identifier->str);
            local_lookahead = lookahead;
            after_local_lookahead = TokenArray.next();
            expr_elem_t *elem = malloc(sizeof(expr_elem_t));
            elem->type = IDENTIFIER;
            elem->token = NULL;
            elem->ret_type = 0; // TODO get ret func type from symtable
            return elem;
        }
    }
    expr_elem_t *elem = malloc(sizeof(expr_elem_t));
    elem->type = map_token(local_lookahead);
    elem->token = local_lookahead;
    elem->ret_type = 0;
    return elem;
}

int parse_expr(char *type) {
    init_stacks();
    expr_elem_t *dollar = malloc(sizeof(expr_elem_t));
    dollar->type = DOLLAR;
    dollar->token = NULL;
    dollar->ret_type = 0;
    Stack.push(pushdown_stack, dollar);

    TokenArray.prev();

    local_lookahead = TokenArray.next();
    after_local_lookahead = TokenArray.next();

    expr_elem_t *a;
    expr_elem_t *b;

    first_flag = true;

    a = next_token();

    first_flag = false;

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
                a = next_token();
                break;
            case 2:
                reduce();
                break;
            case 3:
                Stack.push(pushdown_stack, a);
                a = next_token();
                break;
            default:
                fprintf(stderr, "Error: EXPR syntax error.\n");
                return SYNTAX_ERROR;
        }
    }
    lookahead = TokenArray.prev();
    expr_elem_t *result = Stack.top(pushdown_stack);
    switch (result->ret_type) {
        case int_type:
            *type = 'i';
            break;
        case double_type:
            *type = 'f';
            break;
        case string_type:
            *type = 's';
            break;
        case bool_type:
            *type = 'b';
            break;
        case nil_type:
            *type = 'n';
            break;
        default:
            fprintf(stderr, "Error: EXPR internal. %d\n", result->ret_type);
            exit(99);
    }
//    DEBUG_PRINT("EXPR: %c\n", *type);
    destroy_stacks();
    return SUCCESS;
}