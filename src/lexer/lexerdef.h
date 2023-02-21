#include <stdio.h>

union number {
    int intVal;
    float floatVal;
};

typedef struct token{
    void *tokenID; // token_id; will eventually be an enum of all tokens
    char *lexeme; // the actual string that was matched
    number numericValue; // for numbers; union of int and float. 
    /* TODO
    1. handle numbers with E notation
    2. bad naming?
    */
    int lineNumber; // line number of the token
} token;