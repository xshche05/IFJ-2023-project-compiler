#include "parts.h"
#include "utils.h"
#include "new_symtable.h"
#include "../error.h"
#include "codegen/codegen.h"

bool inside_func = false;
int inside_loop = 0;
int inside_branch = 0;
int scope = 0;
int stayed = 0;
token_t *last_token = NULL;
token_t *last_id = NULL;

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
        "TOKEN_UNWRAP_NILLABLE",
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
    if (type == TOKEN_RETURN) {
        if (!inside_func) {
            sprintf(error_msg, "Syntax error: TOKEN_RETURN outside of function\n");
            return false;
        }
    }
    if (type == TOKEN_BREAK || type == TOKEN_CONTINUE) {
        if (!inside_loop) {
            sprintf(error_msg, "Syntax error: %s outside of loop\n", tokens_as_str[type]);
            return false;
        }
    }
    if (type == TOKEN_FUNC) {
        if (scope - 1 != 0 || inside_loop || inside_branch) { //TODO FIX
            sprintf(error_msg, "Syntax error: function declaration outside of global scope\n", tokens_as_str[type]);
            return false;
        }
    }
    if (lookahead->type == type) {
        nl_flag = lookahead->has_newline_after;
        last_token = lookahead;
        if (lookahead->type == TOKEN_IDENTIFIER) {
            last_id = lookahead;
        }
        lookahead = TokenArray.next();
        return true;
    }
    sprintf(error_msg, "Syntax error: expected %s, got %s\n", tokens_as_str[type], tokens_as_str[lookahead->type]);
    return false;
}

void increment_scope() {
    scope++;
    fprintf(stderr, "scope inc: %d\n", scope);
}

void decrement_scope() {
    scope--;
    fprintf(stderr, "scope dec: %d\n", scope);
    stayed = 0;
}

void scope_new() {
    stayed++;
    fprintf(stderr, "scope stay: %d\n", scope);
}

void scope_leave() {
    fprintf(stderr, "scope leave: %d\n", scope);
}

bool call_expr_parser(token_t *token) {
    if (token->type == TOKEN_INTEGER_LITERAL) {
        printf("PUSHS int@%d\n", token->attribute.integer);
    }
    if (token->type == TOKEN_REAL_LITERAL) {
        printf("PUSHS float@%a\n", token->attribute.real);
    }
    if (token->type == TOKEN_STRING_LITERAL) {
        printf("PUSHS string@%s\n", token->attribute.string->str);
    }
    if (token->type == TOKEN_TRUE_LITERAL) {
        printf("PUSHS bool@true\n");
    }
    if (token->type == TOKEN_FALSE_LITERAL) {
        printf("PUSHS bool@false\n");
    }
    return match(token->type);
}

bool nl_check() {
    if (nl_flag) {
        return true;
    }
    sprintf(error_msg, "Syntax error: New line expected\n");
    return false;
}

