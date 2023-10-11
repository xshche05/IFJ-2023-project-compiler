//
// Created by Spagetik on 11.10.2023.
//

#ifndef IFJ_PRJ_LISTS_H
#define IFJ_PRJ_LISTS_H

typedef struct element {
    void *data;
    struct element* next;
    struct element* prev;
} element;

typedef struct {
    element* first;
    element* last;
} two_way_list;

void list_init(two_way_list* list);

void insert_last(two_way_list* list, void* data);

void list_dtor(two_way_list* list);



#endif //IFJ_PRJ_LISTS_H
