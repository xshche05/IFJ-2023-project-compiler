//
// Created by Spagetik on 22.10.2023.
//

// valid tokens for expression parser id-non-nil, int_literal, double_literal, str_literal, nil_literal, true_literal, false_literal
// (, ), +, -, *, /, <, <=, >, >=, ==, !=, &&, ||, !_, _!

#include "expr_parser.h"
#include "token/token.h"
#include "stack.h"


token_t *previous_token = NULL;
token_t *current_token = NULL;
token_t *next_token = NULL;

void next() {
    token_t *tmp = TokenArray.next();
    while (tmp != NULL && tmp->type == TOKEN_NEWLINE) {
        tmp = TokenArray.next();
    }
    previous_token = current_token;
    current_token = next_token;
    next_token = tmp;
}

bool validate_end() {
    if (previous_token->type == TOKEN_LOGICAL_NOT) return false;
    if (previous_token->type == TOKEN_LEFT_BRACKET) return false;
    if (previous_token->type == TOKEN_ADDITION) return false;
    if (previous_token->type == TOKEN_SUBTRACTION) return false;
    if (previous_token->type == TOKEN_MULTIPLICATION) return false;
    if (previous_token->type == TOKEN_DIVISION) return false;
    if (previous_token->type == TOKEN_LESS_THAN) return false;
    if (previous_token->type == TOKEN_LESS_THAN_OR_EQUAL_TO) return false;
    if (previous_token->type == TOKEN_GREATER_THAN) return false;
    if (previous_token->type == TOKEN_GREATER_THAN_OR_EQUAL_TO) return false;
    if (previous_token->type == TOKEN_EQUAL_TO) return false;
    if (previous_token->type == TOKEN_NOT_EQUAL_TO) return false;
    if (previous_token->type == TOKEN_LOGICAL_AND) return false;
    if (previous_token->type == TOKEN_LOGICAL_OR) return false;
    if (previous_token->type == TOKEN_LOGICAL_NOT) return false;
    return true;
}

bool continue_parse_expr() {
    // if previous token is ID or RIGHT_BRACKET or INT_LITERAL or DOUBLE_LITERAL or STR_LITERAL or NIL_LITERAL or TRUE_LITERAL or FALSE_LITERAL or UNWRAP_NILABLE
    // and next token is OPERATOR or LEFT_BRACKET return true

    // if previous token is OPERATOR (exept UNWRAP_NILABLE) and next token is ID or LEFT_BRACKET or INT_LITERAL or DOUBLE_LITERAL or STR_LITERAL or NIL_LITERAL or TRUE_LITERAL or FALSE_LITERAL or UNWRAP_NILABLE return true

    // else return false
    if (previous_token == NULL) {
        return true;
    } else if (previous_token->type == TOKEN_IDENTIFIER || previous_token->type == TOKEN_RIGHT_BRACKET || previous_token->type == TOKEN_INTEGER_LITERAL || previous_token->type == TOKEN_REAL_LITERAL || previous_token->type == TOKEN_STRING_LITERAL || previous_token->type == TOKEN_NIL_LITERAL || previous_token->type == TOKEN_TRUE_LITERAL || previous_token->type == TOKEN_FALSE_LITERAL) {
        if (current_token->type == TOKEN_ADDITION || current_token->type == TOKEN_SUBTRACTION || current_token->type == TOKEN_MULTIPLICATION || current_token->type == TOKEN_DIVISION || current_token->type == TOKEN_LESS_THAN || current_token->type == TOKEN_LESS_THAN_OR_EQUAL_TO || current_token->type == TOKEN_GREATER_THAN || current_token->type == TOKEN_GREATER_THAN_OR_EQUAL_TO || current_token->type == TOKEN_EQUAL_TO || current_token->type == TOKEN_NOT_EQUAL_TO || current_token->type == TOKEN_LOGICAL_AND || current_token->type == TOKEN_LOGICAL_OR || current_token->type == TOKEN_LOGICAL_NOT || current_token->type == TOKEN_RIGHT_BRACKET || current_token->type == TOKEN_UNWRAP_NILABLE) {
            return true;
        }
    } else if (previous_token->type == TOKEN_ADDITION || previous_token->type == TOKEN_SUBTRACTION || previous_token->type == TOKEN_MULTIPLICATION || previous_token->type == TOKEN_DIVISION || previous_token->type == TOKEN_LESS_THAN || previous_token->type == TOKEN_LESS_THAN_OR_EQUAL_TO || previous_token->type == TOKEN_GREATER_THAN || previous_token->type == TOKEN_GREATER_THAN_OR_EQUAL_TO || previous_token->type == TOKEN_EQUAL_TO || previous_token->type == TOKEN_NOT_EQUAL_TO || previous_token->type == TOKEN_LOGICAL_AND || previous_token->type == TOKEN_LOGICAL_OR || previous_token->type == TOKEN_LOGICAL_NOT || previous_token->type == TOKEN_LEFT_BRACKET) {
        if (current_token->type == TOKEN_IDENTIFIER || current_token->type == TOKEN_LEFT_BRACKET || current_token->type == TOKEN_INTEGER_LITERAL || current_token->type == TOKEN_REAL_LITERAL || current_token->type == TOKEN_STRING_LITERAL || current_token->type == TOKEN_NIL_LITERAL || current_token->type == TOKEN_TRUE_LITERAL || current_token->type == TOKEN_FALSE_LITERAL || current_token->type == TOKEN_LOGICAL_NOT) {
            return true;
        }
    } else if (previous_token->type == TOKEN_UNWRAP_NILABLE) {
        if (current_token->type == TOKEN_ADDITION || current_token->type == TOKEN_SUBTRACTION || current_token->type == TOKEN_MULTIPLICATION || current_token->type == TOKEN_DIVISION || current_token->type == TOKEN_LESS_THAN || current_token->type == TOKEN_LESS_THAN_OR_EQUAL_TO || current_token->type == TOKEN_GREATER_THAN || current_token->type == TOKEN_GREATER_THAN_OR_EQUAL_TO || current_token->type == TOKEN_EQUAL_TO || current_token->type == TOKEN_NOT_EQUAL_TO || current_token->type == TOKEN_LOGICAL_AND || current_token->type == TOKEN_LOGICAL_OR || current_token->type == TOKEN_LOGICAL_NOT || current_token->type == TOKEN_LEFT_BRACKET) {
            return true;
        }
    } else if (current_token->type == TOKEN_UNWRAP_NILABLE) {
        if (previous_token->type == TOKEN_IDENTIFIER || previous_token->type == TOKEN_RIGHT_BRACKET) {
            return true;
        }
    } else if (previous_token->type == TOKEN_LOGICAL_NOT) {
        if (current_token->type == TOKEN_IDENTIFIER || current_token->type == TOKEN_LEFT_BRACKET || current_token->type == TOKEN_TRUE_LITERAL || current_token->type == TOKEN_FALSE_LITERAL) {
            return true;
        }
    }
    return false;
}

