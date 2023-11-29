//
// Created by Spagetik on 16-Nov-23.
//

#include "codegen.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

stack_t *branch_label_stack;
stack_t *loop_label_stack;
stack_t *cycle_type_stack;

char* current_loop_start_label = NULL;
char* current_loop_end_label = NULL;

bool current_loop_is_for = false;


void init_codegen() {
    branch_label_stack = Stack.init();
    loop_label_stack = Stack.init();
    cycle_type_stack = Stack.init();
}

void gen_header() {
    printf(".IFJcode23\n");
    gen_register_def();
    printf("JUMP $$main\n");
}

void gen_register_def() {
    for (int i = 0; i < 6; i++) {
        char reg[10];
        strncpy(reg, registers[i], 10);
        printf("DEFVAR %s\n", reg);
    }
}

char *gen_unique_label(char *prefix) {
    static int label = 0;
    char *str = malloc(sizeof(char) * strlen(prefix) + 10);
    sprintf(str, "%s_%X", prefix, label);
    label++;
    return str;
}

void gen_std_functions() {

    /*
     * readString
     * readInt
     * readDouble
     *
     * write
     *
     * Int2Double
     * Double2Int
     *
     * length
     *
     * substring
     *
     * ord
     *
     * chr
     */

    // readString
    printf("LABEL $readString_std\n");
    printf("READ GF@$RET string\n");
    printf("RETURN\n");
    // readInt
    printf("LABEL $readInt_std\n");
    printf("READ GF@$RET int\n");
    printf("RETURN\n");
    // readDouble
    printf("LABEL $readDouble_std\n");
    printf("READ GF@$RET float\n");
    printf("RETURN\n");
    // readBool
    printf("LABEL $readBool_std\n");
    printf("READ GF@$RET bool\n");
    printf("RETURN\n");

    // write
    printf("LABEL $write_std\n");
    // while $C != 0 do pop and print decremented $C
    printf("POPS GF@$C\n");
    printf("LABEL $write_std_write_loop\n");
    printf("JUMPIFEQ $write_std_write_end GF@$C int@0\n");
    printf("POPS GF@$A\n");
    printf("WRITE GF@$A\n");
    printf("SUB GF@$C GF@$C int@1\n");
    printf("JUMP $write_std_write_loop\n");
    printf("LABEL $write_std_write_end\n");
    printf("RETURN\n");

    // Int2Double
    printf("LABEL $Int2Double_std\n");
    printf("POPS GF@$A\n");
    printf("INT2FLOAT GF@$RET GF@$A\n");
    printf("RETURN\n");
    // Double2Int
    printf("LABEL $Double2Int_std\n");
    printf("POPS GF@$A\n");
    printf("FLOAT2INT GF@$RET GF@$A\n");
    printf("RETURN\n");
    // length
    printf("LABEL $length_std\n");
    printf("POPS GF@$A\n");
    printf("STRLEN GF@$RET GF@$A\n");
    printf("RETURN\n");
    // substring
    printf("LABEL $substring_std\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("POPS GF@$C\n");
    printf("POPS GF@$B\n");
    printf("POPS GF@$A\n");
    // A - string (s)
    // B - start (i)
    // C - length (j)
    // if i  < 0 || j < 0 || i > j || i >= length(s) || j > length(s) then $RET = nil
    printf("DEFVAR LF@exit\n");
    printf("LT LF@exit GF@$B int@0\n"); // exit = i < 0
    printf("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    printf("LT LF@exit GF@$C int@0\n"); // exit = j < 0
    printf("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    printf("GT LF@exit GF@$B GF@$C\n"); // exit = i > j
    printf("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    printf("PUSHS GF@$A\n");
    printf("CALL $length_std\n"); // $RET = length(s)
    printf("LT LF@exit GF@$B GF@$RET\n"); // exit = !(i >= length(s))
    printf("JUMPIFEQ $substring_std_exit_nil LF@exit bool@false\n"); 
    printf("GT LF@exit GF@$C GF@$RET\n"); // exit = j > length(s)
    printf("JUMPIFEQ $substring_std_exit_nil LF@exit bool@true\n");
    printf("GETCHAR GF@$RET GF@$A GF@$B\n"); // $RET = s[i]
    printf("ADD GF@$B GF@$B int@1\n"); // i++
    printf("LABEL $substring_std_loop\n");
    printf("LT LF@exit GF@$B GF@$C\n"); // continue = i < j
    printf("JUMPIFEQ $substring_std_exit LF@exit bool@false\n"); // if continue == false then exit
    printf("GETCHAR GF@$D GF@$A GF@$B\n"); // $D = s[i]
    printf("CONCAT GF@$RET GF@$RET GF@$D\n"); // $RET = $RET + $D
    printf("ADD GF@$B GF@$B int@1\n"); // i++
    printf("JUMP $substring_std_loop\n"); // go to loop
    printf("LABEL $substring_std_exit\n"); // exit loop
    printf("POPFRAME\n"); // pop frame
    printf("RETURN\n"); // return

    printf("LABEL $substring_std_exit_nil\n"); // exit nil
    printf("MOVE GF@$RET nil@nil\n"); // $RET = nil
    printf("POPFRAME\n"); // pop frame
    printf("RETURN\n"); // return

    // ord
    printf("LABEL $ord_std\n");
    printf("POPS GF@$A\n");
    printf("STRI2INT GF@$RET GF@$A\n");
    printf("RETURN\n");
    // chr
    printf("LABEL $chr_std\n");
    printf("POPS GF@$A\n");
    printf("INT2CHAR GF@$RET GF@$A\n");
    printf("RETURN\n");
    printf("LABEL $$main\n");
}

void gen_line(char *line) {
    printf("%s\n", line);
}

void gen_break() {
    printf("JUMP %s\n", current_loop_end_label);
}

void gen_continue() {
    if (current_loop_is_for) {
        printf("ADD %s %s int@1\n", for_counter_reg, for_counter_reg);
    }
    printf("JUMP %s\n", current_loop_start_label);
}

void gen_return(bool void_return) {
    if (!void_return) {
        printf("POPS %s\n", ret_reg);
    }
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void gen_var_decl(char *var_name) {
}

void gen_var_assign(char *var_name) {
}

void gen_branch_labels(bool gen_end) {
    if (gen_end) {
        char *label_end = gen_unique_label("branch_end");
        Stack.push(branch_label_stack, label_end);
    }
    char *label_skip = gen_unique_label("branch_skip");
    Stack.push(branch_label_stack, label_skip);
}

void gen_branch_if_start(bool let) {
    char *label_skip = (char*)Stack.top(branch_label_stack);
    if (let) {
        printf("POPS %s\n", cond_reg);
        printf("TYPE %s %s\n", cond_reg, cond_reg);
        printf("JUMPIFEQ %s %s string@nil\n", label_skip, cond_reg);
    } else {
        printf("PUSHS bool@true\n");
        printf("JUMPIFNEQS %s\n", label_skip);
    }
}

void gen_branch_if_end() {
    char *label_skip = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    char *label_end = (char*)Stack.top(branch_label_stack);
    printf("JUMP %s\n", label_end);
    printf("LABEL %s\n", label_skip);
}

void gen_branch_end() {
    char *label_end = (char*)Stack.top(branch_label_stack);
    Stack.pop(branch_label_stack);
    printf("LABEL %s\n", label_end);
}

void gen_while_start() {
    // todo
    bool *tmp = malloc(sizeof(bool));
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
    printf("LABEL %s\n", current_loop_start_label);
}

void gen_while_cond() {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS %s\n", current_loop_end_label);
}

void gen_while_end() {
    printf("JUMP %s\n", current_loop_start_label);
    printf("LABEL %s\n", current_loop_end_label);
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
    // todo
    bool *tmp = malloc(sizeof(bool));
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
    printf("LABEL %s\n", label_start);
}

void gen_for_range_save() {
    printf("PUSHS %s\n", for_counter_reg);
    printf("PUSHS %s\n", for_max_val);
}

void gen_for_range_restore() {
    printf("POPS %s\n", for_max_val);
    printf("POPS %s\n", for_counter_reg);
}


void gen_for_range(bool open) {
    if (!open) {
        printf("PUSHS int@1\n");
        printf("ADDS\n");
    }
    printf("POPS %s\n", for_max_val);
    printf("POPS %s\n", for_counter_reg);
}

void gen_for_cond() {
    printf("JUMPIFEQ %s %s %s\n", current_loop_end_label, for_counter_reg, for_max_val);
}

void gen_for_end() {
    printf("ADD %s %s int@1\n", for_counter_reg, for_counter_reg);
    printf("JUMP %s\n", current_loop_start_label);
    printf("LABEL %s\n", current_loop_end_label);
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
    printf("LABEL %s\n", func_name);
}

void gen_new_frame() {
    printf("CREATEFRAME\n");
}

void gen_push_frame() {
    printf("PUSHFRAME\n");
}

void gen_pop_frame() {
    printf("POPFRAME\n");
}

void gen_pop_params(string_t *params) {
    char *str = malloc(sizeof(char) * (strlen(params->str) + 1));
    strcpy(str, params->str);

    char *token = strtok(str, "#:");

    int i = 0;

    stack_t *param_stack = Stack.init();

    while (token != NULL) {
        if (i % 3 == 1) {
            char *id = malloc(sizeof(char) * (strlen(token) + 1));\
            strcpy(id, token);
            printf("DEFVAR TF@%s\n", id);
            Stack.push(param_stack, id);
        }
        token = strtok(NULL, "#:");
        i++;
    }

    while (Stack.top(param_stack)) {
        char *id = (char*)Stack.top(param_stack);
        Stack.pop(param_stack);
        printf("POPS TF@%s\n", id);
        free(id);
    }

    free(str);
}
