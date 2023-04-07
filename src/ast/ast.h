#ifndef AST_H
#define AST_H

#include <stdio.h>

#include "../lexer/lexerdef.h"
#include "../parser/parserdef.h"

enum TYPE{
    __NUM__,
    __RNUM__,
    __BOOL__,
    __ARRAY__,
    __ERROR__
};

enum AST_NODE_TYPE{
    PROGRAM_AST,
    MODULE_DECLARATION_AST,
    MODULE_DEF_AST,
    DRIVER_MODULE_AST,
    RANGE_AST,
    GET_VALUE_AST,
    PRINT_AST,
    EQUALS_AST,
    USE_AST,
    DECLARE_AST,
    SWITCH_AST,
    CASE_AST,
    DEFAULT_AST,
    FOR_AST,
    WHILE_AST,
    PLUS_AST,
    MINUS_AST,
    MUL_AST,
    DIV_AST,
    AND_AST,
    OR_AST,
    EQ_AST,
    NE_AST,
    LT_AST,
    LE_AST,
    GT_AST,
    GE_AST,
    LINKED_LIST_AST,
    TRUE_PTN_AST = TRUE,
    FALSE_PTN_AST = FALSE,
    BOOLEAN_AST,
    ARRAY_AST,
    PARSE_TREE_AST = ID,
    FORMALPARAM_AST,
    PARSE_TREE_AST1 = NUM,
    PARSE_TREE_AST2 = RNUM,
    ARR_ELEM_AST,
    INTEGER_AST,
    REAL_AST,
    NUM_AST,
    RNUM_AST
};

typedef struct ast_node{
    enum AST_NODE_TYPE nodeType;
    struct ast_node *left;
    struct ast_node *right;
    enum TYPE type;
    tokenInfo leafNodeInfo;
    int isLeafNode;
    void *aux_info;
} ast_node;

typedef struct ast {
    ast_node *root;
} ast;

typedef struct LinkedListASTNode{
    ast_node *data;
    struct LinkedListASTNode *next;
} LinkedListASTNode;

struct programAuxInfo{
    LinkedListASTNode *ModDec;
    LinkedListASTNode *OtherMod1;
    ast_node *DriverMod;
    LinkedListASTNode *OtherMod2;
};

struct moduleDefAuxInfo{
    parse_tree_node *Id;
    LinkedListASTNode *input_plist;
    LinkedListASTNode *ret;
    LinkedListASTNode *moduleDef;
};

void print_ll(LinkedListASTNode *head, int depth, FILE *fp);
void print_parse_tree_node(parse_tree_node *node, int depth, FILE *fp);
void print_ast_node(ast_node *node, int depth, FILE *fp);

#endif