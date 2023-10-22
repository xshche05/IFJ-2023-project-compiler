//
// Created by Spagetik on 10.10.2023.
//


#include <stdbool.h>
#include "token/token.h"
#include "parser.h"
#include "expr_parser.h"

bool match(token_type_t type) {
    if (type != TOKEN_NEWLINE)
    {
        while (lookahead == TOKEN_NEWLINE)
        {
            lookahead = TokenArray.next()->type;
        }
    }
    if (lookahead == type) {
        lookahead = TokenArray.next()->type;
        return true;
    }
    printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
}

bool POS_CALL_PARAM() {
    parse_expr();
    return true;
}

bool NAMED_CALL_PARAM() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
            flag &= match(TOKEN_IDENTIFIER);
            flag &= match(TOKEN_ASSIGNMENT);
            parse_expr();
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool NEXT_CALL_PARAM() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_COMMA:
            flag &= match(TOKEN_COMMA);
            flag &= CALL_PARAM();
            flag &= NEXT_CALL_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool CALL_PARAM() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
            flag &= NAMED_CALL_PARAM();
            break;
        default:
            flag &= POS_CALL_PARAM();
    }
    return flag;
}

bool PARAM_NAME() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
            flag &= match(TOKEN_IDENTIFIER);
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool CALL_PARAM_LIST() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            flag &= CALL_PARAM();
            flag &= NEXT_CALL_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            return true;
        default:
            flag &= CALL_PARAM();
            flag &= NEXT_CALL_PARAM();
    }
    return flag;
}

bool ELSE_IF() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IF:
            flag &= match(TOKEN_IF);
            parse_expr();
            flag &= match(TOKEN_LEFT_BRACE);
            flag &= CODE();
            flag &= match(TOKEN_RIGHT_BRACE);
            flag &= ELSE();
            break;
        case TOKEN_LEFT_BRACE:
            flag &= match(TOKEN_LEFT_BRACE);
            flag &= CODE();
            flag &= match(TOKEN_RIGHT_BRACE);
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool NEXT_PARAM() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_COMMA:
            flag &= match(TOKEN_COMMA);
            flag &= PARAM();
            flag &= NEXT_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool PARAM() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
            flag &= match(TOKEN_IDENTIFIER);
            flag &= match(TOKEN_COLON);
            switch (lookahead) {
                case TOKEN_STRING_TYPE:
                    flag &= match(TOKEN_STRING_TYPE);
                    break;
                case TOKEN_INT_TYPE:
                    flag &= match(TOKEN_INT_TYPE);
                    break;
                case TOKEN_BOOL_TYPE:
                    flag &= match(TOKEN_BOOL_TYPE);
                    break;
                case TOKEN_DOUBLE_TYPE:
                    flag &= match(TOKEN_DOUBLE_TYPE);
                    break;
                default:
                    printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
            }
            break;
        case TOKEN_UNDERSCORE:
            flag &= match(TOKEN_UNDERSCORE);
            flag &= match(TOKEN_COLON);
            switch (lookahead) {
                case TOKEN_STRING_TYPE:
                    flag &= match(TOKEN_STRING_TYPE);
                    break;
                case TOKEN_INT_TYPE:
                    flag &= match(TOKEN_INT_TYPE);
                    break;
                case TOKEN_BOOL_TYPE:
                    flag &= match(TOKEN_BOOL_TYPE);
                    break;
                case TOKEN_DOUBLE_TYPE:
                    flag &= match(TOKEN_DOUBLE_TYPE);
                    break;
                default:
                    printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
            }
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool NEXT_ID_CALL_OR_ASSIGN() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_ASSIGNMENT:
            flag &= match(TOKEN_ASSIGNMENT);
            parse_expr();
            break;
        case TOKEN_LEFT_BRACKET:
            flag &= match(TOKEN_LEFT_BRACKET);
            flag &= CALL_PARAM_LIST();
            flag &= match(TOKEN_RIGHT_BRACKET);
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool RET_EXPR() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_NEWLINE:
            return true;
        default:
            parse_expr();
    }
    return flag;
}

bool ELSE() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_ELSE:
            flag &= match(TOKEN_ELSE);
            flag &= ELSE_IF();
            break;
        case TOKEN_NEWLINE:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool BR_EXPR() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_LET:
            flag &= match(TOKEN_LET);
            flag &= match(TOKEN_IDENTIFIER);
            break;
        default:
            parse_expr();
    }
    return flag;
}

