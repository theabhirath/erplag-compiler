#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"
#include "parserdef.h"
#include "stack.h"
#include "linked_list.h"

#define HASH_TABLE_SIZE 1050

char *nonterminals[NUM_NONTERMINALS] = {
    "<program>",
    "<moduleDeclarations>",
    "<moduleDeclaration>",
    "<otherModules>",
    "<driverModule>",
    "<module>",
    "<ret>",
    "<input_plist>",
    "<iP0>",
    "<output_plist>",
    "<oP0>",
    "<dataType>",
    "<type>",
    "<moduleDef>",
    "<statements>",
    "<statement>",
    "<ioStmt>",
    "<var3>",
    "<var>",
    "<whichId>",
    "<wI0>",
    "<simpleStmt>",
    "<assignmentStmt>",
    "<whichStmt>",
    "<lvalueIDStmt>",
    "<lvalueARRStmt>",
    "<element_index_with_expression>",
    "<sign>",
    "<N_10>",
    "<arrExpr>",
    "<arr_N4>",
    "<arrTerm>",
    "<arr_N5>",
    "<arrFactor>",
    "<id_num>",
    "<moduleReuseStmt>",
    "<optional>",
    "<idList>",
    "<iL0>",
    "<expression>",
    "<booleanExpr>",
    "<BE00>",
    "<BE0>",
    "<arithmeticExpr>",
    "<AE0>",
    "<term>",
    "<T0>",
    "<op1>",
    "<op2>",
    "<bools>",
    "<factor>",
    "<N_11>",
    "<logicalOp>",
    "<relationalOp>",
    "<declareStmt>",
    "<conditionalStmt>",
    "<caseStmts>",
    "<caseStmt>",
    "<value>",
    "<default>",
    "<iterativeStmt>",
    "<forrange>",
    "<range>",
    "<var2>",
    "<range_arr>",
    "<index_arr>",
    "<new_index>",
    "<actual_para_list>",
    "<aP0>",
    "<aP00>",
    "<aP1>"};

char *terminals[NUM_TOKENS] = {
    "INTEGER",
    "REAL",
    "BOOLEAN",
    "OF",
    "ARRAY",
    "START",
    "END",
    "DECLARE",
    "MODULE",
    "DRIVER",
    "PROGRAM",
    "GET_VALUE",
    "PRINT",
    "USE",
    "WITH",
    "PARAMETERS",
    "TAKES",
    "INPUT",
    "RETURNS",
    "FOR",
    "IN",
    "SWITCH",
    "CASE",
    "BREAK",
    "DEFAULT",
    "WHILE",
    "AND",
    "OR",
    "TRUE",
    "FALSE",
    "DRIVERDEF",
    "DEF",
    "ID",
    "COMMA",
    "NUM",
    "RNUM",
    "SQBO",
    "ASSIGNOP",
    "PLUS",
    "MINUS",
    "BO",
    "MUL",
    "EQ",
    "COLON",
    "SQBC",
    "DIV",
    "NE",
    "RANGEOP",
    "SEMICOL",
    "BC",
    "LT",
    "DRIVERENDDEF",
    "ENDEF",
    "GT",
    "LE",
    "GE",
    "PROGRAMEND"};

// parse table
int Table[NUM_NONTERMINALS][NUM_TOKENS];

// parse tree
parse_tree parseTree;

void createParseTree(parse_tree parseTree, parse_tree_node *rootNode)
{
    parseTree.root = rootNode;
}

