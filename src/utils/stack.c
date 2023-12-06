/*
 * IFJ Project 2023
 * Implementation of stack ADT
 * Author: Kirill Shchetiniuk (xshche05)
 */
#include "stack.h"
#include "memory.h"


stack_t *stack_init() {
    stack_t *stack = (stack_t *) safe_malloc(sizeof(stack_t));
    stack->top = NULL;
    return stack;
}

void stack_push(stack_t *stack, void *data) {
    stack_item_t *item = (stack_item_t *) safe_malloc(sizeof(stack_item_t));
    item->data = data;
    item->down = stack->top;
    stack->top = item;
}

void stack_pop(stack_t *stack) {
    if (stack->top == NULL) {
        return;
    }
    stack_item_t *item = stack->top;
    stack->top = item->down;
    safe_free(item);
}

void *stack_top(stack_t *stack) {
    if (stack == NULL) {
        return NULL;
    }
    if (stack->top == NULL) {
        return NULL;
    }
    return stack->top->data;
}

void stack_destroy(stack_t *stack) {
    while (stack->top != NULL) {
        stack_pop(stack);
    }
    safe_free(stack);
}

stack_interface_t Stack = {
        .init = stack_init,
        .push = stack_push,
        .pop = stack_pop,
        .top = stack_top,
        .destroy = stack_destroy
};