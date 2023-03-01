#ifndef SET_H
#define SET_H

#include "../lexer/lexerdef.h"

// Here a long long int is used to store the set of tokens.
// The reason for this is that the number of tokens is 57 < 64.
// So, the ith least significant bit of the set will correspond to the ith token of the enum TOKEN.
// For example, if the first entry of the enum TOKEN is INTEGER,
// then the least significant bit of the set will correspond to INTEGER.
// The singleton set containing only INTEGER will be 1.
struct token_set
{
    long long int set;
};
typedef struct token_set token_set;

token_set *createTokenSet(); // returns a pointer to a new token_set initialized to the empty set (0)
void addToken(token_set *ts, TOKEN tok); // adds tok to ts if not already present, else does nothing
void removeToken(token_set *ts, TOKEN tok); // removes tok from ts if present, else does nothing
int isMember(token_set *ts, TOKEN tok); // returns 1 if tok is in ts, 0 otherwise

#endif