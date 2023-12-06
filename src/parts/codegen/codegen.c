/*
 * IFJ Project 2023
 * Implementation of code generator
 * Author: Kirill Shchetiniuk (xshche05)
 */

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

/**
 * @brief Generates header of the code for IFJcode23 interpreter. In first cycle of parser does nothing.
 */
void gen_header() {
    start;
    gen_line(".IFJcode23\n");
    gen_register_def();
    gen_line("JUMP $$main\n");
}

/**
 * @brief Generate definition of all general purpose "registers". In first cycle of parser does nothing.
 */
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

/**
 * @brief Generates definition of all general purpose "registers". In first cycle of parser does nothing.
 * @param prefix Prefix of the label
 * @return Pointer to generated label
 */
char *gen_unique_label(char *prefix) {
    start NULL;
    static int label = 0;
    char *str = safe_malloc(sizeof(char) * strlen(prefix) + 10);
    sprintf(str, "%s$%X", prefix, label);
    label++;
    return str;
}

/**
 * @brief Generates definition of all standard functions of IFJ23 language. In first cycle of parser does nothing.
 */
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
    gen_line("POPS GF@$RET\n");
    gen_line("JUMPIFNEQ $??op_skip GF@$RET nil@nil\n");
    gen_line("MOVE GF@$RET GF@$B\n");
    gen_line("LABEL $??op_skip\n");
    gen_line("RETURN\n");
    // Start of main
    gen_line("LABEL $$main\n");
}

/**
 * @brief Generates line of code. Wrapper for printf. In first cycle of parser does nothing.
 * @param format Format of the line
 * @param ... Arguments for the format
 */
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

/**
 * @brief Generates break jump for current loop. In first cycle of parser does nothing.
 */
void gen_break() {
    start;
    gen_line("JUMP %s\n", current_loop_end_label);
}

/**
 * @brief Generates continue jump for current loop, in case of for loop increase counter.
 * In first cycle of parser does nothing.
 */
void gen_continue() {
    start;
    if (current_loop_is_for) {
        gen_line("ADD %s %s int@1\n", for_counter_reg, for_counter_reg);
    }
    gen_line("JUMP %s\n", current_loop_start_label);
}

/**
 * @brief Generates return for current function. In first cycle of parser does nothing.
 * @param void_return True if function should returns void, false otherwise
 */
void gen_return(bool void_return) {
    start;
    if (!void_return) {
        gen_line("POPS %s\n", ret_reg);
    }
    gen_line("POPFRAME\n");
    gen_line("RETURN\n");
}

/**
 * @brief Generates labels for branch. In first cycle of parser does nothing.
 * @param gen_end True if should generate end label, false otherwise
 */
void gen_branch_labels(bool gen_end) {
    start;
    if (gen_end) {
        char *label_end = gen_unique_label("branch_end");
        Stack.push(branch_label_stack, label_end);
    }
    char *label_skip = gen_unique_label("branch_skip");
    Stack.push(branch_label_stack, label_skip);
}

/**
 * @brief Generates start of one branch condition. In first cycle of parser does nothing.
 * @param let True if branch has let id condition, false otherwise
 */
void gen_branch_if_start(bool let) {
    start;
    char *label_skip = (char*)Stack.top(branch_label_stack);
    if (let) {
        gen_line("POPS %s\n", cond_reg);
        gen_line("JUMPIFEQ %s %s nil@nil\n", label_skip, cond_reg);
    } else {
        gen_line("PUSHS bool@true\n");
        gen_line("JUMPIFNEQS %s\n", label_skip);
    }
}

/**
 * @brief Generates end of one branch condition. Generates jump to the end of current branch.
 * In first cycle of parser does nothing.
 */
void gen_branch_if_end() {
    start;
    char *label_skip = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    char *label_end = (char*)Stack.top(branch_label_stack);
    gen_line("JUMP %s\n", label_end);
    gen_line("LABEL %s\n", label_skip);
}

