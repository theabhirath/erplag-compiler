#include "set.h"
token_set *createTokenSet()
{
    token_set *ts = (token_set *)malloc(sizeof(token_set));
    ts->set = 0;
    return ts;
}
void addToken(token_set *ts, TOKEN tok)
{
    ts->set |= (1 << tok);
}
void removeToken(token_set *ts, TOKEN tok)
{
    ts->set &= ~(1 << tok);
}
int isMember(token_set *ts, TOKEN tok)
{
    return (ts->set & (1 << tok)) != 0;
}