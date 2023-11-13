#ifndef IFJ_PRJ_PARSER_H
#define IFJ_PRJ_PARSER_H

#include <stdbool.h>
#include "token.h"

token_t *lookahead;
bool nl_flag;    // new line

bool match(token_type_t type);

bool parse();

bool code();
bool return_r();
bool ret_expr();
bool var_decl();
bool var_let_type();
bool var_let_exp();
bool let_decl();
bool func_decl();
bool func_ret_type();
bool param_list();
bool param();
bool param_name();
bool next_param();
bool branch();
bool br_expr();
bool else_r();
bool else_if();
bool while_loop();
bool for_loop();
bool for_id();
bool range();
bool call_param_list();
bool call_param();
bool named_call_param();
bool call_param_name();
bool pos_call_param();
bool next_call_param();
bool id_call_or_assign();
bool next_id_call_or_assign();
bool soft_nl();
bool nl();
bool type();


#endif //IFJ_PRJ_PARSER_H
