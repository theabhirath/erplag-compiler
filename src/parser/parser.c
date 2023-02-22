#include "parser.h"
#include "linked_list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 1050
struct hash_table_element
{
    enum TNT_TYPE type;
    TNT tnt;
};
typedef struct hash_table_element hash_table_element;
void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, linked_list *rules)
{
    // TODO: Automate population of first and follow sets from grammar
    for (int i = 0; i < NUM_NONTERMINALS; i++)
    {
        firstSet[i].set = 0;
        followSet[i].set = 0;
    }
    int incomplete[NUM_NONTERMINALS] = {0};
    int unhandled[NUM_RULES] = {0};
    int any_incomplete = 0;
    for (int i = 0; i < NUM_RULES; i++)
    {
        linked_list_node *node = rules[i].head;
        NONTERMINAL nt = node->tnt.nonterm;
        node = node->next;
        while (node != NULL)
        {
            if (node->type == __TERMINAL__)
            {
                addToken(&firstSet[nt], node->tnt.tok);
                break;
            }
            else
            {
                incomplete[nt] = 1;
                any_incomplete = 1;
                break;
            }
        }
    }
    while (any_incomplete)
    {
        any_incomplete = 0;
        for (int i = 0; i < NUM_RULES; i++)
        {
            linked_list_node *node = rules[i].head;
            NONTERMINAL nt = node->tnt.nonterm;
            node = node->next;
            if (unhandled[i] == 1)
            {
                if (incomplete[node->tnt.nonterm] == 0)
                {
                    incomplete[nt] = 0;
                    unhandled[i] = 0;
                    firstSet[nt].set |= firstSet[node->tnt.nonterm].set;
                }
                else
                {
                    incomplete[nt] = 1;
                    any_incomplete = 1;
                }
            }
        }
    }
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
    token_set firstSet[NUM_NONTERMINALS];
    token_set followSet[NUM_NONTERMINALS];
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
// 1037 - 8
int hash(char *str)
{
    int multiplier = 829;
    int hash = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        hash = (str[i] + hash * multiplier) % HASH_TABLE_SIZE;
    }
    return hash;
}
hash_table_element *createHashTable(char *nontermsfile, char *termsfile)
{
    FILE *fp;
    char line[100];
    fp = fopen(nontermsfile, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    char *nonterminals[NUM_NONTERMINALS];
    int i = 0;
    char delim[] = " \n";
    while (fgets(line, 100, fp) != NULL)
    {
        char *nonterm = strtok(line, delim);
        nonterminals[i] = malloc(sizeof(char) * strlen(nonterm));
        strcpy(nonterminals[i], nonterm);
        i++;
    }
    fclose(fp);

    fp = fopen(termsfile, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    char *terminals[NUM_TOKENS];
    i = 0;
    while (fgets(line, 100, fp) != NULL)
    {
        char *term = strtok(line, delim);
        terminals[i] = malloc(sizeof(char) * strlen(term));
        strcpy(terminals[i], term);
        i++;
    }
    fclose(fp);
    hash_table_element *hashTable = malloc(sizeof(hash_table_element) * HASH_TABLE_SIZE);
    int collisions = 0;

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hashTable[i].type = __NONE__;
    }
    for (int i = 0; i < NUM_NONTERMINALS; i++)
    {
        int h = hash(nonterminals[i]);
        if (hashTable[h].type == __NONE__)
        {
            hashTable[h].type = __NONTERMINAL__;
            hashTable[h].tnt.nonterm = i;
        }
        else
        {
            // printf("Hash collision\n");
            collisions++;
        }
    }
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        int h = hash(terminals[i]);
        if (hashTable[h].type == __NONE__)
        {
            hashTable[h].type = __TERMINAL__;
            hashTable[h].tnt.tok = i;
        }
        else
        {
            collisions++;
        }
    }
    printf("Collisions: %d\n", collisions);
    return hashTable;
}
int main()
{
    hash_table_element *hashTable = createHashTable("nonterminals.txt", "terminals.txt");
}
