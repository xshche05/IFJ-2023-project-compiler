//
// Created by Spagetik on 08-Nov-23.
//

#include <string.h>
#include "new_symtable.h"

node_t *currentScope;
node_t *functions;

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
    tree_init(&functions);
    scopeStack = Stack.init();
    tmpScopeStack = Stack.init();
    varsToMigrateStack = Stack.init();
    placeHolderStack = Stack.init();
    scopeDepth = 0;
}

void symtable_destroy() {
    tree_destroy(&currentScope);
    tree_destroy(&functions);
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

void tree_node_swap(node_t **first_node, node_t **second_node){
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
    tree_node_swap(&(*root), &(*root)->right);
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
    tree_node_swap(&(*root), &(*root)->left);
    node_t *temp_node = (*root)->right;
    (*root)->right = (*root)->left;
    (*root)->left = (*root)->right->left;
    (*root)->right->left = (*root)->right->right;
    (*root)->right->right = temp_node;
    tree_update_height(&(*root)->right);
    tree_update_height(&(*root));
}

bool tree_add(node_t **root, string_t *key, symTableData_t data) {
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
            return false;
        }
    }
    tree_update_height(&(*root));
    int balance = tree_get_balance(&(*root));
    if (balance > 1) {
        if (tree_get_height(&(*root)->right->left) > tree_get_height(&(*root)->right->right)) {
            tree_right_rotate(&(*root)->right);
        }
        tree_left_rotate(&(*root));
    } else if (balance < -1) {
        if (tree_get_height(&(*root)->left->right) > tree_get_height(&(*root)->left->left)) {
            tree_left_rotate(&(*root)->left);
        }
        tree_right_rotate(&(*root));
    }
    return true;
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
            if (data != NULL) *data = (*root)->data;
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

bool add_func(funcData_t *funcData) {
    symTableData_t data;
    data.type = ndFunc;
    data.funcData = funcData;
    if (data.funcData->params->length == 0) {
        if (tree_find(&currentScope, funcData->name, NULL, 0) != -1) {
            fprintf(stderr, "Error: function name collision with var.\n");
            exit(9); // TODO error code
        }
    }
    return tree_add(&functions, funcData->name, data);
}

bool add_var(varData_t *varData) {
    symTableData_t data;
    data.type = ndVar;
    data.varData = varData;
    symTableData_t *funcData = malloc(sizeof(symTableData_t));
    if (tree_find(&functions, varData->name, &funcData, 0) != -1) {
        if (funcData->funcData->params->length == 0) {
            fprintf(stderr, "Error: var name collision with function.\n");
            exit(9); // TODO error code
        }
    }
    free(funcData);
    return tree_add(&currentScope, varData->name, data);
}

bool add_let(letData_t *letData) {
    symTableData_t data;
    data.type = ndLet;
    data.letData = letData;
    symTableData_t *funcData = malloc(sizeof(symTableData_t));
    if (tree_find(&functions, letData->name, &funcData, 0) != -1) {
        if (funcData->funcData->params->length == 0) {
            fprintf(stderr, "Error: let name collision with function.\n");
            exit(9); // TODO error code
        }
    }
    free(funcData);
    return tree_add(&currentScope, letData->name, data);
}

funcData_t *get_func(string_t *name) {
    symTableData_t *data = malloc(sizeof(symTableData_t));
    int found = tree_find(&functions, name, &data, 0);
    if (found == -1) {
        fprintf(stderr, "Error: you use undeclared func\n");
        exit(9); // TODO error code
    }
    return data->funcData;
}

varData_t *get_var(string_t *name, int *scope) {
    symTableData_t *data = malloc(sizeof(symTableData_t));
    int found = symtable_find(name, &data);
    if (found == -1) {
        fprintf(stderr, "NOTFOUND VAR\n");
        exit(99); // TODO error code
    }
    if (data->type != ndVar) {
        return NULL;
    }
    if (scope != NULL) *scope = found;
    return data->varData;
}

letData_t *get_let(string_t *name, int *scope) {
    symTableData_t *data = malloc(sizeof(symTableData_t));
    int found = symtable_find(name, &data);
    if (found == -1) {
        fprintf(stderr, "NOTFOUND LET\n");
        exit(99); // TODO error code
    }
    if (data->type != ndLet) {
        return NULL;
    }
    if (scope != NULL) *scope = found;
    return data->letData;
}




