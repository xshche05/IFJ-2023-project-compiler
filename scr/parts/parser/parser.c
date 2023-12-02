#include <string.h>
#include "../codegen/codegen.h"
#include "expr_parser.h"
#include "parser.h"
#include "token.h"
#include "../macros.h"
#include <limits.h>
#include <stdio.h>
#include "memory.h"

bool inside_func = false;
bool has_return = false;
int inside_loop = 0;
int inside_branch = 0;
token_t *lookahead = NULL;
funcData_t *currentFunc = NULL;
bool collect_funcs = false;


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
        if (get_scope() != 0 || inside_loop || inside_branch) {
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

int S() { //start
    bool s;
    collect_funcs = !collect_funcs;
    init_codegen();
    symtable_init();
    gen_header();
    gen_std_functions();
    gen_new_frame();
    gen_push_frame();
    // Jump to GF and LF defvar
    char *main_defvar_label = gen_unique_label("main_defvar");
    char *main_defvar_label_back = gen_unique_label("main_defvar_back");
    gen_line("JUMP %s\n", main_defvar_label);
    gen_line("LABEL %s\n", main_defvar_label_back);
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
    gen_pop_frame();
    // gen skip label
    char *skip_label = gen_unique_label("main_def_skip");
    gen_line("JUMP %s\n", skip_label);
    gen_line("LABEL %s\n", main_defvar_label);
    gen_used_vars();
    gen_used_global_vars();
    gen_line("JUMP %s\n", main_defvar_label_back);
    gen_line("LABEL %s\n", skip_label);
    safe_free(main_defvar_label);
    safe_free(main_defvar_label_back);
    safe_free(skip_label);
    symtable_destroy();
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

void check_return_type(type_t type, bool is_literal, funcData_t **funcData) {
    if (collect_funcs) return;
    if (type != (*funcData)->returnType && (*funcData)->returnType - type != nil_int_type-int_type) {
        if (type == int_type && (*funcData)->returnType == double_type) {
            if (!is_literal) {
                fprintf(stderr, "Error: cant convert non literal value from INT to DOUBLE\n");
                safe_exit(SEMANTIC_ERROR_4);
            }
            gen_line("INT2FLOATS\n");
        }
        else {
            fprintf(stderr, "Error: return type mismatch\n");
            safe_exit(SEMANTIC_ERROR_4);
        }
    }
    if (type == void_type && (*funcData)->returnType == void_type) {
        fprintf(stderr, "Error: smth after return\n");
        safe_exit(9);
    }
    gen_return(false);
}

bool RET_EXPR(funcData_t **funcData) {
    bool s;
    type_t type;
    bool is_literal;
    switch (lookahead->type) {
        case TOKEN_RIGHT_BRACE:
        case TOKEN_EOF:
            s = true;
            if (collect_funcs) break;
            if ((*funcData)->returnType != void_type) {
                fprintf(stderr, "Error: void return in non void func\n");
                safe_exit(SEMANTIC_ERROR_6);
            }
            gen_return(true);
            break;
        default:
            if (call_expr_parser(&type, &is_literal)) {
                if (collect_funcs) return true;
                check_return_type(type, is_literal, funcData);
                return true;
            }
            sprintf(error_msg, "Syntax error [RET_EXPR]: expected ['TOKEN_FALSE_LITERAL', 'TOKEN_LESS_THAN', 'TOKEN_LOGICAL_AND', 'TOKEN_LEFT_BRACKET', 'TOKEN_NIL_LITERAL', 'TOKEN_REAL_LITERAL', 'TOKEN_STRING_LITERAL', 'TOKEN_ADDITION', 'TOKEN_SUBTRACTION', 'TOKEN_LOGICAL_OR', 'TOKEN_IDENTIFIER', 'TOKEN_EQUAL_TO', 'TOKEN_LESS_THAN_OR_EQUAL_TO', 'TOKEN_GREATER_THAN', 'TOKEN_NOT_EQUAL_TO', 'TOKEN_GREATER_THAN_OR_EQUAL_TO', 'TOKEN_TRUE_LITERAL', 'TOKEN_IS_NIL', 'TOKEN_DIVISION', 'TOKEN_MULTIPLICATION', 'TOKEN_LOGICAL_NOT', 'TOKEN_UNWRAP_NILLABLE', 'TOKEN_INTEGER_LITERAL', 'TOKEN_RIGHT_BRACE', 'TOKEN_EOF'], got %s\n", tokens_as_str[lookahead->type]);
            s = false;
    }
    has_return = has_return || true;
    return s;
}

void check_decl_type(type_t type, varData_t *varData) {
    if (collect_funcs) return;
    if (type != none_type) {
        varData->type = type;
        if (type > 3) {
            varData->isDeclared = true;
            varData->minInitScope = min(get_scope(), varData->minInitScope);
            gen_line("MOVE %s nil@nil\n", gen_var_name(varData->name->str, get_scope()));
        }
    } else {
        varData->type = none_type;
    }
}

void check_decl_expr(type_t type, bool is_literal, varData_t *varData) {
    if (collect_funcs) return;
    if (type != none_type) {
        varData->isDeclared = true;
        varData->minInitScope = min(get_scope(), varData->minInitScope);
        if (varData->type == none_type) {
            if (type == nil_type) {
                fprintf(stderr, "Error: nil variable without type\n");
                safe_exit(SEMANTIC_ERROR_8);
            }
            varData->type = type;
        }
        else if (varData->type != type && varData->type - type != nil_int_type-int_type && !(varData->type > 3 && type == nil_type)) {
            if (varData->type == double_type || varData->type == nil_double_type) {
                if (type == int_type) {
                    if (!is_literal) {
                        fprintf(stderr, "Error: cant convert non literal value from DOUBLE to INT\n");
                        safe_exit(SEMANTIC_ERROR_7);
                    }
                    gen_line("INT2FLOATS\n");
                } else {
                    fprintf(stderr, "Error: variable type mismatch\n");
                    safe_exit(SEMANTIC_ERROR_7);
                }
            } else {
                fprintf(stderr, "Error: variable type mismatch\n");
                safe_exit(SEMANTIC_ERROR_7);
            }
        }
        int scope = get_scope();
        gen_line("POPS %s\n", gen_var_name(varData->name->str, scope));
    }
}

void check_if_none(type_t type, varData_t *varData) {
    if (collect_funcs) return;
    if (varData->type == none_type) {
        fprintf(stderr, "Error: variable without type\n");
        safe_exit(SEMANTIC_ERROR_8);
    } else if (type == void_type) {
        fprintf(stderr, "Error: assign to void\n");
        safe_exit(SEMANTIC_ERROR_8);
    }
}

bool VAR_DECL() {
    bool s;
    varData_t *varData = safe_malloc(sizeof(varData_t));
    type_t type = none_type;
    varData->isDeclared = false;
    varData->minInitScope = INT_MAX;
    varData->canBeRedefined = false;
    switch (lookahead->type) {
        case TOKEN_VAR:
            s = match(TOKEN_VAR);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            varData->name = id->attribute.identifier;
            varData->isDefined = true;
            s = s && VAR_LET_TYPE(&type);
            type_t tmp_type = type;
            if (s) {
                check_decl_type(type, varData);
            }
            bool is_literal;
            s = s && VAR_LET_EXP(id, &type, &is_literal);
            if (tmp_type == none_type && type == none_type && !collect_funcs) {
                fprintf(stderr, "Error: variable without type and expression\n");
                safe_exit(SYNTAX_ERROR);
            }
            if (s) {
                check_decl_expr(type, is_literal, varData);
                check_if_none(type, varData);
                if (collect_funcs) return s;
                if (!add_var(varData)) {
                    fprintf(stderr, "Error: variable already defined\n");
                    safe_exit(SEMANTIC_ERROR_3);
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
    letData_t *letData = safe_malloc(sizeof(letData_t));
    letData->isDeclared = false;
    letData->minInitScope = INT_MAX;
    letData->canBeRedefined = false;
    switch (lookahead->type) {
        case TOKEN_LET:
            s = match(TOKEN_LET);
            token_t *id = lookahead;
            s = s && match(TOKEN_IDENTIFIER);
            letData->name = id->attribute.identifier;
            letData->isDefined = true;
            s = s && VAR_LET_TYPE(&type);
            type_t tmp_type = type;
            if (s) {
                check_decl_type(type, letData);
            }
            bool is_literal;
            s = s && VAR_LET_EXP(id, &type, &is_literal);
            if (tmp_type == none_type && type == none_type && !collect_funcs) {
                fprintf(stderr, "Error: variable without type and expression\n");
                safe_exit(SYNTAX_ERROR);
            }
            if (s) {
                check_decl_expr(type, is_literal, letData);
                check_if_none(type, letData);
                if (collect_funcs) return s;
                if (!add_let(letData)) {
                    fprintf(stderr, "Error: constant already defined\n");
                    safe_exit(SEMANTIC_ERROR_3);
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
    funcData_t *funcData = safe_malloc(sizeof(funcData_t));
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
            char *skip_label = gen_unique_label("skip");
            gen_line("JUMP %s\n", skip_label);
            if (s) {
                gen_func_label(id->attribute.identifier->str);
            }
            // Goto defvar label
            char *defvar_label = gen_unique_label("defvar");
            char *defvar_label_back = gen_unique_label("defvar_back");
            s = s && match(TOKEN_LEFT_BRACKET);
            new_frame();
            gen_new_frame();
            gen_push_frame();
            gen_line("JUMP %s\n", defvar_label);
            gen_line("LABEL %s\n", defvar_label_back);
            s = s && PARAM_LIST(&funcData);
            if (s) {
                if (collect_funcs) {
                    if (!add_func(funcData)) { // TODO overloading
                        fprintf(stderr, "Error: function already defined\n");
                        safe_exit(SEMANTIC_ERROR_3);
                    }
                }
            }
            new_frame();
            gen_pop_params(funcData->params);
            s = s && match(TOKEN_RIGHT_BRACKET);
            s = s && FUNC_RET_TYPE(&funcData->returnType);
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            char *def_skip_label = gen_unique_label("def_skip");
            gen_line("JUMP %s\n", def_skip_label);
            gen_line("LABEL %s\n", defvar_label);
            gen_used_vars();
            gen_line("JUMP %s\n", defvar_label_back);
            gen_line("LABEL %s\n", def_skip_label);
            if (s) {
                if (funcData->returnType == void_type) {
                    gen_return(true);
                } else {
                    if (!has_return) {
                        fprintf(stderr, "Error: missing return statement in non void function\n");
                        safe_exit(SEMANTIC_ERROR_6);
                    }
                }
            }
            s = s && match(TOKEN_RIGHT_BRACE);
            inside_func = false;
            del_frame();
            del_frame();
            gen_line("LABEL %s\n", skip_label);
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
    letData_t *func_param = safe_malloc(sizeof(varData_t));
    func_param->isDefined = true;
    func_param->isDeclared = true;
    func_param->minInitScope = get_scope();
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
                bool flag = add_let(func_param);
                if (!flag) {
                    fprintf(stderr, "Error: parameter already defined\n");
                    safe_exit(SEMANTIC_ERROR_9);
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
            new_frame();
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
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
            if (s && !collect_funcs) {
                varData_t *varData = get_var(id->attribute.identifier);
                if (varData == NULL) {
                    varData = get_let(id->attribute.identifier);
                }
                if (varData->type < 4) {
                    fprintf(stderr, "Var type should be nillable\n");
                    safe_exit(SEMANTIC_ERROR_9);
                }
                if (varData->minInitScope > get_scope()) {
                    fprintf(stderr, "Error: variable not initialized in this scope\n");
                    safe_exit(SEMANTIC_ERROR_5);
                }
                gen_line("PUSHS %s\n", gen_var_name(varData->name->str, varData->scope));
                letData_t *letData = safe_malloc(sizeof(letData_t));
                letData->name = id->attribute.identifier;
                letData->isDefined = true;
                letData->isDeclared = true;
                letData->minInitScope = get_scope();
                letData->canBeRedefined = true;
                letData->type = varData->type - (nil_int_type - int_type);
                gen_line("MOVE %s %s\n", gen_var_name(letData->name->str, get_scope()), gen_var_name(varData->name->str, varData->scope));
                if (!add_let(letData)) {
                    fprintf(stderr, "Error: constant already defined\n");
                    safe_exit(SEMANTIC_ERROR_3);
                }
                gen_branch_if_start(true);
            }
            break;
        default:
            if (call_expr_parser(&type, &is_literal))
            {
                if (!collect_funcs) {
                    if (type != bool_type) {
                        fprintf(stderr, "Error: if condition expr must return BOOL\n");
                        safe_exit(SEMANTIC_ERROR_7);
                    }
                    gen_branch_if_start(false);
                }
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
            new_frame();
            s = s && BR_EXPR();
            s = s && match(TOKEN_LEFT_BRACE);
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            del_frame();
            gen_branch_if_end();
            s = s && ELSE();
            break;
        case TOKEN_LEFT_BRACE:
            s = match(TOKEN_LEFT_BRACE);
            new_frame();
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
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
            gen_while_start();
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                if (!collect_funcs) {
                    if (type != bool_type) {
                        fprintf(stderr, "Error: while condition expr must return BOOL\n");
                        safe_exit(SEMANTIC_ERROR_9);
                    }
                }
            }
            gen_while_cond();
            s = s && match(TOKEN_LEFT_BRACE);
            new_frame();
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            del_frame();
            gen_while_end();
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
                if (!collect_funcs) {
                    if (type != int_type) {
                        fprintf(stderr, "Error: range type must be INT\n");
                        safe_exit(SEMANTIC_ERROR_9);
                    }
                }
            }
            s = s && RANGE();
            gen_for_start();
            gen_for_cond();
            s = s && match(TOKEN_LEFT_BRACE);
            new_frame();
            // TODO add for_id to frame, copy counter to for_id
            s = s && CODE();
            s = s && match(TOKEN_RIGHT_BRACE);
            del_frame();
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
                if (!collect_funcs) {
                    if (type != int_type) {
                        fprintf(stderr, "Error: range type must be INT\n");
                        safe_exit(SEMANTIC_ERROR_9);
                    }
                }
            }
            gen_for_range(false);
            break;
        case TOKEN_HALF_OPEN_RANGE:
            s = match(TOKEN_HALF_OPEN_RANGE);
            s = s && call_expr_parser(&type, &is_literal);
            if (s) {
                if (!collect_funcs) {
                    if (type != int_type) {
                        fprintf(stderr, "Error: range type must be INT\n");
                        safe_exit(SEMANTIC_ERROR_9);
                    }
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
    type_t type;
    bool is_literal;
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
            if (call_expr_parser(&type, &is_literal)) return true;
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
                    gen_line("CALL %s\n", func_name);
                }
            }
            break;
        default:
            if (call_expr_parser(&type, &is_literal)) {
                String.add_char(call_params, '_');
                String.add_char(call_params, ':');
                String.add_char(call_params, type + '0');
                if (call_after_param) {
                    gen_line("CALL %s\n", func_name);
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
            funcData_t *funcData;
            bool call_after_param = false;
            if (!collect_funcs) {
                funcData = get_func(id->attribute.identifier);
                call_after_param = strcmp(funcData->params->str, "*") == 0;
                s = s && CALL_PARAM_LIST(params, call_after_param, funcData->name->str);
            } else {
                s = s && CALL_PARAM_LIST(params, call_after_param, NULL);
            }
            s = s && match(TOKEN_RIGHT_BRACKET);
            // TODO check if params match
            if (s && !collect_funcs) {
                if (!call_after_param) {
                    gen_line("CALL %s\n", funcData->name->str);
                }
                if (!collect_funcs) {
                    if (!check_func_signature(params, funcData)) {
                        fprintf(stderr, "Error: function signature mismatch\n");
                        safe_exit(SEMANTIC_ERROR_4);
                    }
                }
            }
            ignore_right_bracket = false;
            break;
        case TOKEN_ASSIGNMENT:
            s = match(TOKEN_ASSIGNMENT);
            s = s && call_expr_parser(&type, &is_literal);
            if (s && !collect_funcs) {
                varData_t *varData = get_var(id->attribute.identifier);
                letData_t *letData = get_let(id->attribute.identifier);
                if (varData == NULL && letData != NULL) {
                    varData = letData;
                    if (letData->isDeclared) {
                        fprintf(stderr, "Error: const redeclaration\n");
                        safe_exit(SEMANTIC_ERROR_9);
                    }
                }
                if (varData->type != type && varData->type - type != nil_int_type-int_type && !(varData->type > 3 && type == nil_type)) {
                    if (varData->type == double_type || varData->type == nil_double_type) {
                        if (is_literal && type == int_type) {
                            gen_line("INT2FLOATS\n");
                        } else {
                            fprintf(stderr, "Error: variable type mismatch\n");
                            safe_exit(SEMANTIC_ERROR_7);
                        }
                    }
                    else {
                        fprintf(stderr, "Error: variable type mismatch\n");
                        safe_exit(SEMANTIC_ERROR_7);
                    }
                }
                varData->isDeclared = true;
                varData->minInitScope = min(varData->minInitScope, get_scope());
                gen_line("POPS %s\n", gen_var_name(id->attribute.identifier->str, varData->scope));
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