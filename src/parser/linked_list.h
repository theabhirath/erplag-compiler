#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "parser.h"
struct linked_list_node
{
    struct linked_list_node *next;
    TNT tnt;
};
struct linked_list
{
    struct linked_list_node *head;
    struct linked_list_node *tail;
};

#endif