/**
 * @brief Generates end of current branch. In first cycle of parser does nothing.
 */
void gen_branch_end() {
    start;
    char *label_end = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    gen_line("LABEL %s\n", label_end);
}

/**
 * @brief Generates start of while loop. In first cycle of parser does nothing.
 */
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

/**
 * @brief Generates condition of while loop. In first cycle of parser does nothing.
 */
void gen_while_cond() {
    start;
    gen_line("PUSHS bool@true\n");
    gen_line("JUMPIFNEQS %s\n", current_loop_end_label);
}

/**
 * @brief Generates end of while loop. In first cycle of parser does nothing.
 */
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

/**
 * @brief Generates start of for loop. In first cycle of parser does nothing.
 */
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

/**
 * @brief Generates save of previous for loop range. In first cycle of parser does nothing.
 */
void gen_for_range_save() {
    start;
    gen_line("PUSHS %s\n", for_counter_reg);
    gen_line("PUSHS %s\n", for_max_val);
}

/**
 * @brief Generates restore of previous for loop range. In first cycle of parser does nothing.
 */
void gen_for_range_restore() {
    start;
    gen_line("POPS %s\n", for_max_val);
    gen_line("POPS %s\n", for_counter_reg);
}

/**
 * @brief Generates range of for loop. In first cycle of parser does nothing.
 * @param open True if range is open, false otherwise
 */
void gen_for_range(bool open) {
    start;
    if (!open) {
        gen_line("PUSHS int@1\n");
        gen_line("ADDS\n");
    }
    gen_line("POPS %s\n", for_max_val);
    gen_line("POPS %s\n", for_counter_reg);
}

/**
 * @brief Generates condition of for loop. In first cycle of parser does nothing.
 */
void gen_for_cond() {
    start;
    gen_line("JUMPIFEQ %s %s %s\n", current_loop_end_label, for_counter_reg, for_max_val);
}

/**
 * @brief Generates end of for loop. In first cycle of parser does nothing.
 */
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

/**
 * @brief Generates function label. In first cycle of parser does nothing.
 * @param func_name Name of the function
 */
void gen_func_label(char *func_name) {
    start;
    gen_line("LABEL %s\n", func_name);
}

/**
 * @brief Generates new frame creation. In first cycle of parser does nothing.
 */
void gen_new_frame() {
    start;
    gen_line("CREATEFRAME\n");
}

/**
 * @brief Generates push frame. In first cycle of parser does nothing.
 */
void gen_push_frame() {
    start;
    gen_line("PUSHFRAME\n");
    Stack.push(array_stack, used_vars);
    used_vars = DynamicArray.ctor();
}

/**
 * @brief Generates pop frame. In first cycle of parser does nothing.
 */
void gen_pop_frame() {
    start;
    gen_line("POPFRAME\n");
}

/**
 * @brief Generates definition of used variables in current frame. In first cycle of parser does nothing.
 */
void gen_used_vars() {
    start;
    for (int i = 0; i < used_vars->size; i++) {
        gen_line("DEFVAR %s\n", (char*)DynamicArray.get(used_vars, i));
    }
    DynamicArray.dtor(used_vars);
    used_vars = (dynamic_array_t*)Stack.top(array_stack);
    Stack.pop(array_stack);
}

/**
 * @brief Generates definition of used global variables. In first cycle of parser does nothing.
 */
void gen_used_global_vars() {
    start;
    for (int i = 0; i < global_vars->size; i++) {
        gen_line("DEFVAR %s\n", (char*)DynamicArray.get(global_vars, i));
    }
    DynamicArray.dtor(global_vars);
}

/**
 * @brief Generates pop of func parameters from stack. In first cycle of parser does nothing.
 * @param params Parameters of function
 */
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

/**
 * @brief Generates name of variable. Save generated name to array of used vars in this frame.
 * In first cycle of parser does nothing.
 * @param id Name of variable
 * @param scope Scope of variable
 * @return Pointer to generated name
 */
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
