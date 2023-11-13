#ifndef IFJ_PRJ_SYMTABLE_H
#define IFJ_PRJ_SYMTABLE_H

#include <stdbool.h>
#include "utils.h"

/*     BINARY SEARCH TREE     */

typedef enum {
    ndtConstant,                    /* nodeDataTypeConstant */
    ndtVariable,                    /* nodeDataTypeVariable */
    ndtFunction,                    /* nodeDataTypeFunction */
} tNodeDataType;

/*   tree node   */

typedef struct  {
    tNodeDataType type;
    void *Data;
}symTableData;

typedef struct  {
    string_t *Key;                  /* key */
    symTableData *Data;             /* node content */
    struct tBVSNode *left;          /* left subtree */
    struct tBVSNode *right;         /* right subtree */
} tBVSNodePtr;

typedef struct {
    int datatype;
} tDataVariable;

typedef struct {
    int datatype;
    bool defined;
    int paramCount;
} tDataFunction;

typedef struct symtable {
    tBVSNodePtr root;
} tSymTable;

/*   FUNCTION PROTOTYPES     */

/** @brief Initializes binary search tree
 *
 * @param rootPtr - pointer to root of binary search tree
 */
void BSTInit(tBVSNodePtr *rootPtr);

/** @brief Inserts new node into binary search tree
 *
 * @param rootPtr - pointer to root of binary search tree
 * @param key - key of new node
 * @param Data - data of new node
 * @param nodeDataType - type of data the node contains
 * @return true if node was inserted, false if node with same key already exists
 */
bool BSTInsert(tBVSNodePtr *rootPtr, char *key, void *Data, tNodeDataType nodeDataType);

/** @brief Finds node with given key in binary search tree
 *
 * @param rootPtr - pointer to root of binary search tree
 * @param key - key of node to be found
 * @return pointer to node with given key, NULL if node with given key doesn't exist
 */
tBVSNodePtr BSTSearch(tBVSNodePtr rootPtr, char *key);

/** @brief Deletes node with given key from binary search tree
 *
 * @param rootPtr - pointer to root of binary search tree
 * @param key - key of node to be deleted
 * @return true if node was deleted, false if node with given key doesn't exist
 */

bool BSTDelete(tBVSNodePtr *rootPtr, char *key);

/** @brief Deletes all nodes from binary search tree
 *
 * @param rootPtr - pointer to root of binary search tree
 */
void BSTDispose(tBVSNodePtr *rootPtr);



/*    FUNCTIONS FOR WORKING WITH THE TABLE    */
//some functions are not implemented yet





#endif //IFJ_PRJ_SYMTABLE_H