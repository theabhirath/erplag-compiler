#include "parser.h"

void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, linked_list *rules)
{
    // TODO: Automate population of first and follow sets from grammar
    firstSet = (token_set *)malloc(sizeof(token_set) * NUM_NONTERMINALS);
    followSet = (token_set *)malloc(sizeof(token_set) * NUM_NONTERMINALS);
    // INCOMPLETE
}
void populateParseTable(token_set *firstSet, token_set *followSet, linked_list *rules)
{
    // TODO: Automate population of parse table from grammar and first and follow sets
}
void parseInputSourceCode(char *testcaseFile, char *grammarFile)
{
    /*
        Call create_rules() to create a linked list of rules
        Call computeFirstAndFollowSets() to compute first and follow sets
        Call populateParseTable() to populate the parse table
        Parse the input source code as per LECTURE 21/02/2023 while creating the parse tree
    */
}
void printParseTree(parse_tree *tree, FILE *fp)
{
    // TODO: Print the parse tree
}
linked_list *createRuleList(char *grammarFile)
{
    // TODO: Create a linked list of rules from the grammar file
    return NULL;
}
