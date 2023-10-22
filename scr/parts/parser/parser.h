//
// Created by Spagetik on 10.10.2023.
//

#ifndef IFJ_PRJ_PARSER_H
#define IFJ_PRJ_PARSER_H

#include <stdbool.h>
#include "token/token.h"

token_type_t lookahead;

bool PROG();

bool CODE();

bool RETURN();

bool RET_EXPR();

bool VAR_DECL();

bool VAR_LET_TYPE();

bool VAR_LET_EXPR();

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

bool NAMED_CALL_PARAM();

bool POS_CALL_PARAM();

bool NEXT_CALL_PARAM();

bool ID_CALL_OR_ASSIGN();

bool NEXT_ID_CALL_OR_ASSIGN();

bool parse();




#endif //IFJ_PRJ_PARSER_H