void createParseTreeNode(TNT tnt, parse_tree_node *node)
{
    node->tnt = tnt;
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

void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, grammar rules)
{
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
        if (rules[i].head == NULL)
        {
            printf("Error: Rule %d is empty in Grammar\n", i);
            exit(1);
        }
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
                    if (isMember(&firstSet[node->tnt.nonterm], EPSILON))
                    {
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
                                if (incomplete[node->tnt.nonterm] == 0)
                                {
                                    token_set temp = firstSet[node->tnt.nonterm];
                                    removeToken(&temp, EPSILON);
                                    firstSet[nt].set |= temp.set;
                                    if (!isMember(&firstSet[node->tnt.nonterm], EPSILON))
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    incomplete[nt] += 1;
                                    any_incomplete = 1;
                                    unhandled[i] = 1;
                                    break;
                                }
                            }
                            node = node->next;
                        }
                        if (node == NULL)
                        {
                            addToken(&firstSet[nt], EPSILON);
                        }
                    }
                }
                else
                {
                    any_incomplete = 1;
                }
            }
        }
    }
    int changed = 1;
    while (changed)
    {
        changed = 0;
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
                if (isMember(&firstSet[node->tnt.nonterm], EPSILON))
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
    // printf("First sets computed\n");
    changed = 1;

    // Follow set computation
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < NUM_RULES; i++)
        {
            linked_list_node *node = rules[i].head;
            NONTERMINAL lhs_nt = node->tnt.nonterm;
            node = node->next;
            while (node != NULL)
            {
                if (node->type == __TERMINAL__)
                {
                    node = node->next;
                    continue;
                }
                else if (node->type == __NONTERMINAL__)
                {
                    // If this is the last node in the rule, add follow set of lhs to follow set of current nonterminal
                    if (node->next == NULL)
                    {
                        token_set temp = followSet[node->tnt.nonterm];
                        temp.set |= followSet[lhs_nt].set;
                        if (temp.set != followSet[node->tnt.nonterm].set)
                        {
                            followSet[node->tnt.nonterm].set = temp.set;
                            // printf("Change loc 1\n");
                            // printf("Rule No: %d, Nonterminal: %s, Added token of lhs: %s\n", i, nonterminals[node->tnt.nonterm], nonterminals[lhs_nt]);
                            changed = 1;
                        }
                    }
                    // If this node is followed by a terminal, add that terminal to the follow set of current nonterminal
                    else if (node->next->type == __TERMINAL__)
                    {
                        if (!isMember(&followSet[node->tnt.nonterm], node->next->tnt.tok))
                        {
                            addToken(&followSet[node->tnt.nonterm], node->next->tnt.tok);
                            // printf("Change loc 2\n");
                            // printf("Rule No: %d, Nonterminal: %s, Added token: %s\n", i, nonterminals[node->tnt.nonterm], terminals[node->next->tnt.tok]);
                            changed = 1;
                        }
                    }
                    // Handle case where this node is followed by a nonterminal
                    else if (node->next->type == __NONTERMINAL__)
                    {
                        // If first set of following non-terminal contains epsilon, add first set - epsilon to the follow set of current nonterminal and repeat for the rest of the rule
                        linked_list_node *epsilon_traverser = node->next;
                        while (epsilon_traverser != NULL && epsilon_traverser->type == __NONTERMINAL__ && isMember(&firstSet[epsilon_traverser->tnt.nonterm], EPSILON))
                        {
                            token_set temp = firstSet[epsilon_traverser->tnt.nonterm];
                            removeToken(&temp, EPSILON);
                            temp.set |= followSet[node->tnt.nonterm].set;
                            if (temp.set != followSet[node->tnt.nonterm].set)
                            {
                                followSet[node->tnt.nonterm].set = temp.set;
                                // printf("Change loc 3\n");
                                // printf("Rule Number: %d, Nonterminal: %s, Added first set of %s\n", i, nonterminals[node->tnt.nonterm], nonterminals[epsilon_traverser->tnt.nonterm]);
                                changed = 1;
                            }
                            epsilon_traverser = epsilon_traverser->next;
                        }

                        if (epsilon_traverser != NULL && epsilon_traverser->type == __NONTERMINAL__)
                        {
                            token_set temp = firstSet[epsilon_traverser->tnt.nonterm];
                            temp.set |= followSet[node->tnt.nonterm].set;
                            if (temp.set != followSet[node->tnt.nonterm].set)
                            {
                                followSet[node->tnt.nonterm].set = temp.set;
                                // printf("Change loc 4\n");
                                // printf("Rule Number: %d, Nonterminal: %s, Added follow set of %s\n", i, nonterminals[node->tnt.nonterm], nonterminals[epsilon_traverser->tnt.nonterm]);
                                changed = 1;
                            }
                        }
                        else if (epsilon_traverser != NULL && epsilon_traverser->type == __TERMINAL__)
                        {
                            if (!isMember(&followSet[node->tnt.nonterm], epsilon_traverser->tnt.tok))
                            {
                                addToken(&followSet[node->tnt.nonterm], epsilon_traverser->tnt.tok);
                                // printf("Change loc 5\n");
                                // printf("Rule Number: %d, Nonterminal: %s, Added token: %s\n", i, nonterminals[node->tnt.nonterm], terminals[epsilon_traverser->tnt.tok]);
                                changed = 1;
                            }
                        }
                        else
                        {
                            token_set temp = followSet[lhs_nt];
                            temp.set |= followSet[node->tnt.nonterm].set;
                            if (temp.set != followSet[node->tnt.nonterm].set)
                            {
                                followSet[node->tnt.nonterm].set = temp.set;
                                // printf("Change loc 6\n");
                                // printf("Rule Number: %d, Nonterminal: %s, Added follow set of lhs: %s\n", i, nonterminals[node->tnt.nonterm], nonterminals[lhs_nt]);
                                changed = 1;
                            }
                        }
                    }
                    node = node->next;
                }
            }
        }
    }
}

