#include "parser.h"
#include "linked_list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 1050

char *nonterminals[NUM_NONTERMINALS];
char *terminals[NUM_TOKENS];

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

linked_list *createRuleList(char *grammarFile, hash_table_element *hashTable)
{
    FILE *fp;
    char line[100];
    fp = fopen(grammarFile, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    linked_list *rules = malloc(sizeof(linked_list) * NUM_RULES);
    int i = 0;
    char delim[] = ",\n";
    while (fgets(line, 100, fp) != NULL)
    {
        char *tnt = strtok(line, delim);
        if(hashTable[hash(tnt)].type != __NONTERMINAL__)
        {
            printf("Error in line %d\n", i+1);
            printf("Expected nonterminal, found %s\n", tnt);
        }
        linked_list_node *lhs_nt = createNode(hashTable[hash(tnt)].tnt, __NONTERMINAL__);
        addNode(&rules[i], lhs_nt);
        tnt = strtok(NULL, delim);
        while(tnt != NULL)
        {
            // printf("%s\n", tnt);
            if(hashTable[hash(tnt)].type == __NONE__)
            {
                printf("None token found in rule %d RHS\n", i+1);
            }
            else if(hashTable[hash(tnt)].type == __EPSILON__)
            {
                break;
            }
            else
            {
                linked_list_node *rhs_nt = createNode(hashTable[hash(tnt)].tnt, hashTable[hash(tnt)].type);
                addNode(&rules[i], rhs_nt);
            }
            tnt = strtok(NULL, delim);
        }
        i++;
    }
    return rules;
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
    i = 0;
    while (fgets(line, 100, fp) != NULL)
    {
        char *term = strtok(line, delim);
        terminals[i] = malloc(sizeof(char) * strlen(term));
        strcpy(terminals[i], term);
        i++;
    }
    fclose(fp);
    char epsilon[] = "EPSILON";
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
            printf("Hash collision with token %d: %s\n", i, terminals[i]);
        }
    }
    int h = hash(epsilon);
    if (hashTable[h].type == __NONE__)
    {
        hashTable[h].type = __EPSILON__;
    }
    else
    {
        collisions++;
        printf("Hash collision\n");
        // Print term colliding with epsilon
        if(hashTable[h].type == __TERMINAL__)
        {
            printf("Colliding term: %d\n", terminals[hashTable[h].tnt.tok]);
        }
        else
        {
            printf("Colliding nonterm: %d\n", nonterminals[hashTable[h].tnt.nonterm]);
        }
    }
    printf("Collisions: %d\n", collisions);
    return hashTable;
}
void print_rules(linked_list *rules)
{
    for (int i = 0; i < NUM_RULES; i++)
    {
        linked_list_node *temp = rules[i].head;
        printf("%d: %s --> ", i, nonterminals[temp->tnt.nonterm]);
        temp = temp->next;
        if (temp == NULL)
        {
            printf("EPSILON");
        }
        while (temp != NULL)
        {
            if (temp->type == __NONTERMINAL__)
            {
                printf("%s ", nonterminals[temp->tnt.nonterm]);
            }
            else if (temp->type == __TERMINAL__)
            {
                printf("%s ", terminals[temp->tnt.tok]);
            }
            else
            {
                printf("EPSILON ");
            }
            temp = temp->next;
        }
        printf("\n");
    }
}
void printSet(token_set *set)
{
    long long cur = set->set;
    if(cur == 0)
    {
        printf("EMPTY SET\n");
        return;
    }
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        if(cur & 1)
        {
            printf("%s ", terminals[i]);
        }
        cur = cur >> 1;
    }
}
int main()
{
    hash_table_element *hashTable = createHashTable("nonterminals.txt", "terminals.txt");

    linked_list *rules = createRuleList("grammar.csv", hashTable);
    print_rules(rules);
    token_set *first_sets = malloc(sizeof(token_set) * NUM_NONTERMINALS);
    token_set *follow_sets = malloc(sizeof(token_set) * NUM_NONTERMINALS);
    computeFirstAndFollowSets(first_sets, follow_sets, rules);
    for (int i = 0; i < NUM_NONTERMINALS; i++)
    {
        printf("First(%s): ", nonterminals[i]);
        printSet(&first_sets[i]);
        printf("\n");
    }
    return 0;
}
