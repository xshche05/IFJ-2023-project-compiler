#include <string.h>
#include "utils.h"
#include "codegen/codegen.h"
#include "expr_parser.h"
#include "parser.h"
#include "scanner/token.h"

bool inside_func = false;
bool not_name = true;
bool has_return = false;
int inside_loop = 0;
int inside_branch = 0;
int scope = 0;
token_t *lookahead = NULL;
funcData_t *currentFunc = NULL;

#define RETURN_TYPE_ERROR 4
#define VOID_RETURN_ERROR 6
#define NO_TYPE_ERROR 8
#define VAR_REDEFINITION_ERROR 3


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
        if (scope != 0 || inside_loop || inside_branch) { // TODO FIX
            sprintf(error_msg, "Syntax error: function declaration outside of global scope\n");
            return false;
        }
    }
    if (lookahead->type == type) {
        nl_flag = lookahead->has_newline_after;
        lookahead = TokenArray.next();
        return true;
    }
    sprintf(error_msg, "Syntax error: expected %s, got %s\n", tokens_as_str[type], tokens_as_str[lookahead->type]);
    return false;
}

bool call_expr_parser(type_t *type, bool *is_literal) {
    if (parse_expr(type, is_literal) == SUCCESS) {
        lookahead = TokenArray.prev();
        nl_flag = lookahead->has_newline_after;
        lookahead = TokenArray.next();
        return true;
    }
    else {
        return false;
    }
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
    symtable_init();
    init_codegen();
//    gen_header();
//    gen_register_def();
//    gen_std_functions();
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
    else return SYNTAX_ERROR;
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
            s = RETURN(&currentFunc);
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


bool RETURN(funcData_t **funcData) {
    bool s;
    switch (lookahead->type) {
        case TOKEN_RETURN:
            s = match(TOKEN_RETURN);
            has_return = has_return || true;
            s = s && RET_EXPR(funcData);
            break;
        default:
            sprintf(error_msg, "Syntax error [RETURN]: expected ['TOKEN_RETURN'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool RET_EXPR(funcData_t **funcData) {
    bool s;
    type_t type;
    bool is_literal;
    switch (lookahead->type) {
        case TOKEN_RIGHT_BRACE:
        case TOKEN_EOF:
            s = true;
            if ((*funcData)->returnType != void_type) {
                fprintf(stderr, "Error: void return in non void func\n");
                exit(VOID_RETURN_ERROR);
            }
            gen_return(true);
            break;
        default:
            if (call_expr_parser(&type, &is_literal)) {
                if (type != (*funcData)->returnType) {
                    if (type == int_type && (*funcData)->returnType == double_type) {
                        if (!is_literal) {
                            fprintf(stderr, "Error: cant convert non literal value from INT to DOUBLE\n");
                            exit(RETURN_TYPE_ERROR);
                        }
                        printf("INT2FLOATS\n");
                    }
                    else if (type == double_type && (*funcData)->returnType == int_type) {
                        // TODO convert double to int
                        if (!is_literal) {
                            fprintf(stderr, "Error: cant convert non literal value from DOUBLE to INT\n");
                            exit(RETURN_TYPE_ERROR);
                        }
                        printf("FLOAT2INTS\n");
                    }
                    // TODO error return type mismatch
                    else {
                        fprintf(stderr, "Error: return type mismatch\n");
                        exit(RETURN_TYPE_ERROR);
                    }
                }
                gen_return(false);
                return true;
            }
            sprintf(error_msg, "Syntax error [RET_EXPR]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    has_return = has_return || true;
    return s;
}


bool VAR_DECL() {
    bool s;
    varData_t *varData = malloc(sizeof(varData_t));
    type_t type = none_type;
    varData->isDefined = false;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = match(TOKEN_VAR);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            varData->name = id->attribute.identifier;
            varData->isDeclared = true;
            // TODO declare variable in LF if scope > 0 else in GF
            s = s && VAR_LET_TYPE(&type);
            if (s && type != none_type) {
                varData->type = type;
                if (type > 3) {
                    varData->isDefined = true;
                    // TODO declare variable as nil
                }
            } else {
                varData->type = none_type;
            }
            bool is_literal;
            s = s && VAR_LET_EXP(id, &type, &is_literal);
            if (s && type != none_type) {
                varData->isDefined = true;
                if (varData->type == none_type) {
                    if (type == nil_type) {
                        fprintf(stderr, "Error: nil variable without type\n");
                        exit(NO_TYPE_ERROR);
                    }
                    varData->type = type;
                }
                else if (varData->type != type && varData->type - type != nil_int_type-int_type && !(varData->type > 3 && type == nil_type)) {
                    // TODO int to double, double to int if literal

                    if (varData->type == int_type || varData->type == nil_int_type) {
                        if (type == double_type) {
                            if (!is_literal) {
                                fprintf(stderr, "Error: cant convert non literal value from INT to DOUBLE\n");
                                exit(99);
                            }
                            printf("INT2FLOATS\n");
                        }
                    } else if (varData->type == double_type || varData->type == nil_double_type) {
                        if (type == int_type) {
                            if (!is_literal) {
                                fprintf(stderr, "Error: cant convert non literal value from DOUBLE to INT\n");
                                exit(99);
                            }
                            printf("FLOAT2INTS\n");
                        }
                    } else {
                        fprintf(stderr, "Error: variable type mismatch\n");
                        exit(99);
                    }
                }
            }
            if (s) {
                if (varData->type == none_type) {
                    fprintf(stderr, "Error: variable without type\n");
                    exit(NO_TYPE_ERROR);
                } else if (type == void_type) {
                    fprintf(stderr, "Error: assign to void\n");
                    exit(NO_TYPE_ERROR);
                }
                if (!add_var(varData)) {
                    fprintf(stderr, "Error: variable already defined\n");
                    exit(VAR_REDEFINITION_ERROR);
                }
            }
            break;
        default:
            sprintf(error_msg, "Syntax error [VAR_DECL]: expected ['TOKEN_VAR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool LET_DECL() {
    bool s;
    type_t type = none_type;
    letData_t *letData = malloc(sizeof(letData_t));
    letData->isDefined = false;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            letData->name = id->attribute.identifier;
            letData->isDeclared = true;
            // TODO declare variable in LF if scope > 0 else in GF
            s = s && VAR_LET_TYPE(&type);
            if (s && type != none_type) {
                letData->type = type;
                if (type > 3) {
                    letData->isDefined = true;
                    // TODO declare variable as nil
                }
            } else {
                letData->type = none_type;
            }
            bool is_literal;
            s = s && VAR_LET_EXP(id, &type, &is_literal);
            if (s && type != none_type) {
                letData->isDefined = true;
                if (letData->type == none_type) {
                    if (type == nil_type) {
                        fprintf(stderr, "Error: nil constant without type\n");
                        exit(NO_TYPE_ERROR);
                    } else if (type == void_type) {
                        fprintf(stderr, "Error: assign to void\n");
                        exit(NO_TYPE_ERROR);
                    }
                    letData->type = type;
                }
                else if (letData->type != type && letData->type - type != nil_int_type-int_type && !(letData->type > 3 && type == nil_type)) {
                    if (letData->type == int_type || letData->type == nil_int_type) {
                        if (type == double_type) {
                            if (!is_literal) {
                                fprintf(stderr, "Error: cant convert non literal value from INT to DOUBLE\n");
                                exit(99);
                            }
                            printf("INT2FLOATS\n");
                        }
                    } else if (letData->type == double_type || letData->type == nil_double_type) {
                        if (type == int_type) {
                            if (!is_literal) {
                                fprintf(stderr, "Error: cant convert non literal value from DOUBLE to INT\n");
                                exit(99);
                            }
                            printf("FLOAT2INTS\n");
                        }
                    } else {
                        fprintf(stderr, "Error: variable type mismatch\n");
                        exit(99);
                    }
                }
            }

            if (s) {
                if (letData->type == none_type) {
                    fprintf(stderr, "Error: constant without type\n");
                    exit(NO_TYPE_ERROR);
                }

                if (!add_let(letData)) {
                    fprintf(stderr, "Error: constant already defined\n");
                    exit(VAR_REDEFINITION_ERROR);
                }
            }

            break;
        default:
            sprintf(error_msg, "Syntax error [LET_DECL]: expected ['TOKEN_LET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool VAR_LET_TYPE(type_t *type) {
    bool s;
    *type = none_type;
    switch (lookahead->type) {
        case TOKEN_COLON:
            s = match(TOKEN_COLON);
            s = s && TYPE(type);
            break;
        case TOKEN_ASSIGNMENT:
            s = true;
            break;
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [VAR_LET_TYPE]: expected ['TOKEN_COLON', 'TOKEN_ASSIGNMENT', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool VAR_LET_EXP(token_t *id, type_t *type, bool *is_literal) {
    bool s;
    *type = none_type;
    switch (lookahead->type) {
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT);
            s = s && call_expr_parser(type, is_literal);
            if (s) {
                printf("ASSIGN %s\n", id->attribute.identifier->str); // TODO assign
            }
            break;
        default:
            if (nl_flag) return true;
            sprintf(error_msg, "Syntax error [VAR_LET_EXP]: expected ['TOKEN_ASSIGNMENT', 'NL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FUNC_DECL() {
    bool s;
    funcData_t *funcData = malloc(sizeof(funcData_t));
    currentFunc = funcData;
    funcData->params = String.ctor();
    switch (lookahead->type) {
        case TOKEN_FUNC:
            s = match(TOKEN_FUNC);
            inside_func = true;
            has_return = false;
            token_t *id = lookahead;
            funcData->name = id->attribute.identifier;
            s = s && match(TOKEN_IDENTIFIER);
            if (s) {
                gen_func_label(id->attribute.identifier->str);
            }
            s = s && match(TOKEN_LEFT_BRACKET);
            new_frame();
            gen_new_frame();
            s = s && PARAM_LIST(&funcData);
            if (s) {
                if (!add_func(funcData)) { // TODO overloading
                    fprintf(stderr, "Error: function already defined\n");
                    exit(VAR_REDEFINITION_ERROR);
                }
            }
            gen_pop_params(funcData->params);
            gen_push_frame();
            s = s && match(TOKEN_RIGHT_BRACKET);
            s = s && FUNC_RET_TYPE(&funcData->returnType);
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            if (s) {
                if (funcData->returnType == void_type) {
                    gen_return(true);
                } else {
                    if (!has_return) {
                        fprintf(stderr, "Error: missing return statement in non void function\n");
                        exit(RETURN_TYPE_ERROR);
                    }
                }
            }
            s = s && match(TOKEN_RIGHT_BRACE);
            inside_func = false;
            del_frame();
            break;
        default:
            sprintf(error_msg, "Syntax error [FUNC_DECL]: expected ['TOKEN_FUNC'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FUNC_RET_TYPE(type_t *type) {
    bool s;
    *type = void_type;
    switch (lookahead->type) {
        case TOKEN_ARROW:
            s = match(TOKEN_ARROW);
            s = s && TYPE(type);
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


bool PARAM_LIST(funcData_t **funcData) {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            s = PARAM(funcData);
            s = s && NEXT_PARAM(funcData);
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


bool PARAM(funcData_t **funcData) {
    bool s;
    varData_t *func_param = malloc(sizeof(varData_t));
    func_param->isDeclared = true;
    func_param->isDefined = true;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            s = PARAM_NAME(funcData);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            func_param->name = id->attribute.identifier;
            if (s) {
                string_t *param = id->attribute.identifier;
                String.add_string((*funcData)->params, param);
            }
            s = s && match(TOKEN_COLON);
            type_t type;
            s = s && TYPE(&type);
            if (s) {
                func_param->type = type;
            }
            if (s) {
                String.add_char((*funcData)->params, ':');
                String.add_char((*funcData)->params, type + '0');
            }
            if (s) {
                bool flag = add_var(func_param);
                if (!flag) {
                    fprintf(stderr, "Error: parameter already defined\n");
                    exit(99); // TODO error
                }
            }
            break;
        default:
            sprintf(error_msg, "Syntax error [PARAM]: expected ['TOKEN_UNDERSCORE', 'TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool PARAM_NAME(funcData_t **funcData) {
    bool s;
    token_t *id;
    string_t *params = (*funcData)->params;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            id = lookahead;
            s = match(TOKEN_IDENTIFIER);
            if (s) {
                string_t *param = id->attribute.identifier;
                String.add_string(params, param);
                String.add_char(params, ':');
            }
            break;
        case TOKEN_UNDERSCORE:
            s = match(TOKEN_UNDERSCORE);
            if (s) {
                String.add_char(params, '_');
                String.add_char(params, ':');
            }
            break;
        default:
            sprintf(error_msg, "Syntax error [PARAM_NAME]: expected ['TOKEN_IDENTIFIER', 'TOKEN_UNDERSCORE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_PARAM(funcData_t **funcData) {
    bool s;
    string_t *params = (*funcData)->params;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA);
            String.add_char(params, '#');
            s = s && PARAM(funcData);
            s = s && NEXT_PARAM(funcData);
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
            gen_branch_labels(true);
            s = match(TOKEN_IF);
            gen_new_frame();
            new_frame();
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            push_frame();
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_pop_frame();
            del_frame();
            gen_branch_if_end();
            s = s && ELSE();
            gen_branch_end();
            break;
        default:
            sprintf(error_msg, "Syntax error [BRANCH]: expected ['TOKEN_IF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool BR_EXPR() {
    bool s;
    type_t type;
    bool is_literal;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            if (s) {
                if (id->attribute.nillable) {
                    varData_t *varData = get_var(id->attribute.identifier, NULL);
                    if (varData == NULL) {
                        varData = get_let(id->attribute.identifier, NULL);
                    }
                    letData_t *letData = malloc(sizeof(letData_t));
                    letData->name = id->attribute.identifier;
                    letData->isDeclared = true;
                    letData->isDefined = true;
                    letData->type = varData->type - (nil_int_type - int_type);
                    if (!add_let(letData)) {
                        fprintf(stderr, "Error: constant already defined\n");
                        exit(VAR_REDEFINITION_ERROR);
                    }
                } else {
                    fprintf(stderr, "Error: variable is not nillable\n");
                }
                gen_branch_if_start(true);
            }
            break;
        default:
            if (call_expr_parser(&type, &is_literal))
            {
                // TODO check if type is bool
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
            gen_branch_labels(false);
            s = match(TOKEN_IF);
            gen_new_frame();
            new_frame();
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_pop_frame();
            del_frame();
            gen_branch_if_end();
            s = s && ELSE();
            break;
        case TOKEN_LEFT_BRACE:
            s = match(TOKEN_LEFT_BRACE);
            gen_new_frame();
            new_frame();
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_pop_frame();
            del_frame();
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
    type_t type;
    bool is_literal;
    switch (lookahead->type) {
        case TOKEN_WHILE:
            inside_loop++;
            s = match(TOKEN_WHILE);
            // TODO create new frame, migrate all used variables
            // TODO inc scope
            // TODO push symbol table
            gen_while_start();
            s = s && call_expr_parser(&type, &is_literal);
            // TODO check if type is bool
            gen_while_cond();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            gen_while_end();
            // TODO destroy frame
            // TODO dec scope
            // TODO pop symbol table
            inside_loop--;
            break;
        default:
            sprintf(error_msg, "Syntax error [WHILE_LOOP]: expected ['TOKEN_WHILE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FOR_LOOP() {
    bool s;
    type_t type;
    bool is_literal;
    token_t for_id;
    switch (lookahead->type) {
        case TOKEN_FOR:
            inside_loop++;
            gen_for_range_save();
            s = match(TOKEN_FOR);
            s = s && FOR_ID(&for_id);
            s = s && match(TOKEN_IN);
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                if (type != int_type) {
                    fprintf(stderr, "Error: range type must be INT\n");
                    exit(99); // TODO error
                }
            }
            s = s && RANGE();
            gen_for_start();
            gen_for_cond();
            // TODO create new frame, migrate all used variables
            // TODO declare for id in TF
            // TODO copy FOR counter to id if nor NULL
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            // TODO destroy frame
            // TODO dec scope
            gen_for_end();
            gen_for_range_restore();
            inside_loop--;
            break;
        default:
            sprintf(error_msg, "Syntax error [FOR_LOOP]: expected ['TOKEN_FOR'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool FOR_ID(token_t *for_id) {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            for_id = lookahead;
            s = match(TOKEN_IDENTIFIER);
            // TODO add to symbol table
            break;
        case TOKEN_UNDERSCORE:
            for_id = NULL;
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
    type_t type;
    bool is_literal;
    switch (lookahead->type) {
        case TOKEN_CLOSED_RANGE:
            s = match(TOKEN_CLOSED_RANGE);
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                if (type != int_type) {
                    fprintf(stderr, "Error: range type must be INT\n");
                    exit(99); // TODO error
                }
            }
            gen_for_range(false);
            break;
        case TOKEN_HALF_OPEN_RANGE:
            s = match(TOKEN_HALF_OPEN_RANGE);
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                if (type != int_type) {
                    fprintf(stderr, "Error: range type must be INT\n");
                    exit(99); // TODO error
                }
            }
            gen_for_range(true);
            break;
        default:
            sprintf(error_msg, "Syntax error [RANGE]: expected ['TOKEN_CLOSED_RANGE', 'TOKEN_HALF_OPEN_RANGE'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool CALL_PARAM_LIST(string_t *call_params, bool call_after_param, char *func_name) {
    bool s;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_FALSE_LITERAL:
        case TOKEN_TRUE_LITERAL:
        case TOKEN_NIL_LITERAL:
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_REAL_LITERAL:
        case TOKEN_STRING_LITERAL:
        case TOKEN_LEFT_BRACKET:
        case TOKEN_LOGICAL_NOT:
            s = CALL_PARAM(call_params, call_after_param, func_name);
            s = s && NEXT_CALL_PARAM(call_params, call_after_param, func_name);
            break;
        case TOKEN_RIGHT_BRACKET:
            s = true;
            break;
        default:
//            if (call_expr_parser()) return true; TODO check if really needed
            sprintf(error_msg, "Syntax error [CALL_PARAM_LIST]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_COLON', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_INTEGER_LITERAL', 'TOKEN_RIGHT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    DEBUG_PRINT("CALL_PARAMS: %s\n", call_params->str);
    return s;
}


bool CALL_PARAM(string_t *call_params, bool call_after_param, char *func_name) {
    bool s;
    type_t type;
    bool is_literal;
    token_t *id;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            id = lookahead;
            s = match(TOKEN_IDENTIFIER);
            if (!match(TOKEN_COLON)) {
                lookahead = TokenArray.prev();
                String.add_char(call_params, '_');
            } else {
                String.add_string(call_params, id->attribute.identifier);
            }
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                String.add_char(call_params, ':');
                String.add_char(call_params, type + '0');
            }
            if (s) {
                if (call_after_param) {
                    printf("CALL %s\n", func_name);
                }
            }
            break;
        default:
            if (call_expr_parser(&type, &is_literal)) {
                String.add_char(call_params, '_');
                String.add_char(call_params, ':');
                String.add_char(call_params, type + '0');
                if (s) {
                    if (call_after_param) {
                        printf("CALL %s\n", func_name);
                    }
                }
                return true;
            }
            sprintf(error_msg, "Syntax error [CALL_PARAM_VALUE]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_CALL_PARAM(string_t *call_params, bool call_after_param, char *func_name) {
    bool s;
    switch (lookahead->type) {
        case TOKEN_COMMA:
            s = match(TOKEN_COMMA);
            if (s) String.add_char(call_params, '#');
            s = s && CALL_PARAM(call_params, call_after_param, func_name);
            s = s && NEXT_CALL_PARAM(call_params, call_after_param, func_name);
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
    token_t *id;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            id = lookahead;
            s = match(TOKEN_IDENTIFIER);
            s = s && NEXT_ID_CALL_OR_ASSIGN(id);
            break;
        default:
            sprintf(error_msg, "Syntax error [ID_CALL_OR_ASSIGN]: expected ['TOKEN_IDENTIFIER'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool NEXT_ID_CALL_OR_ASSIGN(token_t *id) {
    bool s;
    type_t type;
    bool is_literal;
    string_t *params = String.ctor();
    switch (lookahead->type) {
        case TOKEN_LEFT_BRACKET:
            s = match(TOKEN_LEFT_BRACKET);
            ignore_right_bracket = true;
            funcData_t *funcData = get_func(id->attribute.identifier);
            bool call_after_param = strcmp(funcData->params->str, "*") == 0;
            s = s && CALL_PARAM_LIST(params, call_after_param, funcData->name->str);
            s = s && match(TOKEN_RIGHT_BRACKET);
            // TODO check if params match
            if (s) {
                if (check_func_signature(params, funcData)) {
                    if (!call_after_param) {
                        printf("CALL %s\n", funcData->name->str);
                    }
                } else {
                    fprintf(stderr, "Error: function call signature mismatch\n");
                    exit(4); // TODO error
                }
            }
            ignore_right_bracket = false;
            break;
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT);
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                int cscope;
                varData_t *varData = get_var(id->attribute.identifier, &cscope);
                letData_t *letData = get_let(id->attribute.identifier, &cscope);
                if (varData == NULL && letData != NULL) {
                    fprintf(stderr, "Error: const redefinition\n");
                    exit(9); // TODO error
                } else if (cscope == -1) {
                    fprintf(stderr, "Error: variable not defined\n");
                } else {
                    if (varData->type != type && varData->type - type != nil_int_type-int_type && !(varData->type > 3 && type == nil_type)) {
                        if (varData->type == int_type || varData->type == nil_int_type) {
                            if (is_literal && type == double_type) {
                                printf("INT2FLOATS\n");
                            } else {
                                fprintf(stderr, "Error: variable type mismatch\n");
                                exit(99); // TODO error
                            }
                        } else if (varData->type == double_type || varData->type == nil_double_type) {
                            if (is_literal && type == int_type) {
                                printf("FLOAT2INTS\n");
                            } else {
                                fprintf(stderr, "Error: variable type mismatch\n");
                                exit(99); // TODO error
                            }
                        }
                        else {
                            fprintf(stderr, "Error: variable type mismatch\n");
                            exit(99); // TODO error
                        }
                    }
                    varData->isDefined = true;
                    // TODO assign
                    printf("ASSIGN %s\n", id->attribute.identifier->str);
                }
            }
            break;
        default:
            sprintf(error_msg, "Syntax error [NEXT_ID_CALL_OR_ASSIGN]: expected ['TOKEN_LEFT_BRACKET', 'TOKEN_ASSIGNMENT'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}


bool TYPE(type_t *type) {
    bool s;
    bool nillable = false;
    *type = none_type;
    switch (lookahead->type) {
        case TOKEN_STRING_TYPE:
            nillable = lookahead->attribute.nillable;
            s = match(TOKEN_STRING_TYPE);
            if (s) {
                if (nillable) *type = nil_string_type;
                else *type = string_type;
            }
            break;
        case TOKEN_INT_TYPE:
            nillable = lookahead->attribute.nillable;
            s = match(TOKEN_INT_TYPE);
            if (s) {
                if (nillable) *type = nil_int_type;
                else *type = int_type;
            }
            break;
        case TOKEN_BOOL_TYPE:
            nillable = lookahead->attribute.nillable;
            s = match(TOKEN_BOOL_TYPE);
            if (s) {
                if (nillable) *type = nil_bool_type;
                else *type = bool_type;
            }
            break;
        case TOKEN_DOUBLE_TYPE:
            nillable = lookahead->attribute.nillable;
            s = match(TOKEN_DOUBLE_TYPE);
            if (s) {
                if (nillable) *type = nil_double_type;
                else *type = double_type;
            }
            break;
        default:
            sprintf(error_msg, "Syntax error [EXPR]: expected ['TOKEN_IDENTIFIER', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_NIL_LITERAL', 'TOKEN_TRUE_LITERAL', 'TOKEN_FALSE_LITERAL', 'TOKEN_INTEGER_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_MULTIPLICATION', 'TOKEN_DIVISION', 'TOKEN_LESS_THAN', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_EQUAL_TO', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_IS_NIL', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_LOGICAL_AND', 'TOKEN_LOGICAL_OR', 'TOKEN_LOGICAL_NOT', 'TOKEN_LEFT_BRACKET'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    return s;
}