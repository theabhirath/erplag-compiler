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
    enum TYPE type;
    tokenInfo leafNodeInfo;
    int isLeafNode;
    void *aux_info;
} ast_node;

typedef struct ast {
    ast_node *root;
} ast;

struct LinkedListASTNode{
    ast_node *data;
    struct LinkedListNode *next;
};

typedef struct LinkedListASTNode LinkedListASTNode;

LinkedListASTNode *insertAtFront(LinkedListASTNode *head, ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    newNode->data = data;
    newNode->next = head;
    return newNode;
}

ast_node *createASTNode(enum AST_NODE_TYPE nodeType)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    node->nodeType = nodeType;
    node->left = NULL;
    node->right = NULL;
    node->aux_info = NULL;
    node->isLeafNode = 0;
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

struct ModuleDeclarationAuxInfo{
    parse_tree_node *ID;
    LinkedListASTNode *input_plist;
    LinkedListASTNode *ret;
    LinkedListASTNode *moduleDef;
};

void *createModule(parse_tree_node *ID, LinkedListASTNode *input_plist, LinkedListASTNode *ret, LinkedListASTNode *moduleDef)
{
    struct ModuleDeclarationAuxInfo *aux_info = (struct ModuleDeclarationAuxInfo *)malloc(sizeof(struct ModuleDeclarationAuxInfo));
    aux_info->ID = ID;
    aux_info->input_plist = input_plist;
    aux_info->ret = ret;
    aux_info->moduleDef = moduleDef;
    return aux_info;
}

/*
AST Node Type   |   Left Child    |   Right Child   |   Aux Info
----------------------------------------------------------------
PROGRAM         |          NULL   |   NULL          |   ProgramAuxInfo
MODULE_DECLARATION | NULL          |   NULL          |   ModuleDeclarationAuxInfo
DRIVER_MODULE   |   NULL          |   NULL          |   LinkedListASTNode (of statements)

*/

