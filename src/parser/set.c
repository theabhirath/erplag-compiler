#include <stdlib.h>

#include "set.h"

token_set *createTokenSet()
{
    token_set *ts = (token_set *)malloc(sizeof(token_set));
    ts->set = 0LL;
    return ts;
}

void addToken(token_set *ts, TOKEN tok)
{
    ts->set |= (1LL << tok);
}

void removeToken(token_set *ts, TOKEN tok)
{
    ts->set &= ~(1LL << tok);
}

int isMember(token_set *ts, TOKEN tok)
{
    return (ts->set & (1LL << tok)) != 0LL;
}