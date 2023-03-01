#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

stack *createStack()
{
    return NULL;
}

stack *push(stack *S, hash_table_element symbol)
{
    stack *temp = (stack *)malloc(sizeof(stack));
    temp->symbol = symbol;
    temp->next = S;
    S = temp;
    return S;
}

hash_table_element top(stack *S)
{
    return S->symbol;
}

stack *pop(stack *S)
{
    stack *temp = S;
    S = S->next;
    free(temp);
    return S;
}

int isEmpty(stack *S)
{
    if (S == NULL)
        return 1;
    return 0;
}