/*
 * IFJ Project 2023
 * Implementation of code generator
 * Author: Kirill Shchetiniuk (xshche05)
 */

#ifndef IFJ_PRJ_CODEGEN_H
#define IFJ_PRJ_CODEGEN_H

#include <stdbool.h>
#include "symtable.h"

//extern string_t *output;

void gen_register_def();

char *gen_unique_label(char *prefix);

void gen_std_functions();

void gen_line(char *format, ...);

void gen_break();

void gen_continue();

void gen_return(bool void_return);

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

void gen_func_label(char *func_name);

void gen_new_frame();

void gen_push_frame();

void gen_pop_frame();

void gen_pop_params(string_t *params);

char *gen_var_name(char *id, int scope);

void gen_used_global_vars();

void gen_used_vars();

#endif //IFJ_PRJ_CODEGEN_H
