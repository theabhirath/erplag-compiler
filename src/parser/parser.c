#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"
#include "linked_list.h"

#define HASH_TABLE_SIZE 1050

char *nonterminals[NUM_NONTERMINALS];
char *terminals[NUM_TOKENS];
int Table[NUM_NONTERMINALS][NUM_TOKENS];
typedef struct parse_tree_node
{
    TNT tnt;
    struct def *parent;
    struct def *child;
    struct def *sibling;
    int leafNodeFlag;
}parse_tree_node;
typedef struct parse_tree{
    parse_tree_node *root;
}parse_tree;

parse_tree parseTree;
parse_tree_node *rootNode;
void createParseTree(parse_tree parseTree){
    parseTree.root = rootNode;
}

parse_tree_node createParseTreeNode(TNT tnt){
    parse_tree_node node;
    node.tnt = tnt;
    return node;
}

//Stack of nodes
typedef struct stack
{
    hash_table_element symbol; 
    struct stack *next;
} stack;

void push(stack **S, hash_table_element symbol)
{
    stack *temp = (stack *)malloc(sizeof(stack));
    temp->symbol = symbol;
    temp->next = *S;
    *S = temp;
}

hash_table_element top(stack *S)
{
    return S->symbol;
}

void pop(stack **S)
{
    stack *temp = *S;
    *S = (*S)->next;
    free(temp);
}

int isEmpty(stack *S)
{
    if(S == NULL)
        return 1;
    return 0;
}

char *trim(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r')
        {
            str[i] = '\0';
            break;
        }
        i++;
    }
    return str;
}
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
        if (node == NULL)
        {
            addToken(&firstSet[nt], EPSILON);
        }
        while (node != NULL)
        {
            if (node->type == __TERMINAL__)
            {
                addToken(&firstSet[nt], node->tnt.tok);
                break;
            }
            else
            {
                incomplete[nt] += 1;
                any_incomplete = 1;
                unhandled[i] = 1;
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
                    unhandled[i] = 0;
                    incomplete[nt] -= 1;
                    token_set temp = firstSet[node->tnt.nonterm];
                    removeToken(&temp, EPSILON);
                    firstSet[nt].set |= temp.set;
                }
                else
                {
                    any_incomplete = 1;
                }
            }
        }
    }
    int changed = 1;
    while(changed)
    {
        changed = 0;
        for (int i = 0; i < NUM_RULES; i++)
        {
            linked_list_node *node = rules[i].head;
            NONTERMINAL nt = node->tnt.nonterm;
            node = node->next;
            while (node != NULL)
            {
                if(node->type == __TERMINAL__)
                {
                    addToken(&firstSet[nt], node->tnt.tok);
                    break;
                }
                if(isMember(&firstSet[node->tnt.nonterm], EPSILON))
                {
                    token_set temp = firstSet[node->tnt.nonterm];
                    removeToken(&temp, EPSILON);
                    firstSet[nt].set |= temp.set;
                    node = node->next;
                }
                else
                {
                    firstSet[nt].set |= firstSet[node->tnt.nonterm].set;
                    break;
                }
            }
        }
    }


}

void populateParseTable(token_set *firstSet, token_set *followSet, linked_list *rules)
{
    // TODO: Automate population of parse table from grammar and first and follow sets
    for (int i = 0; i < NUM_NONTERMINALS; i++)
    {
        for (int j = 0; j < NUM_TOKENS; j++)
        {
            Table[i][j] = -1;
        }
    }
    
    for (int i = 0; i < NUM_RULES; i++)
    {
        linked_list_node *node = rules[i].head;
        NONTERMINAL nt = node->tnt.nonterm;
        // TNT num_val = node->tnt;
        node = node->next;
        if (node == NULL)
        {
            for (int j = 0; j < NUM_TOKENS; j++)
            {
                if (isMember(followSet[nt].set, j))
                {
                    Table[nt][j] = i;
                }
            }
        }
        else
        {
            if (node->type == __TERMINAL__)
            {
                Table[nt][node->tnt.tok] = i;
            }
            else
            {
                for (int j = 0; j < NUM_TOKENS; j++)
                {
                    if (isMember(firstSet[node->tnt.nonterm].set, j))
                    {
                        Table[nt][j] = i;
                    }
                }
                if (isMember(firstSet[node->tnt.nonterm].set, EPSILON))
                {
                    for (int j = 0; j < NUM_TOKENS; j++)
                    {
                        if (isMember(followSet[nt].set, j))
                        {
                            Table[nt][j] = i;
                        }
                    }
                }
            }
        }
    }
}

