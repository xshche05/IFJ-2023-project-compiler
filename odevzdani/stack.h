/*
 * IFJ Project 2023
 * Implementation of stack abstract data type
 * Author: Kirill Shchetiniuk (xshche05)
 */

#ifndef IFJ_PRJ_STACK_H
#define IFJ_PRJ_STACK_H

#include <stdlib.h>

typedef struct stack_item {
    void *data;
    struct stack_item *down;
} stack_item_t;

typedef struct stack {
    stack_item_t *top;
} stack_t;

typedef struct stack_interface {
    stack_t *(*init)();
    void (*push)(stack_t *stack, void *data);
    void (*pop)(stack_t *stack);
    void *(*top)(stack_t *stack);
    void (*destroy)(stack_t *stack);
} stack_interface_t;

extern stack_interface_t Stack;

#endif //IFJ_PRJ_STACK_H