// operator priority_table
static int get_op_priority(token_t *token) {
    if (token == NULL) {
        return -1;
    }
    switch (token->type) {
        case TOKEN_LOGICAL_NOT:
            return 0;
        case TOKEN_MULTIPLICATION:
        case TOKEN_DIVISION:
            return 1;
        case TOKEN_ADDITION:
        case TOKEN_SUBTRACTION:
            return 2;
        case TOKEN_EQUAL_TO:
        case TOKEN_NOT_EQUAL_TO:
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            return 3;
        case TOKEN_LOGICAL_AND:
            return 4;
        case TOKEN_LOGICAL_OR:
            return 5;
        default:
            return -1;
    }
}


bool parse_expr(bool none) {
    previous_token = NULL;
    current_token = lookahead;
    next_token = TokenArray.next();

    token_t *tmp_token = NULL;

    stack_t *operator_stack = Stack.init();
    stack_t *expr_stack = Stack.init();

    int brackets = 0;

    while (continue_parse_expr()) {
        switch (current_token->type) {
            case TOKEN_IDENTIFIER:
            case TOKEN_INTEGER_LITERAL:
            case TOKEN_REAL_LITERAL:
            case TOKEN_STRING_LITERAL:
            case TOKEN_NIL_LITERAL:
            case TOKEN_TRUE_LITERAL:
            case TOKEN_FALSE_LITERAL:
            case TOKEN_UNWRAP_NILABLE:
                Stack.push(expr_stack, current_token);
                break;
            case TOKEN_LEFT_BRACKET:
                // if previous token is ID - function call
                // else - expression
                if (previous_token->type == TOKEN_IDENTIFIER) {
                    // function call
                    Stack.push(expr_stack, current_token);
                    while (current_token->type != TOKEN_RIGHT_BRACKET) {
                        next();
                        Stack.push(expr_stack, current_token);
                    }
                } else {
                    // expression
                    brackets++;
                    Stack.push(operator_stack, current_token);
                }
                break;
            case TOKEN_RIGHT_BRACKET:
                tmp_token = Stack.top(operator_stack);
                while (tmp_token->type != TOKEN_LEFT_BRACKET) {
                    Stack.push(expr_stack, tmp_token);
                    Stack.pop(operator_stack);
                    tmp_token = Stack.top(operator_stack);
                }
                brackets--;
                Stack.pop(operator_stack);
                break;
            case TOKEN_ADDITION:
            case TOKEN_SUBTRACTION:
            case TOKEN_MULTIPLICATION:
            case TOKEN_DIVISION:
            case TOKEN_LESS_THAN:
            case TOKEN_LESS_THAN_OR_EQUAL_TO:
            case TOKEN_GREATER_THAN:
            case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            case TOKEN_EQUAL_TO:
            case TOKEN_NOT_EQUAL_TO:
            case TOKEN_LOGICAL_AND:
            case TOKEN_LOGICAL_OR:
            case TOKEN_LOGICAL_NOT:
                while (true) {
                    tmp_token = Stack.top(operator_stack);
                    if (tmp_token == NULL) {
                        break;
                    }
                    if (tmp_token->type == TOKEN_LEFT_BRACKET) {
                        break;
                    }
                    if (get_op_priority(tmp_token) >= get_op_priority(current_token)) {
                        break;
                    }
                    Stack.pop(operator_stack);
                    Stack.push(expr_stack, tmp_token);
                }
                Stack.push(operator_stack, current_token);
                break;
            default:
                return false;
        }
        next();
    }

    while (Stack.top(operator_stack) != NULL) {
        Stack.push(expr_stack, Stack.top(operator_stack));
        Stack.pop(operator_stack);
    }

    // reverse expr_stack
    stack_t *tmp_stack = Stack.init();
    while (Stack.top(expr_stack) != NULL) {
        Stack.push(tmp_stack, Stack.top(expr_stack));
        Stack.pop(expr_stack);
    }

    // print expr_stack
    while (Stack.top(tmp_stack) != NULL) {
        Token.print(Stack.top(tmp_stack));
        Stack.pop(tmp_stack);
    }
    return validate_end();
}