void populateParseTable(token_set *firstSet, token_set *followSet, grammar rules)
{
    // Automate population of parse table from grammar and first and follow sets
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
                if (isMember(&followSet[nt], j))
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
                    if (isMember(&firstSet[node->tnt.nonterm], j))
                    {
                        Table[nt][j] = i;
                    }
                }
                if (isMember(&firstSet[node->tnt.nonterm], EPSILON))
                {
                    for (int j = 0; j < NUM_TOKENS; j++)
                    {
                        if (isMember(&followSet[node->tnt.nonterm], j))
                        {
                            Table[nt][j] = i;
                        }
                    }
                }
            }
        }
    }
}

void printParseTable()
{
    printf("Parse Table:\n");
    for (int i = 0; i < NUM_NONTERMINALS; i++)
    {
        for (int j = 0; j < NUM_TOKENS; j++)
        {
            printf("%d ", Table[i][j]);
        }
        printf("\n");
    }
}

void parseInputSourceCode(char *testcaseFile, char *parseTreeFile)
{
    /*
        Done - Call create_rules() to create a linked list of rules
        Done - Call computeFirstAndFollowSets() to compute first and follow sets
        Done - Call populateParseTable() to populate the parse table
        Parse the input source code as per LECTURE 21/02/2023 while creating the parse tree
    */
    hash_table_element *hashTable = createHashTable();
    // printf("Hash Table Populated\n");
    grammar rules = createRuleList("src/parser/grammar.csv", hashTable);
    // printf("Rule List Populated\n");
    token_set firstSet[NUM_NONTERMINALS];
    token_set followSet[NUM_NONTERMINALS];
    computeFirstAndFollowSets(firstSet, followSet, rules);
    // printf("First and Follow Sets Computed\n");
    populateParseTable(firstSet, followSet, rules);
    stack *S = createStack(); // Stack for parsing
    hash_table_element startsym;
    startsym.type = __NONTERMINAL__;
    startsym.tnt.nonterm = __program__;
    S = push(S, startsym);
    parse_tree_node *rootNode = (parse_tree_node *)malloc(sizeof(parse_tree_node));
    if (rootNode == NULL)
    {
        printf("Memory Error\n");
        exit(1);
    }
    rootNode->tnt.nonterm = __program__;
    rootNode->child = NULL;
    rootNode->parent = NULL;
    rootNode->sibling = NULL;
    rootNode->leafNodeFlag = 0;
    parseTree.root = rootNode;
    parse_tree_node *currentNode = parseTree.root;
    FILE *fp = fopen(testcaseFile, "r"); // Source code here.
    initialiseTwinBuffers();
    reservedWordsTable();
    tokenInfo L = getNextToken(fp); // Calling lexer here.
    // printf("Read first token: %s\n", terminals[L.tokenID]);
    int count = 0;
    // Print non -1 entries of row program of Table
    // printf("Non -1 entries of row program of Table: \n");
    /*for (int i = 0; i < NUM_TOKENS; i++)
    {
        if (Table[0][i] != -1)
        {
            printf("Token: %s, Rule Number: %d\n", terminals[i], Table[0][i]);
        }
    }*/
    while (isEmpty(S) == 0)
    {
        // printf("Iteration Number: %d and Line number : %d\n", count++, L.lineNumber);
        // printf("Token: %s\n", terminals[L.tokenID]);
        // printf("Stack Top: %s\n", top(S).type == __TERMINAL__ ? terminals[top(S).tnt.tok] : nonterminals[top(S).tnt.nonterm]);
        /*if (L.tokenID == ID)
        {
            printf("Lexeme: %s\n", L.val.lexValue);
        }*/

        hash_table_element X = top(S); // Get top element of stack
        // printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
        if (X.type == __TERMINAL__)
        {
            // printf("Terminal\n");
            //  Check if X matches Current Token
            // printf("X: %s, CurrentNode: %s\n", terminals[X.tnt.tok], terminals[currentNode->tnt.tok]);
            if (X.tnt.tok == L.tokenID)
            {
                // printf("Matched\n");
                // printf("Current Node: %s\n", terminals[currentNode->tnt.tok]);
                currentNode->leafNodeFlag = 1; // Token can only be the leaf node.
                // printf("Leaf Node Flag set\n");
                currentNode->tnt.tok = L.tokenID;
                // printf("Token set\n");
                currentNode->leafNodeInfo = L;
                // printf("Flags set\n");
                if (currentNode->sibling != NULL)
                {
                    currentNode = currentNode->sibling;
                }
                else
                {
                    // Need to go up the tree
                    while (currentNode->parent != NULL)
                    {
                        currentNode = currentNode->parent;
                        // printf("While recursing up the tree, current node: %s\n", nonterminals[currentNode->tnt.nonterm]);
                        if (currentNode->sibling != NULL)
                        {
                            currentNode = currentNode->sibling;
                            break;
                        }
                    }
                }
                // Pop stack to remove top element
                S = pop(S);
                // printf("Popped\n");
                L = getNextToken(fp);
                // printf("got next token\n");
                // TODO: Call function to print error - Premature end of input
                // if (L.tokenID == PROGRAMEND && X.tnt.tok != PROGRAMEND){
                //     printf("Idhar se aa raha hai\n");
                //     prematureEndOfInputError();
                // }
            }
            else
            {
                // Print error
                printf("Line: %d. Error: Expected %s, but got %s instead\n", L.lineNumber, terminals[X.tnt.tok], terminals[L.tokenID]);
                token_set *syncSet = createTokenSet();
                // Add SQBC to syncSet using addToken function
                // addToken(syncSet, SEMICOL);
                addToken(syncSet, END);
                if (isMember(&followSet[X.tnt.tok], EPSILON))
                {
                    addToken(syncSet, SEMICOL);
                    addToken(syncSet, BC);
                    addToken(syncSet, START);
                    addToken(syncSet, SQBC);
                }
                // addToken(syncSet, SEMICOL);
                // addToken(syncSet, BC);
                // addToken(syncSet, START);
                //  Fill syncSet with all the tokens in followSet of the nonterminal on top of stack
                for (int i = 0; i < NUM_TOKENS; i++)
                {
                    if (isMember(&followSet[X.tnt.tok], i) && i != EPSILON)
                    {
                        addToken(syncSet, i);
                    }
                }
                // printf("Sync Set created\n");
                while (isMember(syncSet, L.tokenID) == 0)
                {
                    L = getNextToken(fp);
                    // TODO: Call function to print error - Premature end of input
                    if (L.tokenID == PROGRAMEND && X.tnt.tok != PROGRAMEND)
                    {
                        prematureEndOfInputError();
                        return;
                    }
                }
                // printf("Found token in syncSet %s\n", terminals[L.tokenID]);
                // print X
                // printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
                if (isMember(&followSet[X.tnt.tok], L.tokenID))
                {
                    // printf("Found token in followSet of X\n");
                    S = pop(S);
                    X = top(S);
                    continue;
                }
                // Pop stack till we either find L if X is a terminal or L is in first set of X if it is a nonterminal
                /*while (((X.type == __TERMINAL__ && X.tnt.tok != L.tokenID) || (X.type == __NONTERMINAL__ && isMember(&followSet[X.tnt.nonterm], L.tokenID) == 1)) && isEmpty(S) == 0)
                {
                    S = pop(S);
                    X = top(S);
                    printf("Popped\n");
                }*/
                while (((X.type == __NONTERMINAL__ && !isMember(&followSet[X.tnt.nonterm], L.tokenID)) || (X.type == __TERMINAL__ && !isMember(&followSet[X.tnt.tok], L.tokenID))) && isEmpty(S) == 0)
                {
                    S = pop(S);
                    X = top(S);
                    // printf("Popped\n");
                }
                while (((X.type == __TERMINAL__ && X.tnt.tok != L.tokenID) || (X.type == __NONTERMINAL__ && isMember(&firstSet[X.tnt.nonterm], L.tokenID) == 0)) && isEmpty(S) == 0)
                {
                    S = pop(S);
                    X = top(S);
                    // printf("Popped\n");
                }
                // printf("Found L %s in firstSet of X\n", terminals[L.tokenID]);
                // printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
                // In the parse tree, we traverse siblings recursively till we find whatever X is
                while (((X.type == __TERMINAL__ && terminals[X.tnt.tok] != terminals[currentNode->tnt.tok]) || (X.type == __NONTERMINAL__ && nonterminals[X.tnt.nonterm] != nonterminals[currentNode->tnt.nonterm])) && currentNode->parent != NULL)
                {
                    // printf("Current Node: %s %s  LeafNode: %d\n", terminals[currentNode->tnt.tok], nonterminals[currentNode->tnt.nonterm], currentNode->leafNodeFlag);
                    if (currentNode->sibling == NULL)
                    {
                        currentNode = currentNode->parent;
                    }
                    else
                    {
                        currentNode = currentNode->sibling;
                    }
                }
                /*while(1){
                    // Print Current Node
                    printf("Current Node: %s %s  LeafNode: %d\n", terminals[currentNode->tnt.tok], nonterminals[currentNode->tnt.nonterm], currentNode->leafNodeFlag);
                    if(currentNode->sibling != NULL){
                        currentNode = currentNode->sibling;
                    }
                    else{
                        currentNode = currentNode->parent;
                    }
                    if((currentNode->leafNodeFlag == 1 && currentNode->leafNodeInfo.tokenID == L.tokenID) || isMember(&firstSet[currentNode->tnt.nonterm], L.tokenID)){
                        break;
                    }
                }*/
            }
        }
        else if (X.type == __NONTERMINAL__)
        {
            // Need to find the rule in the parse table
            int ruleNumber = Table[X.tnt.nonterm][L.tokenID];
            // printf("Rule Number: %d\n", ruleNumber);
            // Need to push the RHS of the rule on the stack
            if (ruleNumber != -1)
            {
                stack *Stemp = createStack();
                linked_list_node *firstNode = rules[ruleNumber].head;
                currentNode->ruleNumber = ruleNumber;
                firstNode = firstNode->next; // Skipping the LHS
                S = pop(S);                  // Popping the nonterm from the stack
                int flagSib = 0;             // First child or sibling
                parse_tree_node *keepTrackNode = currentNode;
                if (firstNode == NULL) // Epsilon rule
                {
                    parse_tree_node *newPTNode = (parse_tree_node *)malloc(sizeof(parse_tree_node));
                    newPTNode->tnt.tok = EPSILON;
                    newPTNode->child = NULL;
                    newPTNode->parent = keepTrackNode;
                    newPTNode->sibling = NULL;
                    newPTNode->leafNodeFlag = 0;
                    // newPTNode->leafNodeInfo;
                    currentNode->child = newPTNode;
                    currentNode = currentNode->child;
                }
                while (firstNode != NULL)
                {
                    // printf("Inside while loop\n");
                    hash_table_element toBePushed;
                    toBePushed.type = firstNode->type;
                    toBePushed.tnt = firstNode->tnt;
                    Stemp = push(Stemp, toBePushed);
                    if (flagSib == 0)
                    {
                        // Create left child of currentnode
                        TNT l1 = firstNode->tnt;
                        parse_tree_node *newPTNode = (parse_tree_node *)malloc(sizeof(parse_tree_node));
                        createParseTreeNode(l1, newPTNode);
                        // printf("New PT Node: %s\n", nonterminals[newPTNode->tnt.nonterm]);
                        newPTNode->parent = keepTrackNode;
                        newPTNode->sibling = NULL;
                        newPTNode->child = NULL;
                        newPTNode->leafNodeFlag = 0;
                        // printf("Parent set\n");
                        currentNode->child = newPTNode; // Starts filling children

                        // printf("Child set\n");
                        currentNode = currentNode->child; // Current node is now the child
                        flagSib++;
                    }
                    else
                    {
                        // Create sibling of currentnode
                        TNT l1 = firstNode->tnt;
                        parse_tree_node *newPTNode = (parse_tree_node *)malloc(sizeof(parse_tree_node));
                        newPTNode->parent = keepTrackNode;
                        newPTNode->sibling = NULL;
                        newPTNode->child = NULL;
                        newPTNode->leafNodeFlag = 0;
                        createParseTreeNode(l1, newPTNode);
                        currentNode->sibling = newPTNode; // Starts filling siblings
                        currentNode = currentNode->sibling;
                    }
                    firstNode = firstNode->next;
                }
                if (flagSib)
                {
                    currentNode = keepTrackNode;      // Goes back to where it was
                    currentNode = currentNode->child; // Goes to the leftmost child to now start processing.
                    currentNode->parent = keepTrackNode;
                }
                else
                {
                    // Recurse parents until we reach a sibling
                    while (currentNode->parent != NULL)
                    {
                        currentNode = currentNode->parent;
                        if (currentNode->sibling != NULL)
                        {
                            currentNode = currentNode->sibling;
                            break;
                        }
                    }
                }
                while (!isEmpty(Stemp))
                {
                    hash_table_element toBePushed1 = top(Stemp);
                    // printf("To be pushed: %s\n", nonterminals[toBePushed1.tnt.nonterm]);
                    Stemp = pop(Stemp);
                    // printf("Popped\n");
                    S = push(S, toBePushed1);
                    // printf("Pushed\n");
                    if (Stemp == NULL)
                    {
                        // printf("Stemp is null\n");
                    }
                    else
                    {
                        hash_table_element tempTop = top(Stemp);
                        // printf("Top of Stemp: %s\n", nonterminals[tempTop.tnt.nonterm]);
                    }
                }
            }
            else
            {
                // TODO: error recovery, no rule found in parse table
                printf("Line: %d. Rule error\n", L.lineNumber);
                // Keep getting tokens till we find something in the syncSet
                token_set *syncSet = createTokenSet();
                // Add SQBC to syncSet using addToken function
                // addToken(syncSet, SEMICOL);
                addToken(syncSet, END); // Why is there an error here?
                // if follow set of X contains epsilon
                if (isMember(&followSet[X.tnt.nonterm], EPSILON))
                {
                    addToken(syncSet, SEMICOL);
                    addToken(syncSet, BC);
                    addToken(syncSet, START);
                    addToken(syncSet, SQBC);
                }
                // addToken(syncSet, SEMICOL);
                // addToken(syncSet, BC);
                // addToken(syncSet, START);
                //  Fill syncSet with all the tokens in followSet of the nonterminal on top of stack
                for (int i = 0; i < NUM_TOKENS; i++)
                {
                    if (isMember(&followSet[X.tnt.nonterm], i) && i != EPSILON)
                    {
                        addToken(syncSet, i);
                    }
                }
                while (isMember(syncSet, L.tokenID) == 0)
                {
                    L = getNextToken(fp);
                    // TODO: Call function to print error - Premature end of input
                    if (L.tokenID == PROGRAMEND)
                    {
                        prematureEndOfInputError();
                        return;
                    }
                }
                // printf("L: %s\n", terminals[L.tokenID]);
                // printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
                if (isMember(&followSet[X.tnt.nonterm], L.tokenID))
                {
                    S = pop(S);
                    X = top(S);
                    continue;
                }
                // Keep popping non terminals till we find L in its first set or if X is a terminal, they are equal
                while (((X.type == __NONTERMINAL__ && !isMember(&followSet[X.tnt.nonterm], L.tokenID)) || (X.type == __TERMINAL__ && !isMember(&followSet[X.tnt.tok], L.tokenID))) && isEmpty(S) == 0)
                {
                    S = pop(S);
                    X = top(S);
                    // printf("Popped\n");
                    // printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
                }
                // Print X now
                //  printf("X: %s\n", X.type == __TERMINAL__ ? terminals[X.tnt.tok] : nonterminals[X.tnt.nonterm]);
                while (((X.type == __TERMINAL__ && X.tnt.tok != L.tokenID) || (X.type == __NONTERMINAL__ && isMember(&firstSet[X.tnt.nonterm], L.tokenID) == 0)) && isEmpty(S) == 0)
                {
                    S = pop(S);
                    X = top(S);
                    // printf("Popped\n");
                }
                // Handle parse tree accordingly
                while (((X.type == __TERMINAL__ && terminals[X.tnt.tok] != terminals[currentNode->tnt.tok]) || (X.type == __NONTERMINAL__ && nonterminals[X.tnt.nonterm] != nonterminals[currentNode->tnt.nonterm])) && currentNode->parent != NULL)
                {
                    // printf("Current Node: %s %s  LeafNode: %d\n", terminals[currentNode->tnt.tok], nonterminals[currentNode->tnt.nonterm], currentNode->leafNodeFlag);
                    if (currentNode->sibling == NULL)
                    {
                        currentNode = currentNode->parent;
                    }
                    else
                    {
                        currentNode = currentNode->sibling;
                    }
                }
                /*while(1){
                    if(currentNode->sibling != NULL){
                        currentNode = currentNode->sibling;
                    }
                    else{
                        currentNode = currentNode->parent;
                    }
                    if((currentNode->leafNodeFlag == 1 && currentNode->leafNodeInfo.tokenID == L.tokenID) || isMember(&firstSet[currentNode->tnt.nonterm], L.tokenID)){
                        break;
                    }
                }*/
            }
        }
    }
    if (L.tokenID != PROGRAMEND)
    {
        // printf("Parsing unsuccessful\n");
        // Space for error recovery, stack empty but input not finished
        FILE *fp1 = fopen("parseTree.txt", "w");
        printParseTree(fp1);
        fclose(fp1);
    }
    else
    {
        printf("Parsing completed\n");
        FILE *fp1 = fopen(parseTreeFile, "w");
        printParseTree(fp1);
        fclose(fp1);
    }
    fclose(fp);
    free(hashTable);
    destroyList(rules);
    free(S);
}

