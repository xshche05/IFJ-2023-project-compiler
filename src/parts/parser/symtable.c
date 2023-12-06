/*
 * IFJ Project 2023
 * Implementation of vv-BVS symtable
 * Author: Kirill Shchetiniuk (xshche05)
 */

#include <string.h>
#include "symtable.h"
#include "parser.h"
#include "macros.h"
#include <stdio.h>
#include "memory.h"
#include "codegen.h"

node_t *currentScope;
node_t *functions;

stack_t *scopeStack;
stack_t *tmpScopeStack;

int scopeDepth;

static void tree_init(node_t **root) {
    // if (root == NULL) return;
    *root = NULL;
}

static void tree_destroy(node_t **root) {
    // if (root == NULL) return;
    if (*root == NULL) return;
    if ((*root)->left != NULL) {
        tree_destroy(&(*root)->left);
    }
    if ((*root)->right != NULL) {
        tree_destroy(&(*root)->right);
    }
    String.dtor((*root)->key);
    safe_free((*root)->data);
    safe_free(*root);
}

static int tree_get_height(node_t **root){
    if ((*root) == NULL)
        return -1;
    else
        return (*root)->height;
}

static int tree_get_balance(node_t **root){
    if ((*root) == NULL)
        return 0;
    return (tree_get_height(&(*root)->right) -  tree_get_height(&((*root)->left)));
}

static void tree_node_swap(node_t **first_node, node_t **second_node){
    string_t *temp_key = (*first_node)->key;
    symTableData_t *temp_data = (*first_node)->data;
    (*first_node)->key = (*second_node)->key;
    (*second_node)->key = temp_key;
    (*first_node)->data = (*second_node)->data;
    (*second_node)->data = temp_data;
}

static void tree_update_height(node_t **root){
    int left_height = tree_get_height(&((*root)->left));
    int right_height = tree_get_height(&((*root)->right));
    (*root)->height = 1 + max(left_height, right_height);
}

