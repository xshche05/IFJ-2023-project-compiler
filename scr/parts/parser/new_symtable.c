//
// Created by Spagetik on 08-Nov-23.
//

#include "new_symtable.h"

node_t *currentScope;

stack_t *scopeStack;
stack_t *tmpScopeStack;

int scopeDepth;


void tree_init(node_t **root) {
    // if (root == NULL) return;
    *root = NULL;
}

void tree_destroy(node_t **root) {
    // if (root == NULL) return;
    if (*root == NULL) return;
    if ((*root)->left != NULL) {
        tree_destroy(&(*root)->left);
    }
    if ((*root)->right != NULL) {
        tree_destroy(&(*root)->right);
    }
    String.dtor((*root)->key);
    free((*root)->data);
    free(*root);
}

void symtable_init() {
    tree_init(&currentScope);
    scopeStack = Stack.init();
    tmpScopeStack = Stack.init();
    scopeDepth = 0;
}

void symtable_destroy() {
    tree_destroy(&currentScope);
    int depth = scopeDepth;
    while (depth > 0) {
        depth--;
        node_t *tmp = Stack.top(scopeStack);
        tree_destroy(&tmp);
        Stack.push(tmpScopeStack, tmp);
        Stack.pop(scopeStack);
    }
    Stack.destroy(scopeStack);
    Stack.destroy(tmpScopeStack);
}

void push_frame() {
    Stack.push(scopeStack, currentScope);
    tree_init(&currentScope);
    scopeDepth++;
}

void pop_frame() {
    currentScope = Stack.top(scopeStack);
    Stack.pop(scopeStack);
    scopeDepth--;
}

void tree_add(node_t **root, string_t *key, symTableData_t data) {
    // if (root == NULL) return;
    if (*root == NULL) {
        *root = malloc(sizeof(node_t));
        (*root)->key = String.copy(key);
        (*root)->data = malloc(sizeof(symTableData_t));
        (*root)->data->type = data.type;
        (*root)->data->data = data.data;
        (*root)->left = NULL;
        (*root)->right = NULL;
    } else {
        int cmp = String.cmp(key, (*root)->key);
        if (cmp < 0) {
            tree_add(&(*root)->left, key, data);
        } else if (cmp > 0) {
            tree_add(&(*root)->right, key, data);
        } else {
            fprintf(stderr, "Error: symbol already exists.\n");
        }
    }
}

int tree_find(node_t **root, string_t *key, symTableData_t **data, int depth) {
    // if (root == NULL) return -1;
    if (*root == NULL) {
        return -1;
    } else {
        int cmp = String.cmp(key, (*root)->key);
        if (cmp < 0) {
            return tree_find(&(*root)->left, key, data, depth);
        } else if (cmp > 0) {
            return tree_find(&(*root)->right, key, data, depth);
        } else {
            *data = (*root)->data;
            return depth;
        }
    }
}

int symtable_find(string_t *key, symTableData_t **data) {
    int depth = scopeDepth;
    int found = tree_find(&currentScope, key, data, depth);
    while (depth > 0 && found == -1) {
        depth--;
        node_t *tmp = Stack.top(scopeStack);
        found = tree_find(&tmp, key, data, depth);
        Stack.push(tmpScopeStack, tmp);
        Stack.pop(scopeStack);
    }
    while (depth < scopeDepth) {
        Stack.push(scopeStack, Stack.top(tmpScopeStack));
        Stack.pop(tmpScopeStack);
        depth++;
    }
    return found;
}

void symtable_add(string_t *key, symTableData_t data) {
    tree_add(&currentScope, key, data);
}

void tree_print(node_t **root) {
    // if (root == NULL) return;
    if (*root != NULL) {
        tree_print(&(*root)->left);
        printf("%s\n", (*root)->key->str);
        tree_print(&(*root)->right);
    }
}

void symtable_print() {
    int depth = scopeDepth;
    if (depth == 0) printf("GLOBAL SCOPE:\n");
    else printf("Scope depth: %d\n", depth);
    tree_print(&currentScope);
    while (depth > 0) {
        depth--;
        if (depth == 0) printf("GLOBAL SCOPE:\n");
        else printf("Scope depth: %d\n", depth);
        node_t *tmp = Stack.top(scopeStack);
        tree_print(&tmp);
        Stack.push(tmpScopeStack, tmp);
        Stack.pop(scopeStack);
    }
    while (depth < scopeDepth) {
        Stack.push(scopeStack, Stack.top(tmpScopeStack));
        Stack.pop(tmpScopeStack);
        depth++;
    }
}