int S() {
    bool s;
    init_codegen();
    gen_header();
//    gen_register_def();
    gen_std_functions();
    lookahead = TokenArray.next();
    switch (lookahead->type) {
        case TOKEN_FUNC:
        case TOKEN_LET:
        case TOKEN_CONTINUE:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_IDENTIFIER:
        case TOKEN_RETURN:
        case TOKEN_BREAK:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_EOF:
            s = CODE();
            s = s && match(TOKEN_EOF);
            break;
        default:
            sprintf(error_msg, "Syntax error [S]: expected ['TOKEN_EOF', 'TOKEN_IF', 'TOKEN_IDENTIFIER', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_VAR', 'TOKEN_LET', 'TOKEN_RETURN', 'TOKEN_FUNC', 'TOKEN_BREAK', 'TOKEN_CONTINUE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    if (s) return SUCCESS;
    else return BAD_SYNTAX_ERR;
}


bool CODE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = VAR_DECL();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_LET:
            s = LET_DECL();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_FUNC:
            s = FUNC_DECL();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_WHILE:
            s = WHILE_LOOP();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_FOR:
            s = FOR_LOOP();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_IF:
            s = BRANCH();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_IDENTIFIER:
            s = ID_CALL_OR_ASSIGN();
            s = s && nl_check();
            s = s && CODE();
            break;
        case TOKEN_RETURN:
            s = RETURN();
            break;
        case TOKEN_BREAK:
            s = match(TOKEN_BREAK);
            gen_break();
            break;
        case TOKEN_CONTINUE:
            s = match(TOKEN_CONTINUE);
            gen_continue();
            break;
        case TOKEN_RIGHT_BRACE:
        case TOKEN_EOF:
            s = true;
            break;
        default:
            sprintf(error_msg, "Syntax error [CODE]: expected ['TOKEN_VAR', 'TOKEN_LET', 'TOKEN_FUNC', 'TOKEN_WHILE', 'TOKEN_FOR', 'TOKEN_IF', 'TOKEN_IDENTIFIER', 'TOKEN_RETURN', 'TOKEN_BREAK', 'TOKEN_CONTINUE', 'TOKEN_RIGHT_BRACE', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool RETURN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_RETURN:
            s = match(TOKEN_RETURN);
            s = s && RET_EXPR();
            break;
        default:
            sprintf(error_msg, "Syntax error [RETURN]: expected ['TOKEN_RETURN'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool RET_EXPR() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_RIGHT_BRACE:
        case TOKEN_EOF:
            s = true;
            gen_return(true);
            break;
        default:
            if (call_expr_parser(lookahead)) {
                gen_return(false);
                return true;
            }
            sprintf(error_msg, "Syntax error [RET_EXPR]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool VAR_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = match(TOKEN_VAR);
            s = s && match(TOKEN_IDENTIFIER);
            // if exist in current scope - error
            gen_var_decl(last_id->attribute.identifier->str, scope, stayed);
            s = s && VAR_LET_TYPE();
            s = s && VAR_LET_EXP();
            break;
        default:
            sprintf(error_msg, "Syntax error [VAR_DECL]: expected ['TOKEN_VAR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool VAR_LET_TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COLON:
            s = match(TOKEN_COLON);
            s = s && TYPE();
            break;
        case TOKEN_ASSIGNMENT:
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [VAR_LET_TYPE]: expected ['TOKEN_COLON', 'TOKEN_ASSIGNMENT', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool VAR_LET_EXP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT);
            s = s && call_expr_parser(lookahead);
            gen_var_assign(last_id->attribute.identifier->str, scope, stayed);
            break;
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [VAR_LET_EXP]: expected ['TOKEN_ASSIGNMENT', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool LET_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET);
            s = s && match(TOKEN_IDENTIFIER);
            // if exist in current scope - error
            gen_var_decl(last_id->attribute.identifier->str, scope, stayed);
            s = s && VAR_LET_TYPE();
            s = s && VAR_LET_EXP();
            break;
        default:
            sprintf(error_msg, "Syntax error [LET_DECL]: expected ['TOKEN_LET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FUNC_DECL() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_FUNC:
            inside_func = true;
            increment_scope();
            s = match(TOKEN_FUNC);
            s = s && match(TOKEN_IDENTIFIER);
            s = s && match(TOKEN_LEFT_BRACKET);
            s = s && PARAM_LIST();
            s = s && match(TOKEN_RIGHT_BRACKET);
            s = s && FUNC_RET_TYPE();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            inside_func = false;
            decrement_scope();
            break;
        default:
            sprintf(error_msg, "Syntax error [FUNC_DECL]: expected ['TOKEN_FUNC'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FUNC_RET_TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ARROW:
            s = match(TOKEN_ARROW);
            s = s && TYPE();
            break;
        case TOKEN_LEFT_BRACE:
            s = true;
            break;
        default:
            sprintf(error_msg, "Syntax error [FUNC_RET_TYPE]: expected ['TOKEN_ARROW', 'TOKEN_LEFT_BRACE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool PARAM_LIST() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            s = PARAM();
            s = s && NEXT_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            sprintf(error_msg, "Syntax error [PARAM_LIST]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            s = PARAM_NAME();
            s = s && match(TOKEN_IDENTIFIER);
            s = s && match(TOKEN_COLON);
            s = s && TYPE();
            break;
        default:
            sprintf(error_msg, "Syntax error [PARAM]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool PARAM_NAME() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER);
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE);
            break;
        default:
            sprintf(error_msg, "Syntax error [PARAM_NAME]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA);
            s = s && PARAM();
            s = s && NEXT_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            sprintf(error_msg, "Syntax error [NEXT_PARAM]: expected ['TOKEN_COMMA', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool BRANCH() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IF:
            inside_branch++;
            increment_scope();
            gen_branch_labels(true);
            s = match(TOKEN_IF);
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_branch_if_end();
            s = s && ELSE();
            gen_branch_end();
            inside_branch--;
            decrement_scope();
            break;
        default:
            sprintf(error_msg, "Syntax error [BRANCH]: expected ['TOKEN_IF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool BR_EXPR() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET);
            s = s && match(TOKEN_IDENTIFIER);
            gen_branch_if_start(true);
            break;
        default:
            if (call_expr_parser(lookahead))
            {
                gen_branch_if_start(false);
                return true;
            }
            sprintf(error_msg, "Syntax error [BR_EXPR]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL', 'TOKEN_LET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool ELSE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_ELSE:
            s = match(TOKEN_ELSE);
            s = s && ELSE_IF();
            break;
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [ELSE]: expected ['TOKEN_ELSE', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool ELSE_IF() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IF:
            scope_new();
            gen_branch_labels(false);
            s = match(TOKEN_IF);
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_branch_if_end();
            s = s && ELSE();
            scope_leave();
            break;
        case TOKEN_LEFT_BRACE:
            scope_new();
            s = match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            scope_leave();
            break;
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [ELSE_IF]: expected ['TOKEN_IF', 'TOKEN_LEFT_BRACE', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool WHILE_LOOP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_WHILE:
            inside_loop++;
            increment_scope();
            s = match(TOKEN_WHILE);
            gen_while_start();
            s = s && call_expr_parser(lookahead);
            gen_while_cond();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_while_end();
            inside_loop--;
            decrement_scope();
            break;
        default:
            sprintf(error_msg, "Syntax error [WHILE_LOOP]: expected ['TOKEN_WHILE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FOR_LOOP() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_FOR:
            inside_loop++;
            increment_scope();
            gen_for_range_save();
            s = match(TOKEN_FOR);
            s = s && FOR_ID(); // TODO: allias for id is GF@$FOR_START
            s = s && match(TOKEN_IN);
            s = s && call_expr_parser(lookahead);
            s = s && RANGE();
            gen_for_start();
            gen_for_cond();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_for_end();
            gen_for_range_restore();
            inside_loop--;
            decrement_scope();
            break;
        default:
            sprintf(error_msg, "Syntax error [FOR_LOOP]: expected ['TOKEN_FOR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FOR_ID() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER);
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE);
            break;
        default:
            sprintf(error_msg, "Syntax error [FOR_ID]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool RANGE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_CLOSED_RANGE:
            s = match(TOKEN_CLOSED_RANGE);
            s = s && call_expr_parser(lookahead);
            gen_for_range(false);
            break;
        case TOKEN_HALF_OPEN_RANGE:
            s = match(TOKEN_HALF_OPEN_RANGE);
            s = s && call_expr_parser(lookahead);
            gen_for_range(true);
            break;
        default:
            sprintf(error_msg, "Syntax error [RANGE]: expected ['TOKEN_CLOSED_RANGE', 'TOKEN_HALF_OPEN_RANGE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool CALL_PARAM_LIST() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = CALL_PARAM();
            s = s && NEXT_CALL_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            if (call_expr_parser(lookahead)) return true;
            sprintf(error_msg, "Syntax error [CALL_PARAM_LIST]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_COLON', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_INTEGER_LITERAL', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER);
            if (!match(TOKEN_COLON)) {
                lookahead = TokenArray.prev();
            }
            s = s && call_expr_parser(lookahead);
            break;
        default:
            if (call_expr_parser(lookahead)) return true;
            sprintf(error_msg, "Syntax error [CALL_PARAM_VALUE]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_CALL_PARAM() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA);
            s = s && CALL_PARAM();
            s = s && NEXT_CALL_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
            sprintf(error_msg, "Syntax error [NEXT_CALL_PARAM]: expected ['TOKEN_COMMA', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool ID_CALL_OR_ASSIGN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            s = match(TOKEN_IDENTIFIER);
            s = s && NEXT_ID_CALL_OR_ASSIGN();
            break;
        default:
            sprintf(error_msg, "Syntax error [ID_CALL_OR_ASSIGN]: expected ['TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_ID_CALL_OR_ASSIGN() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_LEFT_BRACKET:
            s = match(TOKEN_LEFT_BRACKET);
            s = s && CALL_PARAM_LIST();
            s = s && match(TOKEN_RIGHT_BRACKET);
            break;
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT);
            s = s && call_expr_parser(lookahead);
            gen_var_assign(last_id->attribute.identifier->str, scope, stayed);
            break;
        default:
            sprintf(error_msg, "Syntax error [NEXT_ID_CALL_OR_ASSIGN]: expected ['TOKEN_LEFT_BRACKET', 'TOKEN_ASSIGNMENT'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool TYPE() {
    bool s;
    switch (lookahead->type) {
        case TOKEN_STRING_TYPE:
            s = match(TOKEN_STRING_TYPE);
            break;
        case TOKEN_INT_TYPE:
            s = match(TOKEN_INT_TYPE);
            break;
        case TOKEN_BOOL_TYPE:
            s = match(TOKEN_BOOL_TYPE);
            break;
        case TOKEN_DOUBLE_TYPE:
            s = match(TOKEN_DOUBLE_TYPE);
            break;
        default:
            sprintf(error_msg, "Syntax error [EXPR]: expected ['TOKEN_IDENTIFIER', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_NIL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_FALSE_LITERAL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_MULTIPLICATION', 'TOKEN_DIVISION', 'TOKEN_LESS_THAN', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_EQUAL_TO', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_LOGICAL_AND', 'TOKEN_LOGICAL_OR', 'TOKEN_LOGICAL_NOT', 'TOKEN_LEFT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}