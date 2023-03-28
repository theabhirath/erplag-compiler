#include "../lexer/lexerdef.h"
#include "../parser/parserdef.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"


enum TYPE{
    __NUM__,
    __RNUM__,
    __BOOL__,
    __ARRAY__,
    __ERROR__
};

enum AST_NODE_TYPE{
    PROGRAM,
    MODULE_DECLARATION,
    MODULE_DEF,
    DRIVER_MODULE,
    RANGE,
    GET_VALUE,
    PRINT,
    EQUALS,
    USE,
    DECLARE,
    SWITCH,
    CASE,
    DEFAULT,
    FOR,
    WHILE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    AND,
    OR,
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,
    LINKED_LIST
};
typedef struct ast_node{
    enum AST_NODE_TYPE nodeType;
    struct ast_node *left;
    struct ast_node *right;
    struct ast_node *addr;
    struct ast_node *syn_addr;
    struct ast_node *inh_addr;
    enum TYPE type;
    tokenInfo leafNodeInfo;
    void *aux_info;
} ast_node;

typedef struct ast {
    ast_node *root;
} ast;

ast_node *createASTNode(enum AST_NODE_TYPE nodeType)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    node->nodeType = nodeType;
    node->left = NULL;
    node->right = NULL;
    node->addr = NULL;
    node->syn_addr = NULL;
    node->inh_addr = NULL;
    node->leafNodeInfo = NULL;
    node->aux_info = NULL;
    return node;
}

struct ProgramAuxInfo{
    ast_node *ModDec;
    ast_node *OtherMod1;
    ast_node *DriverMod;
    ast_node *OtherMod2;
};

void *createProgram(ast_node *ModDec, ast_node *OtherMod1, ast_node *DriverMod, ast_node *OtherMod2)
{
    struct ProgramAuxInfo *aux_info = (struct ProgramAuxInfo *)malloc(sizeof(struct ProgramAuxInfo));
    aux_info->ModDec = ModDec;
    aux_info->OtherMod1 = OtherMod1;
    aux_info->DriverMod = DriverMod;
    aux_info->OtherMod2 = OtherMod2;
    return aux_info;
}

ast_node *process_subtree(parse_tree_node *ptn)
{
    int ruleNo = ptn->ruleNumber;
    ast_node *cur_node = (ast_node *)malloc(sizeof(ast_node));

    switch(ruleNo)
    {
        case 1:
            parse_tree_node *ModDec = ptn->child;
            parse_tree_node *OtherMod1 = ModDec->sibling;
            parse_tree_node *DriverMod = ModDef->sibling;
            parse_tree_node *OtherMod2 = DriverMod->sibling;
            parse_tree_node *ProgramEnd = OtherMod2->sibling;
            ast_node *ModDec_ast = process_subtree(ModDec);
            ast_node *OtherMod1_ast = process_subtree(OtherMod1);
            ast_node *DriverMod_ast = process_subtree(DriverMod);
            ast_node *OtherMod2_ast = process_subtree(OtherMod2);
            
            cur_node->addr = createASTNode(PROGRAM);
            cur_node->aux_info = createProgram(ModDec_ast->syn_addr, OtherMod1_ast->syn_addr, DriverMod_ast->addr, OtherMod2_ast->syn_addr);

            free(ModDec);
            free(OtherMod1);
            free(DriverMod);
            free(OtherMod2);
            free(ProgramEnd);
            break;

        case 2: 

    }
}

ast *create_ast(parse_tree *pt)
{
    ast *a = (ast *)malloc(sizeof(ast));
    a->root = process_subtree(pt->root);
    return a;
}