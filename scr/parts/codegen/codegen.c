//
// Created by Spagetik on 16-Nov-23.
//

#include "codegen.h"
#include "stack.h"
#include "parser.h"
#include "lists.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "memory.h"

static char* registers[] = {
        "GF@$A",
        "GF@$B",
        "GF@$C",
        "GF@$D",
        "GF@$RET",
        "GF@$COND",
        "GF@$FOR_COUNTER",
        "GF@$FOR_MAX_VAL",
};

char *cond_reg = "GF@$COND";
char *ret_reg = "GF@$RET";
char *for_counter_reg = "GF@$FOR_COUNTER";
char *for_max_val = "GF@$FOR_MAX_VAL";

//string_t *output = NULL;

stack_t *branch_label_stack;
stack_t *loop_label_stack;
stack_t *cycle_type_stack;
dynamic_array_t *used_vars = NULL;
dynamic_array_t *global_vars = NULL;
stack_t *array_stack;

char* current_loop_start_label = NULL;
char* current_loop_end_label = NULL;

bool current_loop_is_for = false;

#define start if (collect_funcs) return

void init_codegen() {
    start;
//    output = String.ctor();
    branch_label_stack = Stack.init();
    loop_label_stack = Stack.init();
    cycle_type_stack = Stack.init();
    array_stack = Stack.init();
    global_vars = DynamicArray.ctor();
}

void gen_header() {
    start;
    gen_line(".IFJcode23\n");
    gen_register_def();
    gen_line("JUMP $$main\n");
}

void gen_register_def() {
    start;
    for (int i = 0; i < 8; i++) {
        char reg[30];
        strncpy(reg, registers[i], 30);
        gen_line("DEFVAR %s\n", reg);
    }
    gen_line("MOVE GF@$FOR_COUNTER int@0\n");
    gen_line("MOVE GF@$FOR_MAX_VAL int@0\n");
}

char *gen_unique_label(char *prefix) {
    start NULL;
    static int label = 0;
    char *str = safe_malloc(sizeof(char) * strlen(prefix) + 10);
    sprintf(str, "%s$%X", prefix, label);
    label++;
    return str;
}

