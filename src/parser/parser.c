#include "parser.h"

struct linked_list_node
{
    struct linked_list_node *next;
    TNT tnt;
};
struct linked_list
{
    struct linked_list_node *head;
    struct linked_list_node *tail;
};
struct token_set
{
    // TODO: Define a set ADT for tokens and add allied functions
};
void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, linked_list *rules)
{
    // TODO: Automate population of first and follow sets from grammar
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
