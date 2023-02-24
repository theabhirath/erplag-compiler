#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

FILE *getStream(FILE *fp, char *buf1, char *buf2);
void removeComments(char *test_file, char *clean_file);
tokenInfo getNextToken();

#endif