void parseInputSourceCode(char *testcaseFile, char *grammarFile)
{
    /*
        Done - Call create_rules() to create a linked list of rules
        Done - Call computeFirstAndFollowSets() to compute first and follow sets
        Done - Call populateParseTable() to populate the parse table
        Parse the input source code as per LECTURE 21/02/2023 while creating the parse tree
    */
    hash_table_element *hashTable = createHashTable("nonterminals.txt", "terminals.txt");
    linked_list *rules = createRuleList(grammarFile, hashTable);
    token_set firstSet[NUM_NONTERMINALS];
    token_set followSet[NUM_NONTERMINALS];
    computeFirstAndFollowSets(firstSet, followSet, rules);
    populateParseTable(firstSet, followSet, rules);
    stack S; //Stack for parsing
    hash_table_element startsym;
    startsym.type = __NONTERMINAL__;
    startsym.tnt.nonterm = __program__;
    push(&S, startsym); //Pushing start table on stack
    createParseTree(parseTree);
    parse_tree_node *currentNode = parseTree.root; //Will help in traversing parse tree.
    FILE *fp = fopen(testcaseFile, "r"); //Source code here.
    tokenInfo L = getNextToken(); //Calling lexer here.
    while(L.tokenID!=NULL){
        hash_table_element X = top(&S); //Get top element of stack
        if(X.type == __TERMINAL__){
            if(X.tnt.tok == L.tokenID){
                //Need to make a leaf node in the parse tree.
                /*TNT l1 = {L.tokenID};
                parse_tree_node leafNode = createParseTreeNode(l1);
                currentNode->child = &leafNode;*/
                //NEED TO HANDLE CURRENT NODE APPROPRIATELY
                currentNode->leafNodeFlag = 1; //Token can only be the leaf node.
                if(currentNode->sibling != NULL){
                    currentNode = currentNode->sibling;
                }
                else{
                    //Need to go up the tree
                    while(currentNode->parent != NULL){
                        currentNode = currentNode->parent;
                        if(currentNode->sibling != NULL){
                            currentNode = currentNode->sibling;
                            break;
                        }
                    }
                }
                //Pop stack to remove top element
                pop(&S);
                L = getNextToken();
            }
            else{
                //Space for error recovery, tokens not matching
            }
        }
        else if (X.type == __NONTERMINAL__){
            //Need to find the rule in the parse table
            int ruleNumber = Table[X.tnt.nonterm][L.tokenID];
            //Need to push the RHS of the rule on the stack
            if(ruleNumber != 0){
                stack Stemp;
                linked_list_node *firstNode = rules[ruleNumber].head;
                firstNode = firstNode->next;
                pop(&S);
                int flagSib = 0;
                while(firstNode != NULL){
                    hash_table_element toBePushed;
                    toBePushed.type = firstNode->type;
                    toBePushed.tnt = firstNode->tnt;
                    push(&Stemp, toBePushed);
                    parse_tree_node *keepTrackNode = currentNode;
                    if (flagSib == 0){
                        //Create left child of currentnode
                        TNT l1 = firstNode->tnt;
                        parse_tree_node newPTNode = createParseTreeNode(l1);
                        currentNode->child = &newPTNode; //Sabse pehli baar child banaya
                        currentNode = currentNode->child; //Current node is now the child
                        flagSib++;
                    }
                    else{
                        //Create sibling of currentnode
                        TNT l1 = firstNode->tnt;
                        parse_tree_node newPTNode = createParseTreeNode(l1);
                        currentNode->sibling = &newPTNode; //Starts filling siblings
                        currentNode = currentNode->sibling;
                    }
                    firstNode = firstNode->next;
                    currentNode = keepTrackNode; //Goes back to where it was
                    currentNode = currentNode->child; //Goes to the leftmost child to now start processing.
                }
                while(!isEmpty(&Stemp)){
                    hash_table_element toBePushed1 = top(&Stemp);
                    pop(&Stemp);
                    push(&S, toBePushed1);
                }
            }
            else{
                //Space for error recovery, no rule found in parse table
            }
        }
        else{
            printf("Error in parsing\n");
            //Space for error recovery, stack end achieved but input not finished
            exit(1);
        }
    }
    if(!isEmpty(&S)){
        //Space for error recovery, stack not empty but input finished
    }
    fclose(fp);
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
            tnt = trim(tnt);
            // printf("%s\n", tnt);
            if(hashTable[hash(tnt)].type == __NONE__)
            {
                printf("None token found in rule %d RHS\n", i+1);
                printf("%s\n", tnt);
                exit(1);
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
            printf("Hash collision\n");
            collisions++;
        }
    }
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        int h = hash(terminals[i]);
        printf("%d %s %d\n", i, terminals[i], h);
        if (hashTable[h].type == __NONE__)
        {
            hashTable[h].type = __TERMINAL__;
            hashTable[h].tnt.tok = i;
        }
        else
        {
            collisions++;
            printf("Hash collision with token %d--- %s. Colliding token: %s\n", i, terminals[i], hashTable[h].type == __TERMINAL__ ? terminals[hashTable[h].tnt.tok] : nonterminals[hashTable[h].tnt.nonterm]);
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
            printf("Colliding term: %s\n", terminals[hashTable[h].tnt.tok]);
        }
        else
        {
            printf("Colliding nonterm: %s\n", nonterminals[hashTable[h].tnt.nonterm]);
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
    if(isMember(set, EPSILON))
    {
        printf("EPSILON");
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
