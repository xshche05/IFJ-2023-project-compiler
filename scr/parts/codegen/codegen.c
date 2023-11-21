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
        "GF@$FOR_START",
        "GF@$FOR_END",
};

stack_t *func_label_stack;
stack_t *branch_label_stack;
stack_t *while_label_stack;
stack_t *for_label_stack;

char* current_loop_start_label = NULL;
char* current_loop_end_label = NULL;

char* current_branch_end_label;

bool current_loop_is_for = false;


void init_codegen() {
    func_label_stack = Stack.init();
    branch_label_stack = Stack.init();
    while_label_stack = Stack.init();
    for_label_stack = Stack.init();
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

char *gen_scope_var(char *var_name, int scope, int stayed, bool with_frame) {
    char *str = malloc(sizeof(char) * (strlen(var_name) + 10 + scope));
    char *ret = str;
    if (with_frame) {
        if (scope == 0) {
            sprintf(str, "GF@");
        } else {
            sprintf(str, "LF@");
        }
        str += 3;
    }
    for (int i = 0; i < scope; i++) {
        sprintf(str, "%c", '%');
        str += 1;
    }
    sprintf(str, "%s", var_name);
    str += strlen(var_name);
    if (stayed) {
        sprintf(str, "%c%d", '%', stayed);
        str += strlen(str);
    }

    return ret;
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
    printf(".IFJcode23\n");
    printf("JUMP $$main\n");
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
        printf("ADD GF$@FOR_START GF$@FOR_START int@1\n");
    }
    printf("JUMP %s\n", current_loop_start_label);
}

void gen_return(bool void_return) {
    if (!void_return) {
        printf("POPS GF@$RET\n");
    }
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void gen_var_decl(char *var_name, int scope, int stayed) {
    char *var = gen_scope_var(var_name, scope, stayed, true);
    printf("DEFVAR %s\n", var);
}

void gen_var_assign(char *var_name, int scope, int stayed) {
    char *var = gen_scope_var(var_name, scope, stayed, true);
    printf("POPS %s\n", var);
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
        printf("POPS GF@$COND\n");
        printf("TYPE GF@$COND GF@$COND\n");
        printf("JUMPIFEQ %s GF@$COND string@nil\n", label_skip);
    } else {
        printf("PUSHS bool@true\n");
        printf("JUMPIFNEQ %s\n", label_skip);
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
    char *label_start = gen_unique_label("while_start");
    current_loop_start_label = label_start;
    char *label_end = gen_unique_label("while_end");
    current_loop_end_label = label_end;
    Stack.push(while_label_stack, label_end);
    Stack.push(while_label_stack, label_start);
    printf("LABEL %s\n", label_start);
}

void gen_while_cond() {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQ %s\n", current_loop_end_label);
}

void gen_while_end() {
    char *label_start = (char*)Stack.top(while_label_stack);
    Stack.pop(while_label_stack);
    char *label_end = (char*)Stack.top(while_label_stack);
    Stack.pop(while_label_stack);
    printf("JUMP %s\n", label_start);
    printf("LABEL %s\n", label_end);
}

void gen_for_start() {
    // todo
    current_loop_is_for = true;
    char *label_start = gen_unique_label("for_start");
    current_loop_start_label = label_start;
    char *label_end = gen_unique_label("for_end");
    current_loop_end_label = label_end;
    Stack.push(for_label_stack, label_end);
    Stack.push(for_label_stack, label_start);
    printf("LABEL %s\n", label_start);
}

void gen_for_range_save() {
    printf("PUSHS GF@$FOR_START\n");
    printf("PUSHS GF@$FOR_END\n");
}

void gen_for_range_restore() {
    printf("POPS GF@$FOR_END\n");
    printf("POPS GF@$FOR_START\n");
}


void gen_for_range(bool open) {
    if (!open) {
        printf("PUSHS int@1\n");
        printf("ADDS\n");
    }
    printf("POPS GF@$FOR_END\n");
    printf("POPS GF@$FOR_START\n");
}

void gen_for_cond() {
    printf("JUMPIFEQ %s GF@$FOR_START GF@$FOR_END\n", current_loop_end_label);
}

void gen_for_end() {
    char *label_start = (char*)Stack.top(for_label_stack);
    Stack.pop(for_label_stack);
    char *label_end = (char*)Stack.top(for_label_stack);
    Stack.pop(for_label_stack);
    printf("ADD GF@$FOR_START GF@$FOR_START int@1\n");
    printf("JUMP %s\n", label_start);
    printf("LABEL %s\n", label_end);
}
