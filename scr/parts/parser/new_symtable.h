//
// Created by Spagetik on 08-Nov-23.
//

#ifndef IFJ_PRJ_NEW_SYMTABLE_H
#define IFJ_PRJ_NEW_SYMTABLE_H

#include <stdbool.h>
#include "utils.h"

typedef enum {
    ndLet,
    ndVar,
    ndFunc,
} NodeDataType_t;

typedef struct  {
    NodeDataType_t type;
    void *data;
} symTableData_t;

typedef struct tBVSNode {
    string_t *key;
    symTableData_t *data;
    struct tBVSNode *left;
    struct tBVSNode *right;
} node_t;

void symtable_init();

void symtable_destroy();

void push_frame();

void pop_frame();

void symtable_add(string_t *key, symTableData_t data);

int symtable_find(string_t *key, symTableData_t **data);

void symtable_print();




#endif //IFJ_PRJ_NEW_SYMTABLE_H
