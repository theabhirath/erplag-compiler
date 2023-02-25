#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "lexerdef.h"

int reservedWordsTable();
int hashRW(char *word);
int getReservedWordToken(char *word);

FILE *getStream(FILE *fp);
void removeComments(char *test_file, char *clean_file);
tokenInfo getNextToken(FILE *fp);

#endif