void printParseTree(FILE *fp)
{
    // printf("Inside printParseTree\n");
    parse_tree_node *currentNode = parseTree.root;
    fprintf(fp, "%15s\t%10s\t%10s\t%10s\t%10s\t%10s\t%20s\n", "lexeme", "lineNo", "tokenName", "valueIfNumber", "parentNodeSymbol", "isLeafNode", "nodeSymbol");
    printSubTree(currentNode, fp);
}

void printSubTree(parse_tree_node *currentNode, FILE *fp)
{
    if (currentNode == NULL)
        return;
    parse_tree_node *leftChild = currentNode->child;
    parse_tree_node *rightChild = NULL;
    if (leftChild != NULL)
    {
        printSubTree(leftChild, fp);
        rightChild = leftChild->sibling;
    }
    // Printing the lexeme when leaf node
    if (currentNode->leafNodeFlag)
    {
        if (currentNode->tnt.tok == EPSILON)
            return;
        if (currentNode->tnt.tok == ID)
            fprintf(fp, "%15s\t", currentNode->leafNodeInfo.lexeme);
        else if (currentNode->tnt.tok == NUM || currentNode->tnt.tok == RNUM)
            fprintf(fp, "%15s\t", currentNode->leafNodeInfo.lexeme);
        else
            fprintf(fp, "%15s\t", currentNode->leafNodeInfo.lexeme);
    }
    else
    {
        fprintf(fp, "%15s\t", "--------");
    }
    // Printing the line number when leaf node
    if (currentNode->leafNodeFlag)
    {
        fprintf(fp, "%10d\t", currentNode->leafNodeInfo.lineNumber);
    }
    else
    {
        fprintf(fp, "%10s\t", "--------");
    }
    // Printing the token name when leaf node
    if (currentNode->leafNodeFlag)
    {
        fprintf(fp, "%10s\t", terminals[currentNode->tnt.tok]);
    }
    else
    {
        fprintf(fp, "%10s\t", "--------");
    }
    // Printing the value when leaf node is NUM or RNUM
    if (currentNode->leafNodeFlag)
    {
        if (currentNode->tnt.tok == NUM)
            fprintf(fp, "%10d\t", currentNode->leafNodeInfo.val.intValue);
        else if (currentNode->tnt.tok == RNUM)
            fprintf(fp, "%10f\t", currentNode->leafNodeInfo.val.floatValue);
        else
            fprintf(fp, "%10s\t", "--------");
    }
    else
    {
        fprintf(fp, "%10s\t", "--------");
    }
    // Printing the parent node symbol
    if (currentNode->parent == NULL)
    {
        fprintf(fp, "%10s\t", "--------");
    }
    else
    {
        fprintf(fp, "%10s\t", nonterminals[currentNode->parent->tnt.nonterm]);
    }
    // Printing the leaf node flag
    if (currentNode->leafNodeFlag)
    {
        fprintf(fp, "%15s\t", "YES");
    }
    else
    {
        fprintf(fp, "%15s\t", "NO");
    }
    // Printing the node symbol
    if (currentNode->leafNodeFlag)
    {
        fprintf(fp, "%25s\t", terminals[currentNode->tnt.tok]);
    }
    else
    {
        fprintf(fp, "%25s\t", nonterminals[currentNode->tnt.nonterm]);
    }
    fprintf(fp, "\n");
    while (rightChild != NULL)
    {
        printSubTree(rightChild, fp);
        rightChild = rightChild->sibling;
    }
}

