//
// Created by Spagetik on 08-Nov-23.
//

#include <string.h>
#include "new_symtable.h"

node_t *currentScope;

stack_t *scopeStack;
stack_t *tmpScopeStack;

stack_t *varsToMigrateStack = NULL;
stack_t *placeHolderStack = NULL;

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
    varsToMigrateStack = Stack.init();
    placeHolderStack = Stack.init();
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

int tree_get_height(node_t **root){
    if ((*root) == NULL)
        return -1;
    else
        return (*root)->height;
}

int tree_get_balance(node_t **root){
    if ((*root) == NULL)
        return 0;
    return (tree_get_height(&(*root)->right) -  tree_get_height(&((*root)->left)));
}

void node_swap(node_t **first_node, node_t **second_node){
    string_t *temp_key = (*first_node)->key;
    symTableData_t *temp_data = (*first_node)->data;
    (*first_node)->key = (*second_node)->key;
    (*second_node)->key = temp_key;
    (*first_node)->data = (*second_node)->data;
    (*second_node)->data = temp_data;
}

void tree_update_height(node_t **root){
    int left_height = tree_get_height(&((*root)->left));
    int right_height = tree_get_height(&((*root)->right));
    (*root)->height = 1 + max(left_height, right_height);
}

void tree_left_rotate(node_t **root){
    node_swap(&(*root), &(*root)->right);
    node_t *temp_node = (*root)->left;
    (*root)->left = (*root)->right;
    (*root)->right = (*root)->left->right;
    (*root)->right->left = (*root)->right->right;
    (*root)->left->right = (*root)->left->left;
    (*root)->left->left = temp_node;
    tree_update_height(&(*root)->left);
    tree_update_height(&(*root));
}

void tree_right_rotate(node_t **root){
    node_swap(&(*root), &(*root)->left);
    node_t *temp_node = (*root)->right;
    (*root)->right = (*root)->left;
    (*root)->left = (*root)->right->left;
    (*root)->right->left = (*root)->right->right;
    (*root)->right->right = temp_node;
    tree_update_height(&(*root)->right);
    tree_update_height(&(*root));
}

void tree_add(node_t **root, string_t *key, symTableData_t data) {
    // if (root == NULL) return;
    if (*root == NULL) {
        *root = malloc(sizeof(node_t));
        (*root)->key = String.copy(key);
        (*root)->data = malloc(sizeof(symTableData_t));
        (*root)->height = 0;
        (*root)->data->type = data.type;
        (*root)->data->funcData = data.funcData;
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
    tree_update_height(&(*root));
    int balance = tree_get_balance(&(*root));
    if (balance > 1)
        tree_left_rotate(&(*root));
    else if (balance < -1)
        tree_right_rotate(&(*root));
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

bool check_func_signature(string_t *params, funcData_t *funcData) {
    char *paramsStr = malloc(sizeof(char) * (params->length + 1));
    strcpy(paramsStr, params->str);
    char *token = strtok(paramsStr, "#:");
    stack_t *aliasStack = Stack.init();
    stack_t *typeStack = Stack.init();
    char *elem;
    while (token != NULL) {
        elem = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(elem, token);
        Stack.push(aliasStack, elem);
        token = strtok(NULL, "#:");
        elem = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(elem, token);
        Stack.push(typeStack, elem);
        token = strtok(NULL, "#:");
    }
    char *funcParamsStr = malloc(sizeof(char) * (funcData->params->length + 1));
    strcpy(funcParamsStr, funcData->params->str);
    token = strtok(funcParamsStr, "#:");
    stack_t *funcAliasStack = Stack.init();
    stack_t *funcTypeStack = Stack.init();
    while (token != NULL) {
        elem = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(elem, token);
        Stack.push(funcAliasStack, elem);
        strtok(NULL, "#:");
        token = strtok(NULL, "#:");
        elem = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(elem, token);
        Stack.push(funcTypeStack, elem);
        token = strtok(NULL, "#:");
    }
    free(funcParamsStr);
    free(paramsStr);
    while (Stack.top(aliasStack) && Stack.top(funcAliasStack)) {
        char *alias = Stack.top(aliasStack);
        char *funcAlias = Stack.top(funcAliasStack);
        if (strcmp(alias, funcAlias) != 0) {
            fprintf(stderr, "Error: function signature mismatch.\n");
            exit(99);
        }
        free(alias);
        free(funcAlias);
        Stack.pop(aliasStack);
        Stack.pop(funcAliasStack);
    }
    if (Stack.top(aliasStack) || Stack.top(funcAliasStack)) {
        fprintf(stderr, "Error: wrong number of params\n");
        exit(99);
    }
    while (Stack.top(typeStack) && Stack.top(funcTypeStack)) {
        char *type = Stack.top(typeStack);
        char *funcType = Stack.top(funcTypeStack);
        if (strcmp(type, funcType) != 0) {
            fprintf(stderr, "Error: function signature mismatch.\n");
            exit(99);
        }
        free(type);
        free(funcType);
        Stack.pop(typeStack);
        Stack.pop(funcTypeStack);
    }
    if (Stack.top(typeStack) || Stack.top(funcTypeStack)) {
        fprintf(stderr, "Error: wrong number of params\n");
        exit(99);
    }
    Stack.destroy(aliasStack);
    Stack.destroy(funcAliasStack);
    Stack.destroy(typeStack);
    Stack.destroy(funcTypeStack);
    return true;
}




