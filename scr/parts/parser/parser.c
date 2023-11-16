#include "parts.h"
#include "utils.h"
#include "new_symtable.h"

static char *tokens_as_str[] = {
        "TOKEN_IDENTIFIER",
        "TOKEN_DOUBLE_TYPE",
        "TOKEN_ELSE",
        "TOKEN_FUNC",
        "TOKEN_IF",
        "TOKEN_INT_TYPE",
        "TOKEN_LET",
        "TOKEN_RETURN",
        "TOKEN_STRING_TYPE",
        "TOKEN_VAR",
        "TOKEN_WHILE",
        "TOKEN_BOOL_TYPE",
        "TOKEN_FOR",
        "TOKEN_IN",
        "TOKEN_BREAK",
        "TOKEN_CONTINUE",
        "TOKEN_UNDERSCORE",
        "TOKEN_ASSIGNMENT",
        "TOKEN_CLOSED_RANGE",
        "TOKEN_HALF_OPEN_RANGE",
        "TOKEN_REAL_LITERAL",
        "TOKEN_STRING_LITERAL",
        "TOKEN_NIL_LITERAL",
        "TOKEN_TRUE_LITERAL",
        "TOKEN_FALSE_LITERAL",
        "TOKEN_INTEGER_LITERAL",
        "TOKEN_ADDITION",
        "TOKEN_SUBTRACTION",
        "TOKEN_MULTIPLICATION",
        "TOKEN_DIVISION",
        "TOKEN_LESS_THAN",
        "TOKEN_LESS_THAN_OR_EQUAL_TO",
        "TOKEN_GREATER_THAN",
        "TOKEN_GREATER_THAN_OR_EQUAL_TO",
        "TOKEN_EQUAL_TO",
        "TOKEN_NOT_EQUAL_TO",
        "TOKEN_IS_NIL",
        "TOKEN_UNWRAP_NILABLE",
        "TOKEN_LOGICAL_AND",
        "TOKEN_LOGICAL_OR",
        "TOKEN_LOGICAL_NOT",
        "TOKEN_LEFT_BRACKET",
        "TOKEN_RIGHT_BRACKET",
        "TOKEN_LEFT_BRACE",
        "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA",
        "TOKEN_COLON",
        "TOKEN_SEMICOLON",
        "TOKEN_ARROW",
        "TOKEN_NEWLINE",
        "TOKEN_EOF"
};
    
bool match(token_type_t type) {
    if (lookahead->type == type) {
        lookahead = TokenArray.next();
        return true;
    }
    printf("Syntax error: expected %s, got %s\n", "dd", "dd");
    return false;
}

bool call_expr_parser() {
    return true;
}
    
