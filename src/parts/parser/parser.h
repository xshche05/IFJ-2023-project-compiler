/*
 * IFJ Project 2023
 * Implementation of expression parser
 * Author: Kirill Shchetiniuk (xshche05)
 * Author: Nadzeya Antsipenka (xantsi00)
 * Author: Veranika Saltanava (xsalta01)
 * Author: Lilit Movsesian (xmovse00)
 */
#ifndef IFJ_PRJ_PARSER_H
#define IFJ_PRJ_PARSER_H

#include <stdbool.h>
#include "token.h"
#include "symtable.h"

extern token_t *lookahead;
extern bool nl_flag;
extern bool collect_funcs;
extern int inside_branch;

bool match(token_type_t type);

bool call_expr_parser(type_t *type, bool *is_literal);

int S();
bool CODE();
bool RETURN(funcData_t **funcData);
bool RET_EXPR(funcData_t **funcData);
bool VAR_DECL();
bool VAR_LET_TYPE(type_t *type);
bool VAR_LET_EXP(type_t *type, bool *is_literal);
bool LET_DECL();
bool FUNC_DECL();
bool FUNC_RET_TYPE(type_t *type);
bool PARAM_LIST(funcData_t **funcData);
bool PARAM(funcData_t **funcData);
bool PARAM_NAME(funcData_t **funcData);
bool NEXT_PARAM(funcData_t **funcData);
bool BRANCH();
bool BR_EXPR();
bool ELSE();
bool ELSE_IF();
bool WHILE_LOOP();
bool FOR_LOOP();
bool FOR_ID(token_t **id);
bool RANGE();
bool CALL_PARAM_LIST(bool call_after_param, char *func_name);
bool CALL_PARAM(bool call_after_param, char *func_name, int *call_param_num);
bool NEXT_CALL_PARAM(bool call_after_param, char *func_name, int *call_param_num);
bool ID_CALL_OR_ASSIGN();
bool NEXT_ID_CALL_OR_ASSIGN(token_t *id);
bool TYPE(type_t *type);
#endif //IFJ_PRJ_PARSER_H
