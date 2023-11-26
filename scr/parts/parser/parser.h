#ifndef IFJ_PRJ_PARSER_H
#define IFJ_PRJ_PARSER_H

#include <stdbool.h>
#include "token.h"

extern token_t *lookahead; // TODO
extern bool nl_flag;    // new line // TODO

bool match(token_type_t type);

bool call_expr_parser();

int S();
bool CODE();
bool RETURN();
bool RET_EXPR();
bool VAR_DECL();
bool VAR_LET_TYPE();
bool VAR_LET_EXP();
bool LET_DECL();
bool FUNC_DECL();
bool FUNC_RET_TYPE();
bool PARAM_LIST();
bool PARAM();
bool PARAM_NAME();
bool NEXT_PARAM();
bool BRANCH();
bool BR_EXPR();
bool ELSE();
bool ELSE_IF();
bool WHILE_LOOP();
bool FOR_LOOP();
bool FOR_ID();
bool RANGE();
bool CALL_PARAM_LIST();
bool CALL_PARAM();
bool CALL_PARAM_NAME();
bool CALL_PARAM_VALUE();
bool NEXT_CALL_PARAM();
bool ID_CALL_OR_ASSIGN();
bool NEXT_ID_CALL_OR_ASSIGN();
bool TYPE();
bool EXPR();
#endif //IFJ_PRJ_PARSER_H