bool RANGE() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_CLOSED_RANGE:
            flag &= match(TOKEN_CLOSED_RANGE);
            parse_expr();
            break;
        case TOKEN_HALF_OPEN_RANGE:
            flag &= match(TOKEN_HALF_OPEN_RANGE);
            parse_expr();
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool FOR_ID() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
            flag &= match(TOKEN_IDENTIFIER);
            break;
        case TOKEN_UNDERSCORE:
            flag &= match(TOKEN_UNDERSCORE);
            break;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool CODE_2() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_VAR:
            flag &= VAR_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_LET:
            flag &= LET_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_FUNC:
            flag &= FUNC_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_WHILE:
            flag &= WHILE_LOOP();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_FOR:
            flag &= FOR_LOOP();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_IF:
            flag &= BRANCH();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_IDENTIFIER:
            flag &= ID_CALL_OR_ASSIGN();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_RETURN:
            flag &= RETURN();
            flag &= match(TOKEN_NEWLINE);
            break;
        case TOKEN_BREAK:
            match(TOKEN_BREAK);
            return true;
        case TOKEN_CONTINUE:
            match(TOKEN_CONTINUE);
            return true;
        case TOKEN_RIGHT_BRACE:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool FUNC_RET_TYPE() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_ARROW:
            flag &= match(TOKEN_ARROW);
            switch (lookahead) {
                case TOKEN_STRING_TYPE:
                    flag &= match(TOKEN_STRING_TYPE);
                    break;
                case TOKEN_INT_TYPE:
                    flag &= match(TOKEN_INT_TYPE);
                    break;
                case TOKEN_BOOL_TYPE:
                    flag &= match(TOKEN_BOOL_TYPE);
                    break;
                case TOKEN_DOUBLE_TYPE:
                    flag &= match(TOKEN_DOUBLE_TYPE);
                    break;
                default:
                    printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
            }
            break;
        case TOKEN_LEFT_BRACE:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool PARAM_LIST() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_IDENTIFIER:
        case TOKEN_UNDERSCORE:
            flag &= PARAM();
            flag &= NEXT_PARAM();
            break;
        case TOKEN_RIGHT_BRACKET:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool VAR_LET_EXPR() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_ASSIGNMENT:
            flag &= match(TOKEN_ASSIGNMENT);
            parse_expr();
            break;
        case TOKEN_NEWLINE:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool VAR_LET_TYPE() {
    bool flag = true;
    switch (lookahead) {
        case TOKEN_COLON:
            flag &= match(TOKEN_COLON);
            switch (lookahead) {
                case TOKEN_STRING_TYPE:
                    flag &= match(TOKEN_STRING_TYPE);
                    break;
                case TOKEN_INT_TYPE:
                    flag &= match(TOKEN_INT_TYPE);
                    break;
                case TOKEN_BOOL_TYPE:
                    flag &= match(TOKEN_BOOL_TYPE);
                    break;
                case TOKEN_DOUBLE_TYPE:
                    flag &= match(TOKEN_DOUBLE_TYPE);
                    break;
                default:
                    printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
            }
            break;
        case TOKEN_ASSIGNMENT:
        case TOKEN_NEWLINE:
            return true;
        default:
            printf("ERROR!!!!:::%d\n\n", __LINE__);
return false;
    }
    return flag;
}

bool RETURN() {
    bool flag = true;
    flag &= match(TOKEN_RETURN);
    flag &= RET_EXPR();
    return flag;
}

bool ID_CALL_OR_ASSIGN() {
    bool flag = true;
    flag &= match(TOKEN_IDENTIFIER);
    flag &= NEXT_ID_CALL_OR_ASSIGN();
    return flag;
}

bool FOR_LOOP() {
    bool flag = true;
    flag &= match(TOKEN_FOR);
    flag &= FOR_ID();
    flag &= match(TOKEN_IN);
    parse_expr();
    flag &= match(TOKEN_LEFT_BRACE);
    flag &= CODE();
    flag &= match(TOKEN_RIGHT_BRACE);
    return flag;
}

bool BRANCH() {
    bool flag = true;
    flag &= match(TOKEN_IF);
    flag &= BR_EXPR();
    flag &= match(TOKEN_LEFT_BRACE);
    flag &= CODE();
    flag &= match(TOKEN_RIGHT_BRACE);
    flag &= ELSE();
    return flag;
}

bool WHILE_LOOP() {
    bool flag = true;
    flag &= match(TOKEN_WHILE);
    parse_expr();
    flag &= match(TOKEN_LEFT_BRACE);
    flag &= CODE();
    flag &= match(TOKEN_RIGHT_BRACE);
    return flag;
}

bool FUNC_DECL() {
    bool flag = true;
    flag &= match(TOKEN_FUNC);
    flag &= match(TOKEN_IDENTIFIER);
    flag &= match(TOKEN_LEFT_BRACKET);
    flag &= PARAM_LIST();
    flag &= match(TOKEN_RIGHT_BRACKET);
    flag &= FUNC_RET_TYPE();
    flag &= match(TOKEN_LEFT_BRACE);
    flag &= CODE();
    flag &= match(TOKEN_RIGHT_BRACE);
    return flag;
}


bool LET_DECL() {
    bool flag = true;
    flag &= match(TOKEN_LET);
    flag &= match(TOKEN_IDENTIFIER);
    flag &= VAR_LET_TYPE();
    flag &= VAR_LET_EXPR();
    return flag;
}


bool VAR_DECL() {
    bool flag = true;
    flag &= match(TOKEN_VAR);
    flag &= match(TOKEN_IDENTIFIER);
    flag &= VAR_LET_TYPE();
    flag &= VAR_LET_EXPR();
    return flag;
}

bool CODE() {
    bool flag = true;
    while (lookahead == TOKEN_NEWLINE) {
        lookahead = TokenArray.next()->type;
    }
    switch (lookahead) {
        case TOKEN_VAR:
            flag &= VAR_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_LET:
            flag &= LET_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_FUNC:
            flag &= FUNC_DECL();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_WHILE:
            flag &= WHILE_LOOP();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_FOR:
            flag &= FOR_LOOP();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_IF:
            flag &= BRANCH();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_IDENTIFIER:
            flag &= ID_CALL_OR_ASSIGN();
            flag &= match(TOKEN_NEWLINE);
            flag &= CODE();
            break;
        case TOKEN_RETURN:
            flag &= RETURN();
            flag &= match(TOKEN_NEWLINE);
            break;
        case TOKEN_BREAK:
            match(TOKEN_BREAK);
            return true;
        case TOKEN_CONTINUE:
            match(TOKEN_CONTINUE);
            return true;
        default:
            return true;
    }
    return flag;
}


bool PROG() {
    return CODE();
}

bool parse() {
    lookahead = TokenArray.next()->type;
    return PROG();
}