void gen_std_functions() {
    start;
    // readString
    gen_line("LABEL readString\n");
    gen_line("READ GF@$RET string\n");
    gen_line("RETURN\n");
    // readInt
    gen_line("LABEL readInt\n");
    gen_line("READ GF@$RET int\n");
    gen_line("RETURN\n");
    // readDouble
    gen_line("LABEL readDouble\n");
    gen_line("READ GF@$RET float\n");
    gen_line("RETURN\n");
    // readBool
    gen_line("LABEL readBool\n");
    gen_line("READ GF@$RET bool\n");
    gen_line("RETURN\n");
    // write
    gen_line("LABEL write\n");
    // while $C != 0 do pop and print decremented $C
    gen_line("POPS GF@$A\n");
    gen_line("WRITE GF@$A\n");
    gen_line("RETURN\n");
    // Int2Double
    gen_line("LABEL Int2Double\n");
    gen_line("POPS GF@$A\n");
    gen_line("INT2FLOAT GF@$RET GF@$A\n");
    gen_line("RETURN\n");
    // Double2Int
    gen_line("LABEL Double2Int\n");
    gen_line("POPS GF@$A\n");
    gen_line("FLOAT2INT GF@$RET GF@$A\n");
    gen_line("RETURN\n");
    // length
    gen_line("LABEL length\n");
    gen_line("POPS GF@$A\n");
    gen_line("STRLEN GF@$RET GF@$A\n");
    gen_line("RETURN\n");
    // substring
    gen_line("LABEL substring\n");
    gen_line("CREATEFRAME\n");
    gen_line("PUSHFRAME\n");
    gen_line("POPS GF@$C\n");
    gen_line("POPS GF@$B\n");
    gen_line("POPS GF@$A\n");
    // A - string (s)
    // B - start (i)
    // C - length (j)
    // if i  < 0 || j < 0 || i > j || i >= length(s) || j > length(s) then $RET = nil
    gen_line("DEFVAR LF@exit\n");
    gen_line("LT LF@exit GF@$B int@0\n"); // exit = i < 0
    gen_line("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    gen_line("LT LF@exit GF@$C int@0\n"); // exit = j < 0
    gen_line("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    gen_line("GT LF@exit GF@$B GF@$C\n"); // exit = i > j
    gen_line("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    gen_line("PUSHS GF@$A\n");
    gen_line("CALL length\n"); // $RET = length(s)
    gen_line("LT LF@exit GF@$B GF@$RET\n"); // exit = !(i >= length(s))
    gen_line("JUMPIFEQ $substring_std_exit_nil LF@exit bool@false\n");
    gen_line("GT LF@exit GF@$C GF@$RET\n"); // exit = j > length(s)
    gen_line("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    gen_line("MOVE GF@$RET string@\n"); // $RET = ""
    gen_line("LABEL $substring_std_loop\n");
    gen_line("LT LF@exit GF@$B GF@$C\n"); // continue = i < j
    gen_line("JUMPIFEQ $substring_std_exit LF@exit bool@false\n"); // if continue == false then exit
    gen_line("GETCHAR GF@$D GF@$A GF@$B\n"); // $D = s[i]
    gen_line("CONCAT GF@$RET GF@$RET GF@$D\n"); // $RET = $RET + $D
    gen_line("ADD GF@$B GF@$B int@1\n"); // i++
    gen_line("JUMP $substring_std_loop\n"); // go to loop
    gen_line("LABEL $substring_std_exit\n"); // exit loop
    gen_line("POPFRAME\n"); // pop frame
    gen_line("RETURN\n"); // return
    gen_line("LABEL $substring_std_exit_nil\n"); // exit nil
    gen_line("MOVE GF@$RET nil@nil\n"); // $RET = nil
    gen_line("POPFRAME\n"); // pop frame
    gen_line("RETURN\n"); // return
    // ord
    gen_line("LABEL ord\n");
    gen_line("POPS GF@$A\n");
    gen_line("PUSHS GF@$A\n");
    gen_line("CALL length\n");
    gen_line("JUMPIFEQ $ord_std_exit GF@$RET int@0\n");
    gen_line("STRI2INT GF@$RET GF@$A int@0\n");
    gen_line("LABEL $ord_std_exit\n");
    gen_line("RETURN\n");
    // chr
    gen_line("LABEL chr\n");
    gen_line("POPS GF@$A\n");
    gen_line("INT2CHAR GF@$RET GF@$A\n");
    gen_line("RETURN\n");
    // ??
    gen_line("LABEL $??op\n");
    gen_line("POPS GF@$B\n");
    gen_line("POPS GF@$A\n");
    gen_line("TYPE GF@$C GF@$A\n");
    gen_line("MOVE GF@$RET GF@$A\n");
    gen_line("JUMPIFNEQ $??op_skip GF@$C string@nil\n");
    gen_line("MOVE GF@$RET GF@$B\n");
    gen_line("LABEL $??op_skip\n");
    gen_line("RETURN\n");
    // Start of main
    gen_line("LABEL $$main\n");
}

void gen_line(char *format, ...) {
    start;
    va_list args;
    va_start(args, format);
//    char *str = safe_malloc(sizeof(char) * (strlen(format) + 100) * 2);
//    vsprintf(str, format, args);
//    String.add_cstr(output, str);
//    safe_free(str);
    vprintf(format, args);
    va_end(args);
}

void gen_break() {
    start;
    gen_line("JUMP %s\n", current_loop_end_label);
}

void gen_continue() {
    start;
    if (current_loop_is_for) {
        gen_line("ADD %s %s int@1\n", for_counter_reg, for_counter_reg);
    }
    gen_line("JUMP %s\n", current_loop_start_label);
}

void gen_return(bool void_return) {
    start;
    if (!void_return) {
        gen_line("POPS %s\n", ret_reg);
    }
    gen_line("POPFRAME\n");
    gen_line("RETURN\n");
}

void gen_branch_labels(bool gen_end) {
    start;
    if (gen_end) {
        char *label_end = gen_unique_label("branch_end");
        Stack.push(branch_label_stack, label_end);
    }
    char *label_skip = gen_unique_label("branch_skip");
    Stack.push(branch_label_stack, label_skip);
}

void gen_branch_if_start(bool let) {
    start;
    char *label_skip = (char*)Stack.top(branch_label_stack);
    if (let) {
        gen_line("POPS %s\n", cond_reg);
        gen_line("TYPE %s %s\n", cond_reg, cond_reg);
        gen_line("JUMPIFEQ %s %s string@nil\n", label_skip, cond_reg);
    } else {
        gen_line("PUSHS bool@true\n");
        gen_line("JUMPIFNEQS %s\n", label_skip);
    }
}

void gen_branch_if_end() {
    start;
    char *label_skip = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    char *label_end = (char*)Stack.top(branch_label_stack);
    gen_line("JUMP %s\n", label_end);
    gen_line("LABEL %s\n", label_skip);
}

void gen_branch_end() {
    start;
    char *label_end = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    gen_line("LABEL %s\n", label_end);
}

void gen_while_start() {
    start;
    bool *tmp = safe_malloc(sizeof(bool));
    *tmp = current_loop_is_for;
    Stack.push(cycle_type_stack, tmp);
    current_loop_is_for = false;
    char *label_start = gen_unique_label("while_start");
    char *label_end = gen_unique_label("while_end");
    if (current_loop_start_label) {
        Stack.push(loop_label_stack, current_loop_start_label);
        Stack.push(loop_label_stack, current_loop_end_label);
        current_loop_start_label = label_start;
        current_loop_end_label = label_end;
    }
    else {
        current_loop_start_label = label_start;
        current_loop_end_label = label_end;
    }
    gen_line("LABEL %s\n", current_loop_start_label);
}

void gen_while_cond() {
    start;
    gen_line("PUSHS bool@true\n");
    gen_line("JUMPIFNEQS %s\n", current_loop_end_label);
}

void gen_while_end() {
    start;
    gen_line("JUMP %s\n", current_loop_start_label);
    gen_line("LABEL %s\n", current_loop_end_label);
    if (Stack.top(loop_label_stack))
    {
        current_loop_end_label = (char*)Stack.top(loop_label_stack);
        Stack.pop(loop_label_stack);
        current_loop_start_label = (char*)Stack.top(loop_label_stack);
        Stack.pop(loop_label_stack);
    }
    else
    {
        current_loop_start_label = NULL;
        current_loop_end_label = NULL;
    }
    current_loop_is_for = *(bool*)Stack.top(cycle_type_stack);
    Stack.pop(cycle_type_stack);
}

void gen_for_start() {
    start;
    bool *tmp = safe_malloc(sizeof(bool));
    *tmp = current_loop_is_for;
    Stack.push(cycle_type_stack, tmp);
    current_loop_is_for = true;
    char *label_start = gen_unique_label("for_start");
    char *label_end = gen_unique_label("for_end");
    if (current_loop_start_label) {
        Stack.push(loop_label_stack, current_loop_start_label);
        Stack.push(loop_label_stack, current_loop_end_label);
        current_loop_start_label = label_start;
        current_loop_end_label = label_end;
    }
    else {
        current_loop_start_label = label_start;
        current_loop_end_label = label_end;
    }
    gen_line("LABEL %s\n", label_start);
}

void gen_for_range_save() {
    start;
    gen_line("PUSHS %s\n", for_counter_reg);
    gen_line("PUSHS %s\n", for_max_val);
}

void gen_for_range_restore() {
    start;
    gen_line("POPS %s\n", for_max_val);
    gen_line("POPS %s\n", for_counter_reg);
}

void gen_for_range(bool open) {
    start;
    if (!open) {
        gen_line("PUSHS int@1\n");
        gen_line("ADDS\n");
    }
    gen_line("POPS %s\n", for_max_val);
    gen_line("POPS %s\n", for_counter_reg);
}

void gen_for_cond() {
    start;
    gen_line("JUMPIFEQ %s %s %s\n", current_loop_end_label, for_counter_reg, for_max_val);
}

void gen_for_end() {
    start;
    gen_line("ADD %s %s int@1\n", for_counter_reg, for_counter_reg);
    gen_line("JUMP %s\n", current_loop_start_label);
    gen_line("LABEL %s\n", current_loop_end_label);
    if (Stack.top(loop_label_stack))
    {
        current_loop_end_label = (char*)Stack.top(loop_label_stack);
        Stack.pop(loop_label_stack);
        current_loop_start_label = (char*)Stack.top(loop_label_stack);
        Stack.pop(loop_label_stack);
    }
    else
    {
        current_loop_start_label = NULL;
        current_loop_end_label = NULL;
    }
    current_loop_is_for = *(bool*)Stack.top(cycle_type_stack);
    Stack.pop(cycle_type_stack);
}

void gen_func_label(char *func_name) {
    start;
    gen_line("LABEL %s\n", func_name);
}

void gen_new_frame() {
    start;
    gen_line("CREATEFRAME\n");
}

void gen_push_frame() {
    start;
    gen_line("PUSHFRAME\n");
    Stack.push(array_stack, used_vars);
    used_vars = DynamicArray.ctor();
}

void gen_pop_frame() {
    start;
    gen_line("POPFRAME\n");
}

void gen_used_vars() {
    start;
    for (int i = 0; i < used_vars->size; i++) {
        gen_line("DEFVAR %s\n", (char*)DynamicArray.get(used_vars, i));
    }
    DynamicArray.dtor(used_vars);
    used_vars = (dynamic_array_t*)Stack.top(array_stack);
    Stack.pop(array_stack);
}

void gen_used_global_vars() {
    start;
    for (int i = 0; i < global_vars->size; i++) {
        gen_line("DEFVAR %s\n", (char*)DynamicArray.get(global_vars, i));
    }
    DynamicArray.dtor(global_vars);
}

void gen_pop_params(string_t *params) {
    start;
    char *str = safe_malloc(sizeof(char) * (strlen(params->str) + 1));
    strcpy(str, params->str);

    char *token = strtok(str, "#:");

    int i = 0;

    stack_t *param_stack = Stack.init();

    while (token != NULL) {
        if (i % 3 == 1) {
            char *id = safe_malloc(sizeof(char) * (strlen(token) + 1));\
            strcpy(id, token);
            Stack.push(param_stack, id);
        }
        token = strtok(NULL, "#:");
        i++;
    }

    while (Stack.top(param_stack)) {
        char *id = (char*)Stack.top(param_stack);
        Stack.pop(param_stack);
        gen_line("POPS LF@%s%c1\n", id, '%');
        safe_free(id);
    }

    safe_free(str);
}

char *gen_var_name(char *id, int scope) {
    start NULL;
    char *str = safe_malloc(sizeof(char) * (strlen(id) + 10));
    if (scope == 0) {
        sprintf(str, "GF@%s", id);
        DynamicArray.add_unique_cstr(global_vars, str);
    } else {
        sprintf(str, "LF@%s%c%d", id, '%', scope);
        DynamicArray.add_unique_cstr(used_vars, str);
    }
    return str;
}
