#include <stdlib.h>

#include "linked_list.h"

// create a new node to be added to the linked list
linked_list_node *createNode(TNT tnt, TNT_TYPE type)
{
    linked_list_node *node = (linked_list_node *)malloc(sizeof(linked_list_node));
    node->next = NULL;
    node->tnt = tnt;
    node->type = type;
    return node;
}

// add a node to the linked list
void addNode(linked_list *list, linked_list_node *node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
}

// free the memory allocated to the linked list
void destroyList(linked_list *list)
{
    linked_list_node *currentNode = list->head;
    linked_list_node *nextNode = NULL;
    while (currentNode != NULL)
    {
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }
    list->head = NULL;
    list->tail = NULL;
}

linked_list *createLinkedListArray(int n)
{
    linked_list *list = (linked_list *)malloc(n * sizeof(linked_list));
    for (int i = 0; i < n; i++)
    {
        list[i].head = NULL;
        list[i].tail = NULL;
    }
    return list;
}