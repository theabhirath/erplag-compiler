#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "parserdef.h"
#include "set.h"

void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, linked_list *rules);
void populateParseTable(token_set *firstSet, token_set *followSet, linked_list *rules);
void parseInputSourceCode(char *testcaseFile, char *grammarFile, char *terminalsFile, char *nonterminalsFile);
void printParseTree(FILE *fp);
linked_list *createRuleList(char *grammarFile, hash_table_element *hashTable);
hash_table_element *createHashTable();
void printSet(token_set *set);
int hash(char *str);
void printSubTree(parse_tree_node *currentNode, FILE *fp);

#endif