bool S() {
    bool s;
    lookahead = TokenArray.next();
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_IF:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_FUNC:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_LET:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_BREAK:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_RETURN:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_CONTINUE:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_EOF:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_WHILE:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_FOR:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        case TOKEN_IDENTIFIER:
            s = CODE() && match(TOKEN_EOF) && true;
            break;
        default:
            printf("Syntax error [S]: expected ['TOKEN_VAR', 'TOKEN_IF', 'TOKEN_FUNC', 'TOKEN_LET', 'TOKEN_BREAK', 'TOKEN_RETURN', 'TOKEN_CONTINUE', 'TOKEN_EOF', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool CODE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = VAR_DECL() && CODE() && true;
            break;
        case TOKEN_LET:
            s = LET_DECL() && CODE() && true;
            break;
        case TOKEN_FUNC:
            s = FUNC_DECL() && CODE() && true;
            break;
        case TOKEN_WHILE:
            s = WHILE_LOOP() && CODE() && true;
            break;
        case TOKEN_FOR:
            s = FOR_LOOP() && CODE() && true;
            break;
        case TOKEN_IF:
            s = BRANCH() && CODE() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = ID_CALL_OR_ASSIGN() && CODE() && true;
            break;
        case TOKEN_RETURN:
            s = RETURN() && true;
            break;
        case TOKEN_BREAK:
            s = match(TOKEN_BREAK) && true;
            break;
        case TOKEN_CONTINUE:
            s = match(TOKEN_CONTINUE) && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        default:
            printf("Syntax error [CODE]: expected ['TOKEN_VAR', 'TOKEN_LET', 'TOKEN_FUNC', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_IF', 'TOKEN_IDENTIFIER', 'TOKEN_RETURN', 'TOKEN_BREAK', 'TOKEN_CONTINUE', 'TOKEN_RIGHT_BRACE', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool RETURN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_RETURN:
            s = match(TOKEN_RETURN) && RET_EXPR() && true;
            break;
        default:
            printf("Syntax error [RETURN]: expected ['TOKEN_RETURN'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool RET_EXPR() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LOGICAL_OR:
            s = EXPR() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_IS_NIL:
            s = EXPR() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_ADDITION:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_REAL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_DIVISION:
            s = EXPR() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = EXPR() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = EXPR() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = EXPR() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = EXPR() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = EXPR() && true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        default:
            printf("Syntax error [RET_EXPR]: expected ['TOKEN_LOGICAL_OR', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_GREATER_THAN', 'TOKEN_FALSE_LITERAL', 'TOKEN_ADDITION', 'TOKEN_LESS_THAN', 'TOKEN_EQUAL_TO', 'TOKEN_STRING_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_REAL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_LEFT_BRACE', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_DIVISION', 'TOKEN_SUBTRACTION', 'TOKEN_NIL_LITERAL', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_LOGICAL_AND', 'TOKEN_IDENTIFIER', 'TOKEN_MULTIPLICATION', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool VAR_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = match(TOKEN_VAR) && match(TOKEN_IDENTIFIER) && VAR_LET_TYPE() && VAR_LET_EXP() && true;
            break;
        default:
            printf("Syntax error [VAR_DECL]: expected ['TOKEN_VAR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool VAR_LET_TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COLON:
            s = match(TOKEN_COLON) && TYPE() && true;
            break;
        case TOKEN_VAR:
            s = true;
            break;
        case TOKEN_IF:
            s = true;
            break;
        case TOKEN_FUNC:
            s = true;
            break;
        case TOKEN_ASSIGNMENT:
            s = true;
            break;
        case TOKEN_BREAK:
            s = true;
            break;
        case TOKEN_LET:
            s = true;
            break;
        case TOKEN_CONTINUE:
            s = true;
            break;
        case TOKEN_RETURN:
            s = true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        case TOKEN_WHILE:
            s = true;
            break;
        case TOKEN_FOR:
            s = true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_IDENTIFIER:
            s = true;
            break;
        default:
            printf("Syntax error [VAR_LET_TYPE]: expected ['TOKEN_COLON', 'TOKEN_VAR', 'TOKEN_IF', 'TOKEN_FUNC', 'TOKEN_ASSIGNMENT', 'TOKEN_BREAK', 'TOKEN_LET', 'TOKEN_CONTINUE', 'TOKEN_RETURN', 'TOKEN_EOF', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_RIGHT_BRACE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool VAR_LET_EXP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT) && EXPR() && true;
            break;
        case TOKEN_VAR:
            s = true;
            break;
        case TOKEN_IF:
            s = true;
            break;
        case TOKEN_FUNC:
            s = true;
            break;
        case TOKEN_BREAK:
            s = true;
            break;
        case TOKEN_LET:
            s = true;
            break;
        case TOKEN_CONTINUE:
            s = true;
            break;
        case TOKEN_RETURN:
            s = true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        case TOKEN_WHILE:
            s = true;
            break;
        case TOKEN_FOR:
            s = true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_IDENTIFIER:
            s = true;
            break;
        default:
            printf("Syntax error [VAR_LET_EXP]: expected ['TOKEN_ASSIGNMENT', 'TOKEN_VAR', 'TOKEN_IF', 'TOKEN_FUNC', 'TOKEN_BREAK', 'TOKEN_LET', 'TOKEN_CONTINUE', 'TOKEN_RETURN', 'TOKEN_EOF', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_RIGHT_BRACE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool LET_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET) && match(TOKEN_IDENTIFIER) && VAR_LET_TYPE() && VAR_LET_EXP() && true;
            break;
        default:
            printf("Syntax error [LET_DECL]: expected ['TOKEN_LET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool FUNC_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_FUNC:
            s = match(TOKEN_FUNC) && match(TOKEN_IDENTIFIER) && match(TOKEN_LEFT_BRACKET) && PARAM_LIST() && match(TOKEN_RIGHT_BRACKET) && FUNC_RET_TYPE() && match(TOKEN_LEFT_BRACE) && CODE() && match(TOKEN_RIGHT_BRACE) && true;
            break;
        default:
            printf("Syntax error [FUNC_DECL]: expected ['TOKEN_FUNC'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool FUNC_RET_TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ARROW:
            s = match(TOKEN_ARROW) && TYPE() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = true;
            break;
        default:
            printf("Syntax error [FUNC_RET_TYPE]: expected ['TOKEN_ARROW', 'TOKEN_LEFT_BRACE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool PARAM_LIST() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_UNDERSCORE:
            s = PARAM() && NEXT_PARAM() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = PARAM() && NEXT_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            printf("Syntax error [PARAM_LIST]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_UNDERSCORE:
            s = PARAM_NAME() && match(TOKEN_IDENTIFIER) && match(TOKEN_COLON) && TYPE() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = PARAM_NAME() && match(TOKEN_IDENTIFIER) && match(TOKEN_COLON) && TYPE() && true;
            break;
        default:
            printf("Syntax error [PARAM]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool PARAM_NAME() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER) && true;
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE) && true;
            break;
        default:
            printf("Syntax error [PARAM_NAME]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool NEXT_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA) && PARAM() && NEXT_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            printf("Syntax error [NEXT_PARAM]: expected ['TOKEN_COMMA', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool BRANCH() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IF:
            s = match(TOKEN_IF) && BR_EXPR() && match(TOKEN_LEFT_BRACE) && CODE() && match(TOKEN_RIGHT_BRACE) && ELSE() && true;
            break;
        default:
            printf("Syntax error [BRANCH]: expected ['TOKEN_IF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool BR_EXPR() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LOGICAL_OR:
            s = EXPR() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_IS_NIL:
            s = EXPR() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_ADDITION:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = EXPR() && true;
            break;
        case TOKEN_REAL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_DIVISION:
            s = EXPR() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = EXPR() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = EXPR() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = EXPR() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = EXPR() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = EXPR() && true;
            break;
        case TOKEN_LET:
            s = match(TOKEN_LET) && match(TOKEN_IDENTIFIER) && true;
            break;
        default:
            printf("Syntax error [BR_EXPR]: expected ['TOKEN_LOGICAL_OR', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_GREATER_THAN', 'TOKEN_FALSE_LITERAL', 'TOKEN_ADDITION', 'TOKEN_LESS_THAN', 'TOKEN_EQUAL_TO', 'TOKEN_STRING_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_REAL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_LEFT_BRACE', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_DIVISION', 'TOKEN_SUBTRACTION', 'TOKEN_NIL_LITERAL', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_LOGICAL_AND', 'TOKEN_IDENTIFIER', 'TOKEN_MULTIPLICATION', 'TOKEN_LET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool ELSE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ELSE:
            s = match(TOKEN_ELSE) && ELSE_IF() && true;
            break;
        case TOKEN_VAR:
            s = true;
            break;
        case TOKEN_IF:
            s = true;
            break;
        case TOKEN_FUNC:
            s = true;
            break;
        case TOKEN_BREAK:
            s = true;
            break;
        case TOKEN_LET:
            s = true;
            break;
        case TOKEN_CONTINUE:
            s = true;
            break;
        case TOKEN_RETURN:
            s = true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        case TOKEN_WHILE:
            s = true;
            break;
        case TOKEN_FOR:
            s = true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_IDENTIFIER:
            s = true;
            break;
        default:
            printf("Syntax error [ELSE]: expected ['TOKEN_ELSE', 'TOKEN_VAR', 'TOKEN_IF', 'TOKEN_FUNC', 'TOKEN_BREAK', 'TOKEN_LET', 'TOKEN_CONTINUE', 'TOKEN_RETURN', 'TOKEN_EOF', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_RIGHT_BRACE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool ELSE_IF() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IF:
            s = true;
            break;
        case TOKEN_LEFT_BRACE:
            s = match(TOKEN_LEFT_BRACE) && CODE() && match(TOKEN_RIGHT_BRACE) && true;
            break;
        case TOKEN_VAR:
            s = true;
            break;
        case TOKEN_FUNC:
            s = true;
            break;
        case TOKEN_BREAK:
            s = true;
            break;
        case TOKEN_LET:
            s = true;
            break;
        case TOKEN_CONTINUE:
            s = true;
            break;
        case TOKEN_RETURN:
            s = true;
            break;
        case TOKEN_EOF:
            s = true;
            break;
        case TOKEN_WHILE:
            s = true;
            break;
        case TOKEN_FOR:
            s = true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = true;
            break;
        case TOKEN_IDENTIFIER:
            s = true;
            break;
        default:
            printf("Syntax error [ELSE_IF]: expected ['TOKEN_IF', 'TOKEN_LEFT_BRACE', 'TOKEN_VAR', 'TOKEN_FUNC', 'TOKEN_BREAK', 'TOKEN_LET', 'TOKEN_CONTINUE', 'TOKEN_RETURN', 'TOKEN_EOF', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_RIGHT_BRACE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool WHILE_LOOP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_WHILE:
            s = match(TOKEN_WHILE) && EXPR() && match(TOKEN_LEFT_BRACE) && CODE() && match(TOKEN_RIGHT_BRACE) && true;
            break;
        default:
            printf("Syntax error [WHILE_LOOP]: expected ['TOKEN_WHILE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool FOR_LOOP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_FOR:
            s = match(TOKEN_FOR) && FOR_ID() && match(TOKEN_IN) && EXPR() && RANGE() && match(TOKEN_LEFT_BRACE) && CODE() && match(TOKEN_RIGHT_BRACE) && true;
            break;
        default:
            printf("Syntax error [FOR_LOOP]: expected ['TOKEN_FOR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool FOR_ID() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER) && true;
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE) && true;
            break;
        default:
            printf("Syntax error [FOR_ID]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool RANGE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_CLOSED_RANGE:
            s = match(TOKEN_CLOSED_RANGE) && EXPR() && true;
            break;
        case TOKEN_HALF_OPEN_RANGE:
            s = match(TOKEN_HALF_OPEN_RANGE) && EXPR() && true;
            break;
        default:
            printf("Syntax error [RANGE]: expected ['TOKEN_CLOSED_RANGE', 'TOKEN_HALF_OPEN_RANGE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool CALL_PARAM_LIST() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_UNDERSCORE:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_OR:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_IS_NIL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_ADDITION:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LESS_THAN:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_REAL_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_DIVISION:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            printf("Syntax error [CALL_PARAM_LIST]: expected ['TOKEN_UNDERSCORE', 'TOKEN_LOGICAL_OR', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_GREATER_THAN', 'TOKEN_FALSE_LITERAL', 'TOKEN_ADDITION', 'TOKEN_LESS_THAN', 'TOKEN_EQUAL_TO', 'TOKEN_STRING_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_REAL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_LEFT_BRACE', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_DIVISION', 'TOKEN_SUBTRACTION', 'TOKEN_NIL_LITERAL', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_LOGICAL_AND', 'TOKEN_IDENTIFIER', 'TOKEN_MULTIPLICATION', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_UNDERSCORE:
            s = NAMED_CALL_PARAM() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_OR:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_IS_NIL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_ADDITION:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LESS_THAN:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_REAL_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_DIVISION:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = POS_CALL_PARAM() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = POS_CALL_PARAM() && true;
            break;
        default:
            printf("Syntax error [CALL_PARAM]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER', 'TOKEN_LOGICAL_OR', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_GREATER_THAN', 'TOKEN_FALSE_LITERAL', 'TOKEN_ADDITION', 'TOKEN_LESS_THAN', 'TOKEN_EQUAL_TO', 'TOKEN_STRING_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_REAL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_LEFT_BRACE', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_DIVISION', 'TOKEN_SUBTRACTION', 'TOKEN_NIL_LITERAL', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_LOGICAL_AND', 'TOKEN_MULTIPLICATION'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool NAMED_CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_UNDERSCORE:
            s = CALL_PARAM_NAME() && match(TOKEN_COLON) && EXPR() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = CALL_PARAM_NAME() && match(TOKEN_COLON) && EXPR() && true;
            break;
        default:
            printf("Syntax error [NAMED_CALL_PARAM]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool CALL_PARAM_NAME() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER) && true;
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE) && true;
            break;
        default:
            printf("Syntax error [CALL_PARAM_NAME]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool POS_CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LOGICAL_OR:
            s = EXPR() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_IS_NIL:
            s = EXPR() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_ADDITION:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN:
            s = EXPR() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = EXPR() && true;
            break;
        case TOKEN_REAL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = EXPR() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_DIVISION:
            s = EXPR() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = EXPR() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = EXPR() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = EXPR() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = EXPR() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = EXPR() && true;
            break;
        case TOKEN_IDENTIFIER:
            s = EXPR() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = EXPR() && true;
            break;
        default:
            printf("Syntax error [POS_CALL_PARAM]: expected ['TOKEN_LOGICAL_OR', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_GREATER_THAN', 'TOKEN_FALSE_LITERAL', 'TOKEN_ADDITION', 'TOKEN_LESS_THAN', 'TOKEN_EQUAL_TO', 'TOKEN_STRING_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_REAL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_LEFT_BRACE', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_DIVISION', 'TOKEN_SUBTRACTION', 'TOKEN_NIL_LITERAL', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_LOGICAL_AND', 'TOKEN_IDENTIFIER', 'TOKEN_MULTIPLICATION'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool NEXT_CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA) && CALL_PARAM() && NEXT_CALL_PARAM() && true;
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            printf("Syntax error [NEXT_CALL_PARAM]: expected ['TOKEN_COMMA', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool ID_CALL_OR_ASSIGN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER) && NEXT_ID_CALL_OR_ASSIGN() && true;
            break;
        default:
            printf("Syntax error [ID_CALL_OR_ASSIGN]: expected ['TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool NEXT_ID_CALL_OR_ASSIGN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LEFT_BRACKET:
            s = match(TOKEN_LEFT_BRACKET) && CALL_PARAM_LIST() && match(TOKEN_RIGHT_BRACKET) && true;
            break;
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT) && EXPR() && true;
            break;
        default:
            printf("Syntax error [NEXT_ID_CALL_OR_ASSIGN]: expected ['TOKEN_LEFT_BRACKET', 'TOKEN_ASSIGNMENT'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_STRING_TYPE:
            s = match(TOKEN_STRING_TYPE) && true;
            break;
        case TOKEN_INT_TYPE:
            s = match(TOKEN_INT_TYPE) && true;
            break;
        case TOKEN_BOOL_TYPE:
            s = match(TOKEN_BOOL_TYPE) && true;
            break;
        case TOKEN_DOUBLE_TYPE:
            s = match(TOKEN_DOUBLE_TYPE) && true;
            break;
        default:
            printf("Syntax error [TYPE]: expected ['TOKEN_STRING_TYPE', 'TOKEN_INT_TYPE', 'TOKEN_BOOL_TYPE', 'TOKEN_DOUBLE_TYPE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

bool EXPR() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = call_expr_parser() && true;
            break;
        case TOKEN_REAL_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_STRING_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_NIL_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_TRUE_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_FALSE_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_INTEGER_LITERAL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_ADDITION:
            s = call_expr_parser() && true;
            break;
        case TOKEN_SUBTRACTION:
            s = call_expr_parser() && true;
            break;
        case TOKEN_MULTIPLICATION:
            s = call_expr_parser() && true;
            break;
        case TOKEN_DIVISION:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LESS_THAN:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            s = call_expr_parser() && true;
            break;
        case TOKEN_GREATER_THAN:
            s = call_expr_parser() && true;
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            s = call_expr_parser() && true;
            break;
        case TOKEN_EQUAL_TO:
            s = call_expr_parser() && true;
            break;
        case TOKEN_NOT_EQUAL_TO:
            s = call_expr_parser() && true;
            break;
        case TOKEN_IS_NIL:
            s = call_expr_parser() && true;
            break;
        case TOKEN_UNWRAP_NILABLE:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LOGICAL_AND:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LOGICAL_OR:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LOGICAL_NOT:
            s = call_expr_parser() && true;
            break;
        case TOKEN_LEFT_BRACE:
            s = call_expr_parser() && true;
            break;
        case TOKEN_RIGHT_BRACE:
            s = call_expr_parser() && true;
            break;
        default:
            printf("Syntax error [EXPR]: expected ['TOKEN_IDENTIFIER', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_NIL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_FALSE_LITERAL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_MULTIPLICATION', 'TOKEN_DIVISION', 'TOKEN_LESS_THAN', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_EQUAL_TO', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_UNWRAP_NILABLE', 'TOKEN_LOGICAL_AND', 'TOKEN_LOGICAL_OR', 'TOKEN_LOGICAL_NOT', 'TOKEN_LEFT_BRACE', 'TOKEN_RIGHT_BRACE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}