ast_node *process_subtree(parse_tree_node *ptn)
{
    int ruleNo = ptn->ruleNumber;
    
    switch(ruleNo)
    {
        case 1: // 1: <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules> PROGRAMEND

        {
            ast_node *cur_node = createASTNode(PROGRAM);

            parse_tree_node *ModDec = ptn->child;
            parse_tree_node *OtherMod1 = ModDec->sibling;
            parse_tree_node *DriverMod = OtherMod1->sibling;
            parse_tree_node *OtherMod2 = DriverMod->sibling;
            parse_tree_node *ProgramEnd = OtherMod2->sibling;

            process_subtree(ModDec);
            process_subtree(OtherMod1);
            process_subtree(DriverMod);
            process_subtree(OtherMod2);
            
            cur_node->addr = createASTNode(PROGRAM);
            cur_node->aux_info = createProgram(ModDec_ast->syn_addr, OtherMod1_ast->syn_addr, DriverMod_ast->addr, OtherMod2_ast->syn_addr);

            free(ModDec);
            free(OtherMod1);
            free(DriverMod);
            free(OtherMod2);
            free(ProgramEnd);

            return cur_node;
        }
        case 2: // 2: <moduleDeclarations>1 -> <moduleDeclaration> <moduleDeclarations>2
        {

            parse_tree_node *ModDec = ptn->child;
            parse_tree_node *ModDecs2 = ModDec->sibling;
            process_subtree(ModDec);
            process_subtree(ModDecs2);
            ptn->addr = ModDec->addr;
            ptn->syn_addr = insertAtFront(ModDecs2->syn_addr, ModDec->addr);
            free(ModDec);
            free(ModDecs2);
            break;
        }
        case 3: // 3: <moduleDeclarations> -> EPSILON
        {
            ptn->syn_addr = NULL;
            ptn->addr = NULL;
            break;
        }
        case 4: // 4: <moduleDeclaration> -> DECLARE MODULE ID SEMICOL
        {
            parse_tree_node *Declare = ptn->child;
            parse_tree_node *Module = Declare->sibling;
            parse_tree_node *ID = Module->sibling;
            parse_tree_node *SemiCol = ID->sibling;
            ast_node *cur_node = createASTNode(MODULE_DECLARATION);
            cur_node->aux_info = ID;
            ptn->addr = cur_node;
            free(Declare);
            free(Module);
            free(SemiCol);
            break;
        }
        case 5: // 5: <otherModules> -> <module> <otherModules>
        {
            parse_tree_node *Mod = ptn->child;
            parse_tree_node *OtherMods2 = Mod->sibling;
            process_subtree(Mod);
            ptn->addr = Mod->addr;
            process_subtree(OtherMods2);
            ptn->syn_addr = insertAtFront(OtherMods2->syn_addr, Mod->addr);
            free(Mod);
            free(OtherMods2);
            break;
        }
        case 6: // 6: <otherModules> -> EPSILON
        {
            ptn->syn_addr = NULL;
            ptn->addr = NULL;
            break;
        }
        case 7: // 7: <driverModule> -> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
        {
            parse_tree_node *DriverDef = ptn->child;
            parse_tree_node *Driver = DriverDef->sibling;
            parse_tree_node *Program = Driver->sibling;
            parse_tree_node *DriverEndDef = Program->sibling;
            parse_tree_node *ModDef = DriverEndDef->sibling;
            process_subtree(ModDef);
            ptn->addr = createASTNode(DRIVER_MODULE);
            ptn->addr->aux_info = ModDef->addr;
            free(DriverDef);
            free(Driver);
            free(Program);
            free(DriverEndDef);
            free(ModDef);
            break;
        }
        case 8: // 8: <module> -> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
        {
            parse_tree_node *Def = ptn->child;
            parse_tree_node *Module = Def->sibling;
            parse_tree_node *ID = Module->sibling;
            parse_tree_node *EndDef = ID->sibling;
            parse_tree_node *Takes = EndDef->sibling;
            parse_tree_node *Input = Takes->sibling;
            parse_tree_node *SqBo = Input->sibling;
            parse_tree_node *InputPlist = SqBo->sibling;
            parse_tree_node *SqBc = InputPlist->sibling;
            parse_tree_node *SemiCol = SqBc->sibling;
            parse_tree_node *Ret = SemiCol->sibling;
            parse_tree_node *ModDef = Ret->sibling;
            process_subtree(InputPlist);
            process_subtree(Ret);
            process_subtree(ModDef);
            ast_node *cur_node = createASTNode(MODULE_DECLARATION);
            cur_node->aux_info = createModule(ID, InputPlist->syn_addr, Ret->addr, ModDef->addr);
            ptn->addr = cur_node;
            free(Def);
            free(Module);
            free(EndDef);
            free(Takes);
            free(Input);
            free(SqBo);
            free(SqBc);
            free(SemiCol);
            free(Ret);
            free(ModDef);
            break;
        }
        case 9: // 9: <ret> -> RETURNS SQBO <output_plist> SQBC SEMICOL
        {
            parse_tree_node *Returns = ptn->child;
            parse_tree_node *SqBo = Returns->sibling;
            parse_tree_node *OutputPlist = SqBo->sibling;
            parse_tree_node *SqBc = OutputPlist->sibling;
            parse_tree_node *SemiCol = SqBc->sibling;
            process_subtree(OutputPlist);
            ptn->addr = OutputPlist->syn_addr;
            free(Returns);
            free(SqBo);
            free(OutputPlist);
            free(SqBc);
            free(SemiCol);
            break;
        }
        case 10: // 10: <ret> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 11: // 11: <input_plist> -> ID COLON <dataType> <iP0>
        {
            
        }
    }
    return NULL:
}

ast *create_ast(parse_tree *pt)
{
    ast *a = (ast *)malloc(sizeof(ast));
    a->root = process_subtree(pt->root);
    return a;
}