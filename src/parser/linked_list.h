#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "parser.h"
enum TNT_TYPE
{
    __TERMINAL__,
    __NONTERMINAL__,
    __NONE__
};
struct linked_list_node
{
    struct linked_list_node *next;
    TNT tnt;
    enum TNT_TYPE type;
};
struct linked_list
{
    struct linked_list_node *head;
    struct linked_list_node *tail;
};

#endif