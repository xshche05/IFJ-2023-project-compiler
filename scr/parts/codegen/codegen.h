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

#endif //IFJ_PRJ_CODEGEN_H
