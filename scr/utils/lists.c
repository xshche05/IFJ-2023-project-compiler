//
// Created by Spagetik on 11.10.2023.
//

#include "lists.h"
#include <stdlib.h>

void list_init(two_way_list* list){
    list->first = NULL;
    list->last = NULL;
}

void insert_last(two_way_list* list, void* data){
    element* new_element = malloc(sizeof(element));
    new_element->data = data;
    new_element->next = NULL;
    new_element->prev = list->last;
    if(list->last != NULL){
        list->last->next = new_element;
    }
    list->last = new_element;
    if(list->first == NULL){
        list->first = new_element;
    }
}

void list_dtor(two_way_list* list){
    element* tmp = list->first;
    while(tmp != NULL){
        element* tmp2 = tmp->next;
        free(tmp);
        tmp = tmp2;
    }
    list->first = NULL;
    list->last = NULL;
}