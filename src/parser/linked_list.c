#include <stdlib.h>

#include "linked_list.h"

linked_list_node *createNode(TNT tnt, TNT_TYPE type)
{
    linked_list_node *node = (linked_list_node *)malloc(sizeof(linked_list_node));
    node->next = NULL;
    node->tnt = tnt;
    node->type = type;
    return node;
}
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
