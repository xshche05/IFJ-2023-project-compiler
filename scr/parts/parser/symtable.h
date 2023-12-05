//
// Created by Spagetik on 08-Nov-23.
//

#ifndef IFJ_PRJ_SYMTABLE_H
#define IFJ_PRJ_SYMTABLE_H

#include <stdbool.h>
#include "string_util.h"
#include "stack.h"

#define max(x, y) ((x > y) ? x : y)

typedef enum {
    ndLet,
    ndVar,
    ndFunc,
} NodeDataType_t;

typedef enum {
    none_type = -1,
    int_type,
    double_type,
    string_type,
    bool_type,
    nil_int_type,
    nil_double_type,
    nil_string_type,
    nil_bool_type,
    nil_type,
    void_type
} type_t;

typedef struct {
    string_t *name;
    int paramCount;
    string_t *params;
    type_t returnType;
} funcData_t;

typedef struct {
    string_t *name;
    type_t type;
    bool isDefined;
    bool isDeclared;
    bool canBeRedefined;
    int scope;
    int minInitScope;
} varData_t;

typedef varData_t letData_t;

typedef struct  {
    NodeDataType_t type;
    union {
        funcData_t *funcData;
        varData_t *varData;
        letData_t *letData;
    };
} symTableData_t;

typedef struct tBVSNode {
    string_t *key;
    symTableData_t *data;
    struct tBVSNode *left;
    struct tBVSNode *right;
    int height;
} node_t;

void symtable_init();

void symtable_destroy();

void symtable_print();

int get_scope();

bool check_func_param(string_t *param, char *funcName, int param_number);

bool add_func(funcData_t *funcData);

bool add_var(varData_t *varData);

bool add_let(letData_t *letData);

funcData_t *get_func(string_t *key);

varData_t *get_var(string_t *key);

letData_t *get_let(string_t *key);

void new_frame();

void del_frame();

#endif //IFJ_PRJ_SYMTABLE_H
