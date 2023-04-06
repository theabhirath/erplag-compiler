#ifndef AST_H
#define AST_H

#include "../lexer/lexerdef.h"

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
    INTEGER_AST,
    REAL_AST,
    BOOLEAN_AST,
    ARRAY_AST,
    PARSE_TREE_AST,
    FORMALPARAM_AST,
    PARSE_TREE_AST1,
    PARSE_TREE_AST2,
    ARR_ELEM_AST
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

#endif