grammar createRuleList(char *grammarFile, hash_table_element *hashTable)
{
    FILE *fp;
    char line[128];
    linked_list_node *lhs_nt;
    linked_list_node *rhs_nt;
    fp = fopen(grammarFile, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    grammar rules = createLinkedListArray(NUM_RULES);
    int i = 0;
    char delim[] = ",\n";
    while (fgets(line, 128, fp) != NULL)
    {
        // printf("%s", line);
        char *tnt = strtok(line, delim);
        // printf("LHS: %s\n", tnt);
        if (hashTable[hash(tnt)].type != __NONTERMINAL__)
        {
            printf("Error in grammar at line %d\n", i + 1);
            printf("Expected nonterminal, found %s\n", tnt);
        }
        // printf("It is a nonterminal\n");
        lhs_nt = createNode(hashTable[hash(tnt)].tnt, __NONTERMINAL__); // Potential memleak
        // printf("Node created\n");
        addNode(&rules[i], lhs_nt);
        // printf("Node added\n");
        // printf("LHS: %s\n", tnt);
        tnt = strtok(NULL, delim);
        while (tnt != NULL)
        {
            tnt = trim(tnt);
            // printf("%s\n", tnt);
            if (hashTable[hash(tnt)].type == __NONE__)
            {
                printf("None token found in rule %d RHS\n", i + 1);
                // printf("%s\n", tnt);
                // printf("Length of string: %ld\n", strlen(tnt));
                // printf("Nonterminals: %s\n", nonterminals[22]);
                // printf("Length of %ld\n", strlen(nonterminals[22]));
                exit(1);
            }
            else if (hashTable[hash(tnt)].type == __EPSILON__)
            {
                // printf("EPSILON found in rule %d RHS\n", i + 1);
                break;
            }
            else
            {
                rhs_nt = createNode(hashTable[hash(tnt)].tnt, hashTable[hash(tnt)].type);
                addNode(&rules[i], rhs_nt);
            }
            tnt = strtok(NULL, delim);
        }
        i++;
    }
    fclose(fp);
    // free(lhs_nt);
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

hash_table_element *createHashTable()
{
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
        // printf("%d %s %d\n", i, nonterminals[i], h);
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
        // printf("%d %s %d\n", i, terminals[i], h);
        if (hashTable[h].type == __NONE__)
        {
            hashTable[h].type = __TERMINAL__;
            hashTable[h].tnt.tok = i;
        }
        else
        {
            collisions++;
            // printf("Hash collision with token %d--- %s. Colliding token: %s\n", i, terminals[i], hashTable[h].type == __TERMINAL__ ? terminals[hashTable[h].tnt.tok] : nonterminals[hashTable[h].tnt.nonterm]);
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
        // printf("Hash collision\n");
        // Print term colliding with epsilon
        // if (hashTable[h].type == __TERMINAL__)
        // {
        // printf("Colliding term: %s\n", terminals[hashTable[h].tnt.tok]);
        // }
        // else
        // {
        // printf("Colliding nonterm: %s\n", nonterminals[hashTable[h].tnt.nonterm]);
        // }
    }
    // printf("Collisions in hash table: %d\n", collisions);
    return hashTable;
}

void print_rules(grammar rules)
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
    if (cur == 0)
    {
        printf("EMPTY SET");
        return;
    }
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        if (cur & 1)
        {
            printf("%s ", terminals[i]);
        }
        cur = cur >> 1;
    }
    if (isMember(set, EPSILON))
    {
        printf("EPSILON");
    }
}

