//
// Created by Spagetik on 08-Nov-23.
//

#ifndef IFJ_PRJ_NEW_SYMTABLE_H
#define IFJ_PRJ_NEW_SYMTABLE_H

#include <stdbool.h>
#include "utils.h"

extern stack_t *varsToMigrateStack;
extern stack_t *placeHolderStack;

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
    bool isDeclared;
    bool isDefined;
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

void push_frame();

void pop_frame();

void symtable_add(string_t *key, symTableData_t data);

int symtable_find(string_t *key, symTableData_t **data);

void symtable_print();

bool check_func_signature(string_t *params, funcData_t *funcData);

bool add_func(funcData_t *funcData);

bool add_var(varData_t *varData);

bool add_let(letData_t *letData);

funcData_t *get_func(string_t *key);

varData_t *get_var(string_t *key, int *scope);

letData_t *get_let(string_t *key, int *scope);




#endif //IFJ_PRJ_NEW_SYMTABLE_H
