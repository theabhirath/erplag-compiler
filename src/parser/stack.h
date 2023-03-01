#ifndef STACK_H
#define STACK_H

#include "parserdef.h"

// Stack of nodes
typedef struct stack
{
    hash_table_element symbol;
    struct stack *next;
} stack;

stack *createStack();
stack *push(stack *S, hash_table_element symbol);
hash_table_element top(stack *S);
stack *pop(stack *S);
int isEmpty(stack *S);

#endif