#include "parts.h"
#include "utils.h"
#include "new_symtable.h"

bool match(token_type_t type) {
    if (type == TOKEN_NEWLINE) {
        return nl_flag;
    }
    if (lookahead->type == type) {
        lookahead = TokenArray.next();
        nl_flag = false;
        while (lookahead->type == TOKEN_NEWLINE) {
            nl_flag = true;
            lookahead = TokenArray.next();
        }
        return true;
    }
    return false;
}


bool parse() {
    bool success = code();
    return success;
}

bool code() {
    bool success = true;
    switch (lookahead->type) {
        case TOKEN_BREAK:
            success = success && match(TOKEN_BREAK);
            break;
        case TOKEN_CONTINUE:
            success = success && match(TOKEN_CONTINUE);
            break;
        case TOKEN_RETURN:
            success = success && return_r();
            break;
        case TOKEN_VAR:
            success = success && var_decl()
                    && code();
            break;
        case TOKEN_IDENTIFIER:
            success = success && id_call_or_assign()
                    && code();
            break;
        case TOKEN_LET:
            success = success && let_decl()
                    && code();
            break;
        case TOKEN_FUNC:
            success = success && func_decl()
                    && code();
            break;
        case TOKEN_IF:
            success = success && branch()
                    && code();
            break;
        case TOKEN_WHILE:
            success = success && while_loop()
                    && code();
            break;
        case TOKEN_FOR:
            success = success && for_loop()
                    && code();
            break;
        case TOKEN_RIGHT_BRACE:
            success = true;
        default:    
            success = false;
            break;
    }
    return success;
}


bool return_r(){
    bool success = true;
    success = success && match(TOKEN_RETURN);
    return success;
}

bool ret_expr(){
    bool success = true;
    // TODO expr
}

bool var_decl() {
    bool success = true;
    success = success && match(TOKEN_VAR)
            && match(TOKEN_IDENTIFIER)
            && var_let_type()
            && var_let_exp()
            && nl();
    return success;
}

bool var_let_type() {
    bool success = true;
    switch (lookahead->type) {
        case TOKEN_COLON:
            success = success   && match(TOKEN_COLON)
                                && type();
            break;
        case TOKEN_NEWLINE:
        default:
            success = false;
            break;
    }
    return success;
}

bool var_let_exp(){
    // TODO expr
}

bool let_decl(){
    bool success = true;
    success = success && match(TOKEN_LET)
        && match(TOKEN_IDENTIFIER)
        && var_let_type()
        && var_let_exp()
        && nl();
    return success;


}

bool func_decl(){
    bool success = true;
    success = success && match(TOKEN_FUNC)
            && match(TOKEN_IDENTIFIER)
            && param_list()
            && func_ret_type()
            && code()
            && nl();
    return success;
}

bool func_ret_type() {
    bool success = true;
    switch (lookahead->type) {
        case TOKEN_LEFT_BRACE:
            success = true;
            break;
        case TOKEN_ARROW:
            success = success && type();
            break;
        default:
            success = false;
            break;
    }
    return success;
}

bool param_list(){
    bool success = true;
    switch (lookahead->type) {
        case TOKEN_IDENTIFIER:
            success = success && next_param();
            break;
        case TOKEN_UNDERSCORE:
            success = success && next_param();
            break;
        case TOKEN_LEFT_BRACKET:
            success = true;
            break;
        default:
            success = false;
            break;
    }
    return success;
}

bool param(){
    bool success = true;
    switch (lookahead->type){
        case TOKEN_IDENTIFIER:
            success = success   && param_name()
                                && match(TOKEN_IDENTIFIER)
                                && match(TOKEN_COLON)
                                && type();
            break;
        case TOKEN_UNDERSCORE:
            success = success   && param_name()
                                && match(TOKEN_IDENTIFIER)
                                && match(TOKEN_COLON)
                                && type();
            break;
        default:
            success = false;
            break;
    }
    return success;
}

bool param_name(){
    bool success = true;
    switch (lookahead->type){
        case TOKEN_IDENTIFIER:
            success = success && match(TOKEN_IDENTIFIER);
            break;
        case TOKEN_UNDERSCORE:
            success = success && match(TOKEN_UNDERSCORE);
            break;
        default:
            success = false;
            break;
    }
    return success;
}

bool next_param{
        bool success = true;
        switch(look)
};

bool branch(){

}