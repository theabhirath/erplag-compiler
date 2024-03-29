#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "parserdef.h"

// Linked list node
struct linked_list_node
{
    struct linked_list_node *next;
    TNT tnt;
    enum TNT_TYPE type;
};

// Linked list
struct linked_list
{
    struct linked_list_node *head;
    struct linked_list_node *tail;
};

linked_list_node *createNode(TNT tnt, TNT_TYPE type);
void addNode(linked_list *list, linked_list_node *node);
void destroyList(linked_list *list);
linked_list *createLinkedListArray(int n);

#endif