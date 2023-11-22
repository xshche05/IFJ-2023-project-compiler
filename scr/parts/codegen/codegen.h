//
// Created by Spagetik on 16-Nov-23.
//

#ifndef IFJ_PRJ_CODEGEN_H
#define IFJ_PRJ_CODEGEN_H

#include <stdbool.h>

void gen_register_def();

char *gen_unique_label(char *prefix);

char *gen_scope_var(char *var_name, int scope, int stayed, bool with_frame);

void gen_std_functions();

void gen_line(char *line);

void gen_break();

void gen_continue();

void gen_return(bool void_return);

void gen_var_decl(char *var_name, int scope, int stayed);

void gen_var_assign(char *var_name, int scope, int stayed);

void gen_branch_labels(bool gen_end);

void gen_branch_if_start(bool let);

void gen_branch_if_end();

void gen_branch_end();

void gen_while_start();

void gen_while_cond();

void gen_while_end();

void gen_for_start();

void gen_for_range_save();

void gen_for_range_restore();

void gen_for_range(bool open);

void gen_for_cond();

void gen_for_end();

void init_codegen();

void gen_header();

#endif //IFJ_PRJ_CODEGEN_H