static void tree_left_rotate(node_t **root){
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

static void tree_right_rotate(node_t **root){
    tree_node_swap(&(*root), &(*root)->left);
    node_t *temp_node = (*root)->right;
    (*root)->right = (*root)->left;
    (*root)->left = (*root)->right->left;
    (*root)->right->left = (*root)->right->right;
    (*root)->right->right = temp_node;
    tree_update_height(&(*root)->right);
    tree_update_height(&(*root));
}

static bool tree_add(node_t **root, string_t *key, symTableData_t data) {
    // if (root == NULL) return;
    if (*root == NULL) {
        *root = safe_malloc(sizeof(node_t));
        (*root)->key = String.copy(key);
        (*root)->data = safe_malloc(sizeof(symTableData_t));
        (*root)->height = 0;
        (*root)->data->type = data.type;
        (*root)->data->funcData = data.funcData;
        (*root)->left = NULL;
        (*root)->right = NULL;
    } else {
        int cmp = String.cmp(key, (*root)->key);
        if (cmp < 0) {
            return tree_add(&(*root)->left, key, data);
        } else if (cmp > 0) {
            return tree_add(&(*root)->right, key, data);
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

static int tree_find(node_t **root, string_t *key, symTableData_t **data, int depth) {
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
            if (data != NULL)
            {
                *data = (*root)->data;
                return depth;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

void tree_print(node_t **root) {
    // if (root == NULL) return;
    if (*root != NULL) {
        tree_print(&(*root)->left);
        fprintf(stderr, "%s\n", (*root)->key->str);
        tree_print(&(*root)->right);
    }
}

int get_scope() {
    return scopeDepth;
}

static void std_func_init() {
    funcData_t *readString = safe_malloc(sizeof(funcData_t));
    readString->name = String.ctor();
    String.assign_cstr(readString->name, "readString");
    readString->params = String.ctor();
    String.assign_cstr(readString->params, "");
    readString->returnType = nil_string_type;
    add_func(readString);

    funcData_t *readInt = safe_malloc(sizeof(funcData_t));
    readInt->name = String.ctor();
    String.assign_cstr(readInt->name, "readInt");
    readInt->params = String.ctor();
    String.assign_cstr(readInt->params, "");
    readInt->returnType = nil_int_type;
    add_func(readInt);

    funcData_t *readDouble = safe_malloc(sizeof(funcData_t));
    readDouble->name = String.ctor();
    String.assign_cstr(readDouble->name, "readDouble");
    readDouble->params = String.ctor();
    String.assign_cstr(readDouble->params, "");
    readDouble->returnType = nil_double_type;
    add_func(readDouble);

    funcData_t *int2double = safe_malloc(sizeof(funcData_t));
    int2double->name = String.ctor();
    String.assign_cstr(int2double->name, "Int2Double");
    int2double->params = String.ctor();
    String.assign_cstr(int2double->params, "_:i:0");
    int2double->returnType = double_type;
    add_func(int2double);

    funcData_t *double2int = safe_malloc(sizeof(funcData_t));
    double2int->name = String.ctor();
    String.assign_cstr(double2int->name, "Double2Int");
    double2int->params = String.ctor();
    String.assign_cstr(double2int->params, "_:d:1");
    double2int->returnType = int_type;
    add_func(double2int);

    funcData_t *write = safe_malloc(sizeof(funcData_t));
    write->name = String.ctor();
    String.assign_cstr(write->name, "write");
    write->params = String.ctor();
    String.assign_cstr(write->params, "*");
    write->returnType = void_type;
    add_func(write);

    funcData_t *length = safe_malloc(sizeof(funcData_t));
    length->name = String.ctor();
    String.assign_cstr(length->name, "length");
    length->params = String.ctor();
    String.assign_cstr(length->params, "_:s:2");
    length->returnType = int_type;
    add_func(length);

    funcData_t *substr = safe_malloc(sizeof(funcData_t));
    substr->name = String.ctor();
    String.assign_cstr(substr->name, "substring");
    substr->params = String.ctor();
    String.assign_cstr(substr->params, "of:string:2#startingAt:i:0#endingBefore:j:0");
    substr->returnType = nil_string_type;
    add_func(substr);

    funcData_t *ord = safe_malloc(sizeof(funcData_t));
    ord->name = String.ctor();
    String.assign_cstr(ord->name, "ord");
    ord->params = String.ctor();
    String.assign_cstr(ord->params, "_:c:2");
    ord->returnType = int_type;
    add_func(ord);

    funcData_t *chr = safe_malloc(sizeof(funcData_t));
    chr->name = String.ctor();
    String.assign_cstr(chr->name, "chr");
    chr->params = String.ctor();
    String.assign_cstr(chr->params, "_:i:0");
    chr->returnType = string_type;
    add_func(chr);
}

static int symtable_find(string_t *key, symTableData_t **data) {
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

void symtable_init() {
    if (!collect_funcs) {
        tree_init(&currentScope);
        scopeStack = Stack.init();
        tmpScopeStack = Stack.init();
        scopeDepth = 0;
        return;
    }
    tree_init(&currentScope);
    tree_init(&functions);
    scopeStack = Stack.init();
    tmpScopeStack = Stack.init();
    scopeDepth = 0;
    std_func_init();
}

void symtable_destroy() {
    tree_destroy(&currentScope);
    if (!collect_funcs) {
        tree_destroy(&functions);
    }
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

void symtable_print() {
    int depth = scopeDepth;
    if (depth == 0) fprintf(stderr, "GLOBAL SCOPE:\n");
    else fprintf(stderr, "Scope depth: %d\n", depth);
    tree_print(&currentScope);
    while (depth > 0) {
        depth--;
        if (depth == 0) fprintf(stderr, "GLOBAL SCOPE:\n");
        else fprintf(stderr, "Scope depth: %d\n", depth);
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

void increase_scope() {
    Stack.push(scopeStack, currentScope);
    tree_init(&currentScope);
    scopeDepth++;
}

void decrease_scope() {
    currentScope = Stack.top(scopeStack);
    Stack.pop(scopeStack);
    scopeDepth--;
}

bool check_func_param(string_t *param, char *funcName, int param_number) {
    char *givenParam = safe_malloc(sizeof(char) * (param->length + 1));
    strcpy(givenParam, param->str);
    string_t *funcNameStr = String.ctor();
    String.assign_cstr(funcNameStr, funcName);
    funcData_t *funcData = get_func(funcNameStr);
    char *funcParamsStr = safe_malloc(sizeof(char) * (funcData->params->length + 1));
    strcpy(funcParamsStr, funcData->params->str);
    if (strcmp(funcParamsStr, "*") == 0) {
        return true;
    }
    if (String.count(funcData->params, '#') < param_number) {
        fprintf(stderr, "Error: function signature mismatch. Too much params\n");
        safe_exit(SEMANTIC_ERROR_4);
    }
    char *token = strtok(funcParamsStr, "#");
    // get param by param_number
    for (int i = 0; i < param_number; i++) {
        token = strtok(NULL, "#");
    }
    char *requiredParam = safe_malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(requiredParam, token);

    char *token1 = strtok(requiredParam, ":");
    char* alias_required = safe_malloc(sizeof(char) * (strlen(token1) + 1));
    strcpy(alias_required, token1);
    strtok(NULL, ":");
    token1 = strtok(NULL, ":");
    type_t type_required = strtol(token1, NULL, 10);


    char *token2 = strtok(givenParam, ":");
    char* alias_given = safe_malloc(sizeof(char) * (strlen(token2) + 1));
    strcpy(alias_given, token2);
    token2 = strtok(NULL, ":");
    type_t type_given = strtol(token2, NULL, 10);
    token2 = strtok(NULL, ":");
    bool is_literal = strcmp(token2, "1") == 0;

    if (strcmp(alias_required, alias_given) != 0) {
        fprintf(stderr, "Error: function signature mismatch. Param name mismatch\n");
        safe_exit(SEMANTIC_ERROR_4);
    }

    if (!(type_required == type_given || type_required-type_given == nil_int_type - int_type || (type_required > 3 && type_given == nil_type))) {
        if (is_literal) {
            if (type_required == double_type || type_required == nil_double_type) {
                if (type_given == int_type) {
                    gen_line("INT2FLOATS\n");
                    return true;
                }
            }
        }
        fprintf(stderr, "Error: function signature mismatch. Param type mismatch\n");
        safe_exit(SEMANTIC_ERROR_4);
    }

    return true;
}

bool add_func(funcData_t *funcData) {
    symTableData_t data;
    data.type = ndFunc;
    data.funcData = funcData;
    if (data.funcData->params->length == 0) {
        if (tree_find(&currentScope, funcData->name, NULL, 0) != -1) {
            fprintf(stderr, "Error: function name collision with var.\n");
            safe_exit(SEMANTIC_ERROR_9);
        }
    }
    if (collect_funcs) {
        if (data.funcData->params->length > 0) {
            char *paramsStr = safe_malloc(sizeof(char) * (funcData->params->length + 1));
            strcpy(paramsStr, funcData->params->str);
            if (strcmp(paramsStr, "*") != 0) {
                char *token = strtok(paramsStr, "#:");
                while (token != NULL) {
                    char *alias = safe_malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(alias, token);
                    token = strtok(NULL, "#:");
                    char *param = safe_malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(param, token);
                    if (strcmp(alias, param) == 0) {
                        if (!(strcmp(alias, "_") == 0 && strcmp(param, "_") == 0)) {
                            fprintf(stderr, "Error: param name should be diff from its id.\n");
                            safe_exit(SEMANTIC_ERROR_9);
                        }
                    }
                    safe_free(alias);
                    safe_free(param);
                    strtok(NULL, "#:");
                    token = strtok(NULL, "#:");
                }
            }
        }
        return tree_add(&functions, funcData->name, data);
    }
    return true;
}

bool add_var(varData_t *varData) {
    symTableData_t data;
    data.type = ndVar;
    data.varData = varData;
    data.varData->scope = scopeDepth;
    symTableData_t *funcData = safe_malloc(sizeof(symTableData_t));
    if (tree_find(&functions, varData->name, &funcData, 0) != -1) {
        if (funcData->funcData->params->length == 0) {
            fprintf(stderr, "Error: var name collision with function.\n");
            safe_exit(SEMANTIC_ERROR_9);
        }
    }
    bool s = tree_add(&currentScope, varData->name, data);
    if (!s) {
        symTableData_t *data2 = safe_malloc(sizeof(symTableData_t));
        symtable_find(varData->name, &data2);
        bool tmp = data2->varData->canBeRedefined;
        data2->varData->canBeRedefined = false;
        return tmp;
    }
    return s;
}

bool add_let(letData_t *letData) {
    symTableData_t data;
    data.type = ndLet;
    data.letData = letData;
    data.letData->scope = scopeDepth;
    symTableData_t *funcData = safe_malloc(sizeof(symTableData_t));
    if (tree_find(&functions, letData->name, &funcData, 0) != -1) {
        if (funcData->funcData->params->length == 0) {
            fprintf(stderr, "Error: let name collision with function.\n");
            safe_exit(SEMANTIC_ERROR_9);
        }
    }
    bool s = tree_add(&currentScope, letData->name, data);
    if (!s) {
        symTableData_t *data2 = safe_malloc(sizeof(symTableData_t));
        symtable_find(letData->name, &data2);
        bool tmp = data2->letData->canBeRedefined;
        data2->letData->canBeRedefined = false;
        return tmp;
    }
    return s;
}

funcData_t *get_func(string_t *name) {
    symTableData_t *data = safe_malloc(sizeof(symTableData_t));
    int found = tree_find(&functions, name, &data, 0);
    if (found == -1) {
        if (collect_funcs) return NULL;
        fprintf(stderr, "Error: you use undefined func `%s`\n", name->str);
        safe_exit(SEMANTIC_ERROR_3);
    }
    return data->funcData;
}

varData_t *get_var(string_t *name) {
    symTableData_t *data = safe_malloc(sizeof(symTableData_t));
    int found = symtable_find(name, &data);
    if (found == -1) {
        fprintf(stderr, "Error: usage of undefined var\n");
        safe_exit(SEMANTIC_ERROR_5);
    }
    if (data->type != ndVar) {
        return NULL;
    }
    return data->varData;
}

letData_t *get_let(string_t *name) {
    symTableData_t *data = safe_malloc(sizeof(symTableData_t));
    int found = symtable_find(name, &data);
    if (found == -1) {
        fprintf(stderr, "Error: usage of undefined const\n");
        safe_exit(SEMANTIC_ERROR_5);
    }
    if (data->type != ndLet) {
        return NULL;
    }
    return data->letData;
}