void prematureEndOfInputError()
{
    // End of source code achieved but stack not empty
    // TODO: Print a better error message in case needed.
    printf("Expected more code\n");
    FILE *fp1 = fopen("parseTree.txt", "w");
    printParseTree(fp1);
    fclose(fp1);
}

// int main()
// {
//     // hash_table_element *hashTable = createHashTable("nonterminals.txt", "terminals.txt");
//     // linked_list *rules = createRuleList("grammar.csv", hashTable);
//     // print_rules(rules);
//     // token_set *first_sets = malloc(sizeof(token_set) * NUM_NONTERMINALS);
//     // token_set *follow_sets = malloc(sizeof(token_set) * NUM_NONTERMINALS);
//     // computeFirstAndFollowSets(first_sets, follow_sets, rules);
//     // for (int i = 0; i < NUM_NONTERMINALS; i++)
//     // {
//     //     printf("First(%s): ", nonterminals[i]);
//     //     printSet(&first_sets[i]);
//     //     printf("\n");
//     // }

//     // for (int i = 0; i < NUM_NONTERMINALS; i++)
//     // {
//     //     printf("Follow(%s): ", nonterminals[i]);
//     //     printSet(&follow_sets[i]);
//     //     printf("\n");
//     // }
//     // printf("Enter the buffer size: ");
//     // scanf("%d", &bufferSize);
//     parseInputSourceCode("../../tests/t5.txt", "parseTree.txt");
//     return 0;
// }
