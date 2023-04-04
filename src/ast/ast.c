#include <stdlib.h>

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
    LINKED_LIST,
    INTEGER,
    REAL,
    BOOLEAN,
    ARRAY
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
    struct LinkedListNode *next;
} LinkedListASTNode;

LinkedListASTNode *insertAtFront(LinkedListASTNode *head, ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    newNode->data = data;
    newNode->next = head;
    return newNode;
}

LinkedListASTNode *insertAtEnd(LinkedListASTNode *head, ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    newNode->data = data;
    newNode->next = NULL;
    if(head == NULL)
        return newNode;
    LinkedListASTNode *temp = head;
    while(temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
    return head;
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
            
            ptn->addr = cur_node;
            cur_node->aux_info = createProgram(ModDec->syn_addr, OtherMod1->syn_addr, DriverMod->addr, OtherMod2->syn_addr);

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
            parse_tree_node *ID = ptn->child;
            parse_tree_node *Colon = ID->sibling;
            parse_tree_node *DataType = Colon->sibling;
            parse_tree_node *IP0 = DataType->sibling;
            process_subtree(DataType);
            process_subtree(IP0);
            DataType->syn_addr->aux_info = ID;
            ptn->syn_addr = insertAtFront(IP0->syn_addr, DataType->syn_addr);
            free(Colon);
            free(IP0);
            break;
        }
        case 12: // 12: <iP0> -> COMMA ID COLON <dataType> <iP0>
        {
            parse_tree_node *Comma = ptn->child;
            parse_tree_node *ID = Comma->sibling;
            parse_tree_node *Colon = ID->sibling;
            parse_tree_node *DataType = Colon->sibling;
            parse_tree_node *IP0 = DataType->sibling;
            process_subtree(DataType);
            process_subtree(IP0);
            DataType->syn_addr->aux_info = ID;
            ptn->syn_addr = insertAtFront(IP0->syn_addr, DataType->syn_addr); //check
            free(Comma);
            free(Colon);
            free(IP0);
            free(DataType);
            break;
        }
        case 13: // 13: <iP0> -> EPSILON
        {
            ptn->syn_addr = NULL;
            break;
        }
        case 14: // 14: <output_plist> -> ID COLON <type> <oP0>
        {
            parse_tree_node *ID = ptn->child;
            parse_tree_node *Colon = ID->sibling;
            parse_tree_node *Type = Colon->sibling;
            parse_tree_node *OP0 = Type->sibling;
            process_subtree(Type);
            process_subtree(OP0);
            Type->syn_addr->aux_info = ID;
            ptn->syn_addr = insertAtFront(OP0->syn_addr, Type->syn_addr);
            free(Colon);
            free(OP0);
            break;
        }
        case 15: // 15: <oP0> -> COMMA ID COLON <type> <oP0>
        {
            parse_tree_node *Comma = ptn->child;
            parse_tree_node *ID = Comma->sibling;
            parse_tree_node *Colon = ID->sibling;
            parse_tree_node *Type = Colon->sibling;
            parse_tree_node *OP0 = Type->sibling;
            process_subtree(Type);
            process_subtree(OP0);
            Type->syn_addr->aux_info = ID;
            ptn->syn_addr = insertAtFront(OP0->syn_addr, Type->syn_addr);
            free(Comma);
            free(Colon);
            free(OP0);
            free(Type);
            break;
        }
        case 16: // 16: <oP0> -> EPSILON
        {
            ptn->syn_addr = NULL;
            break;
        }
        case 17: // 17: <dataType> -> INTEGER
        {
            ptn->addr = createASTNode(INTEGER);
            break;
        }
        case 18: // 18: <dataType> -> REAL
        {
            ptn->addr = createASTNode(REAL);
            break;
        }
        case 19: // 19: <dataType> -> BOOLEAN
        {
            ptn->addr = createASTNode(BOOLEAN);
            break;
        }
        case 20: // 20: <dataType> -> ARRAY SQBO <range_arr> SQBC OF <type>
        {
            parse_tree_node *Array = ptn->child;
            parse_tree_node *SqBo = Array->sibling;
            parse_tree_node *RangeArr = SqBo->sibling;
            parse_tree_node *SqBc = RangeArr->sibling;
            parse_tree_node *Of = SqBc->sibling;
            parse_tree_node *Type = Of->sibling;
            process_subtree(RangeArr);
            process_subtree(Type);
            ptn->addr = createASTNode(ARRAY);
            ptn->addr->left = RangeArr->addr;
            ptn->addr->right = Type->addr;
            free(Array);
            free(SqBo);
            free(SqBc);
            free(Of);
            free(RangeArr);
            free(Type);
            break;
        }
        case 21: // 21: <type> -> INTEGER
        {
            ptn->addr = createASTNode(INTEGER);
            break;
        }
        case 22: // 22: <type> -> REAL
        {
            ptn->addr = createASTNode(REAL);
            break;
        }
        case 23: // 23: <type> -> BOOLEAN
        {
            ptn->addr = createASTNode(BOOLEAN);
            break;
        }
        case 24: // 24: <moduleDef> -> START <statements> END
        {
            parse_tree_node *Start = ptn->child;
            parse_tree_node *Statements = Start->sibling;
            parse_tree_node *End = Statements->sibling;
            process_subtree(Statements);
            ptn->addr = Statements->syn_addr;
            free(Start);
            free(End);
            free(Statements);
            break;
        }
        case 25: // 25: <statements> -> <statement> <statements>
        {
            parse_tree_node *Statement = ptn->child;
            parse_tree_node *Statements = Statement->sibling;
            ptn->addr = Statement->addr;
            process_subtree(Statement);
            process_subtree(Statements);
            ptn->syn_addr = insertAtFront(Statements->syn_addr, Statement->syn_addr);
            free(Statement);
            free(Statements);
            break;
        }
        case 26: // 26: <statements> -> EPSILON
        {
            ptn->syn_addr = makeASTNode(LINKED_LIST);
            break;
        }
        case 27: // 27: <statement> -> <ioStmt>
        {
            parse_tree_node *IOStmt = ptn->child;
            process_subtree(IOStmt);
            ptn->addr = IOStmt->addr;
            ptn->syn_addr = IOStmt->syn_addr; //check
            free(IOStmt);
            break;
        }
        case 28: // 28: <statement> -> <simpleStmt>
        {
            parse_tree_node *SimpleStmt = ptn->child;
            process_subtree(SimpleStmt);
            ptn->addr = SimpleStmt->addr;
            ptn->syn_addr = SimpleStmt->syn_addr;
            free(SimpleStmt);
            break;
        }
        case 29: // 29: <statement> -> <declareStmt>
        {
            parse_tree_node *DeclareStmt = ptn->child;
            process_subtree(DeclareStmt);
            ptn->addr = DeclareStmt->addr;
            ptn->syn_addr = DeclareStmt->syn_addr;
            free(DeclareStmt);
            break;
        }
        case 30: // 30: <statement> -> <conditionalStmt>
        {
            parse_tree_node *ConditionalStmt = ptn->child;
            process_subtree(ConditionalStmt);
            ptn->addr = ConditionalStmt->addr;
            ptn->syn_addr = ConditionalStmt->syn_addr;
            free(ConditionalStmt);
            break;
        }
        case 31: // 31: <statement> -> <iterativeStmt>
        {
            parse_tree_node *IterativeStmt = ptn->child;
            process_subtree(IterativeStmt);
            ptn->addr = IterativeStmt->addr;
            ptn->syn_addr = IterativeStmt->syn_addr;
            free(IterativeStmt);
            break;
        }
        case 32: // 32: <ioStmt> -> GET_VALUE BO ID BC SEMICOL
        {
            parse_tree_node *GetValue = ptn->child;
            parse_tree_node *Bo = GetValue->sibling;
            parse_tree_node *Id = Bo->sibling;
            parse_tree_node *Bc = Id->sibling;
            parse_tree_node *Semicol = Bc->sibling;
            ptn->addr = createASTNode(GET_VALUE);
            ptn->addr->right = Id;
            free(GetValue);
            free(Bo);
            free(Bc);
            free(Semicol);
            break;
        }
        case 33: // 33: <ioStmt> -> PRINT BO <var3> BC SEMICOL
        {
            parse_tree_node *Print = ptn->child;
            parse_tree_node *Bo = Print->sibling;
            parse_tree_node *Var3 = Bo->sibling;
            parse_tree_node *Bc = Var3->sibling;
            parse_tree_node *Semicol = Bc->sibling;
            process_subtree(Var3);
            ptn->addr = createASTNode(PRINT);
            ptn->addr->right = Var3->addr;
            free(Print);
            free(Bo);
            free(Bc);
            free(Semicol);
            free(Var3);
            break;
        }
        case 34: // 34: <var3> -> <var>
        {
            parse_tree_node *Var = ptn->child;
            process_subtree(Var);
            ptn->addr = Var->syn_addr;
            free(Var);
            break;
        }
        case 35: // 35: <var3> -> <bools>
        {
            parse_tree_node *Bools = ptn->child;
            process_subtree(Bools);
            ptn->addr = Bools->addr;
            free(Bools);
            break;
        }
        case 36: // 36: <var> -> ID <whichId>
        {
            parse_tree_node *Id = ptn->child;
            parse_tree_node *WhichId = Id->sibling;
            // process_subtree(WhichId);
            ptn->addr = Id;
            WhichId->inh_addr = ptn->addr;
            process_subtree(WhichId);
            free(WhichId); //check
            break;
        }
        case 37: // 37: <var> -> NUM
        {
            parse_tree_node *Num = ptn->child;
            ptn->addr = Num;
            break;
        }
        case 38: // 38: <var> -> RNUM
        {
            parse_tree_node *Rnum = ptn->child;
            ptn->addr = Rnum;
            break;
        }
        case 39: // 39: <whichId> -> SQBO <wI0>
        {
            parse_tree_node *SqBo = ptn->child;
            parse_tree_node *WI0 = SqBo->sibling;
            process_subtree(WI0);
            ptn->syn_addr = addRightChild(ptn->inh_addr, WI0->addr);
            free(SqBo);
            free(WI0);
            break;
        }
        case 40: // 40: <whichId> -> EPSILON    
        {
            ptn->syn_addr = ptn->inh_addr;
            break;
        }
        case 41: // 41: <wI0> -> <index_arr> SQBC
        {
            parse_tree_node *IndexArr = ptn->child;
            parse_tree_node *SqBc = IndexArr->sibling;
            process_subtree(IndexArr);
            ptn->addr = IndexArr->addr;
            free(SqBc);
            free(IndexArr);
            break;
        }
        case 42: // 42: <simpleStmt> -> <assignmentStmt>
        {
            parse_tree_node *AssignStmt = ptn->child;
            process_subtree(AssignStmt);
            ptn->addr = AssignStmt->addr;
            free(AssignStmt);
            break;
        }
        case 43: // 43: <simpleStmt> -> <moduleReuseStmt>
        {
            parse_tree_node *ModReuseStmt = ptn->child;
            process_subtree(ModReuseStmt);
            ptn->addr = ModReuseStmt->addr;
            free(ModReuseStmt);
            break;
        }
        case 44: // 44: <assignmentStmt> -> ID <whichStmt>
        {
            parse_tree_node *ID = ptn->child;
            parse_tree_node *WhichStmt = ID->sibling;
            ptn->addr = ID;
            WhichStmt->inh_addr = ptn->addr;
            process_subtree(WhichStmt);
            ptn->syn_addr = WhichStmt->syn_addr;
            free(WhichStmt);
            break;
        }
        case 45: // 45: <whichStmt> -> <lvalueIDStmt>
        {
            parse_tree_node *LvalueIDStmt = ptn->child;
            LvalueIDStmt->inh_addr = ptn->inh_addr;
            process_subtree(LvalueIDStmt);
            ptn->syn_addr = LvalueIDStmt->syn_addr;
            free(LvalueIDStmt);
            break;
        }
        case 46: // 46: <whichStmt> -> <lvalueARRStmt>
        {
            parse_tree_node *LvalueARRStmt = ptn->child;
            LvalueARRStmt->inh_addr = ptn->inh_addr;
            process_subtree(LvalueARRStmt);
            ptn->syn_addr = LvalueARRStmt->syn_addr;
            free(LvalueARRStmt);
            break;
        }
        case 47: // 47: <lvalueIDStmt> -> ASSIGNOP <expression> SEMICOL
        {
            parse_tree_node *AssignOp = ptn->child;
            parse_tree_node *Expression = AssignOp->sibling;
            parse_tree_node *SemiCol = Expression->sibling;
            free(AssignOp);
            ptn->addr = createASTNode(EQUALS);
            process_subtree(Expression);
            ptn->addr->right = Expression->addr;
            ptn->addr->left = ptn->inh_addr;
            free(Expression);
            free(SemiCol);
            ptn->syn_addr = ptn->addr;
            break;
        }
        case 48: // 48: <lvalueARRStmt> -> SQBO <element_index_with_expression> SQBC ASSIGNOP <expression> SEMICOL
        {
            parse_tree_node *SqBo = ptn->child;
            parse_tree_node *ElementIndexWithExpression = SqBo->sibling;
            parse_tree_node *SqBc = ElementIndexWithExpression->sibling;
            parse_tree_node *AssignOp = SqBc->sibling;
            parse_tree_node *Expression = AssignOp->sibling;
            parse_tree_node *SemiCol = Expression->sibling;
            free(AssignOp);
            ptn->addr = createASTNode(EQUALS);
            process_subtree(ElementIndexWithExpression);
            process_subtree(Expression);
            ptn->addr->right = Expression->addr;
            ptn->inh_addr->right = ElementIndexWithExpression->syn_addr;
            ptn->addr->left = ptn->inh_addr;
            free(ElementIndexWithExpression);
            free(Expression);
            free(SqBo);
            free(SqBc);
            free(SemiCol);
            break;
        }
        case 49: // 49: <element_index_with_expression> -> <sign> <N_10>
        {
            parse_tree_node *Sign = ptn->child;
            parse_tree_node *N_10 = Sign->sibling;
            process_subtree(Sign);
            
            ptn->addr = Sign->addr;
            N_10->inh_addr = ptn->addr;
            process_subtree(N_10);
            ptn->syn_addr = N_10->syn_addr;
            free(Sign);
            free(N_10);
            break;
        }
        case 50: //  <sign> -> PLUS
        {
            ptn->addr = NULL;
            break;
        }
        case 51: //  <sign> -> MINUS
        {
            ptn->addr = createASTNode(MINUS);
            // make left child null: already done
            break;
        }
        case 52: // 52: <sign> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 53: // 53: <N_10> -> <arrExpr>
        {
            parse_tree_node *ArrExpr = ptn->child;
            ArrExpr->inh_addr = ptn->inh_addr;
            process_subtree(ArrExpr);
            ptn->syn_addr = ArrExpr->syn_addr;
            free(ArrExpr);
            break;
        }
        case 54: // 54: <N_10> -> BO <arrExpr> BC
        {
            parse_tree_node *Bo = ptn->child;
            parse_tree_node *ArrExpr = Bo->sibling;
            parse_tree_node *Bc = ArrExpr->sibling;
            process_subtree(ArrExpr);

            ptn->addr = ArrExpr->syn_addr;
            if (ptn->inh_addr != NULL)
            {
                ptn->inh_addr->right = ptn->addr;
                ptn->addr = ptn->inh_addr;
            }
            free(Bo);
            free(ArrExpr);
            free(Bc);

            ptn->syn_addr = ptn->addr;
            break;
        }
        case 55: // 55: <arrExpr> -> <arrTerm> <arr_N4>
        {
            parse_tree_node *ArrTerm = ptn->child;
            parse_tree_node *Arr_N4 = ArrTerm->sibling;
            process_subtree(ArrTerm);
            ptn->addr = ArrTerm->syn_addr;
            if (ptn->inh_addr != NULL)
            {
                ptn->inh_addr->right = ptn->addr;
                ptn->addr = ptn->inh_addr;
            }
            free(ArrTerm);

            Arr_N4->inh_addr = ptn->addr;
            process_subtree(Arr_N4);
            ptn->syn_addr = Arr_N4->syn_addr;
            free(Arr_N4);
            break;
        }
        case 56: // 56: <arr_N4> -> <op1> <arrTerm> <arr_N4>
        {
            parse_tree_node *Op1 = ptn->child;
            parse_tree_node *ArrTerm = Op1->sibling;
            parse_tree_node *Arr_N4 = ArrTerm->sibling;
            process_subtree(Op1);
            ptn->addr = Op1->addr;
            process_subtree(ArrTerm);
            ptn->addr->right = ArrTerm->syn_addr;
            free(ArrTerm);
            free(Op1);

            Arr_N4->inh_addr = ptn->addr;
            ptn->addr->left = ptn->inh_addr;

            process_subtree(Arr_N4);

            ptn->syn_addr = Arr_N4->syn_addr;
            free(Arr_N4);
        }
        case 57: // 57: <arr_N4> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 58: // 58: <arrTerm> -> <arrFactor> <arr_N5>
        {
            parse_tree_node *ArrFactor = ptn->child;
            parse_tree_node *Arr_N5 = ArrFactor->sibling;
            process_subtree(ArrFactor);
            ptn->addr = ArrFactor->addr;
            free(ArrFactor);

            Arr_N5->inh_addr = ptn->addr;
            process_subtree(Arr_N5);
            ptn->syn_addr = Arr_N5->syn_addr;
            break;
        }
        case 59: // 59: <arr_N5> -> <op2> <arrFactor> <arr_N5>
        {
            parse_tree_node *Op2 = ptn->child;
            parse_tree_node *ArrFactor = Op2->sibling;
            parse_tree_node *Arr_N5 = ArrFactor->sibling;
            process_subtree(Op2);
            ptn->addr = Op2->addr;
            process_subtree(ArrFactor);
            ptn->addr->right = ArrFactor->syn_addr;
            free(ArrFactor);
            free(Op2);

            Arr_N5->inh_addr = ptn->addr;
            ptn->addr->left = ptn->inh_addr;

            process_subtree(Arr_N5);

            ptn->syn_addr = Arr_N5->syn_addr;
            free(Arr_N5);
            break;
        }
        case 60: // 60: <arr_N5> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 61: // 61: <arrFactor> -> BO <arrExpr> BC
        {
            parse_tree_node *Bo = ptn->child;
            parse_tree_node *ArrExpr = Bo->sibling;
            parse_tree_node *Bc = ArrExpr->sibling;
            process_subtree(ArrExpr);
            ptn->addr = ArrExpr->addr;
            free(Bo);
            free(ArrExpr);
            free(Bc);
            break;
        }
        case 62: // 62: <arrFactor> -> <id_num>
        {
            parse_tree_node *Id_num = ptn->child;
            process_subtree(Id_num);
            ptn->addr = Id_num->addr;
            free(Id_num);
            break;
        }
        case 63: // 63: <id_num> -> ID
        {
            parse_tree_node *ID = ptn->child;
            ptn->addr = ID;
        }
        case 64: // 64: <id_num> -> NUM
        {
            parse_tree_node *NUM = ptn->child;
            ptn->addr = NUM;
        }
        case 65: // 65: <arrFactor> -> <bools>
        {
            parse_tree_node *Bools = ptn->child;
            process_subtree(Bools);
            ptn->addr = Bools->addr;
            free(Bools);
            break;
        }
        case 66: // 66: <moduleReuseStmt> -> SQBO <idList> SQBC ASSIGNOP USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
        {
            parse_tree_node *Sqbo = ptn->child;
            parse_tree_node *IdList = Sqbo->sibling;
            parse_tree_node *Sqbc = IdList->sibling;
            parse_tree_node *Assignop = Sqbc->sibling;
            parse_tree_node *Use = Assignop->sibling;
            parse_tree_node *Module = Use->sibling;
            parse_tree_node *ID = Module->sibling;
            parse_tree_node *With = ID->sibling;
            parse_tree_node *Parameters = With->sibling;
            parse_tree_node *Actual_para_list = Parameters->sibling;
            parse_tree_node *Semicol = Actual_para_list->sibling;
            ptn->addr = createASTNode(EQUALS);
            process_subtree(IdList);
            process_subtree(Actual_para_list);
            ptn->addr->left = IdList->syn_addr;
            ast_node *temp = createASTNode(USE);
            temp->left = ID;
            temp->right = Actual_para_list->syn_addr;
            ptn->addr->right = temp;
            free(Sqbo);
            free(IdList);
            free(Sqbc);
            free(Assignop);
            free(Use);
            free(Module);
            free(With);
            free(Parameters);
            free(Actual_para_list);
            free(Semicol);
            break;
        }
        case 67: // 67: <moduleReuseStmt> -> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
        {
            parse_tree_node *Use = ptn->child;
            parse_tree_node *Module = Use->sibling;
            parse_tree_node *ID = Module->sibling;
            parse_tree_node *With = ID->sibling;
            parse_tree_node *Parameters = With->sibling;
            parse_tree_node *Actual_para_list = Parameters->sibling;
            parse_tree_node *Semicol = Actual_para_list->sibling;
            ptn->addr = createASTNode(USE);
            ptn->addr->left = ID;
            process_subtree(Actual_para_list);
            ptn->addr->right = Actual_para_list->syn_addr;
            free(Use);
            free(Module);
            free(With);
            free(Parameters);
            free(Actual_para_list);
            free(Semicol);
            break;
        }
        case 68: // 68: <actual_para_list> -> <aP00> <aP1>
        {
            parse_tree_node *AP00 = ptn->child;
            parse_tree_node *AP1 = AP00->sibling;
            process_subtree(AP00);
            process_subtree(AP1);
            ptn->syn_addr = insertAtFront(AP1->syn_addr, AP00->addr);
            free(AP00);
            free(AP1);
            break;
        }
        case 69: // 69: <aP1> -> COMMA <aP00> <aP1>
        {
            parse_tree_node *Comma = ptn->child;
            parse_tree_node *AP00 = Comma->sibling;
            parse_tree_node *AP1 = AP00->sibling;
            process_subtree(AP00);
            process_subtree(AP1);
            ptn->syn_addr = insertAtFront(AP1->syn_addr, AP00->addr);
            free(Comma);
            free(AP00);
            free(AP1);
            break;
        }
        case 70: // 70: <aP1> -> EPSILON
        {
            ptn->syn_addr = NULL;
            break;
        }
        case 71: // 71: <aP00> -> <sign> <aP0>
        {
            parse_tree_node *Sign = ptn->child;
            parse_tree_node *AP0 = Sign->sibling;
            process_subtree(Sign);
            process_subtree(AP0);
            if (Sign->addr != NULL)
            {
                ptn->addr = Sign->addr;
                ptn->addr->right = AP0->addr;
            }
            else
            {
                ptn->addr = AP0->addr;
            }
            free(Sign);
            free(AP0);
            break;
        }
        case 72: // 72: <aP00> -> <bools>
        {
            parse_tree_node *Bools = ptn->child;
            process_subtree(Bools);
            ptn->addr = Bools->addr;
            free(Bools);
            break;
        }
        case 73: // 73: <aP0> -> NUM
        {
            parse_tree_node *NUM = ptn->child;
            ptn->addr = NUM;
            break;
        }
        case 74: // 74: <aP0> -> RNUM
        {
            parse_tree_node *RNUM = ptn->child;
            ptn->addr = RNUM;
            break;
        }
        case 75: // 75: <aP0> -> ID <N_11>
        {
            parse_tree_node *ID = ptn->child;
            parse_tree_node *N_11 = ID->sibling;
            process_subtree(N_11);
            ptn->addr = ID;
            if (N_11->addr != NULL)
            {
               ptn->addr->right = N_11->addr;
            }
            free(N_11);
            break;
        }
        case 76: // 76: <idList> -> ID <iL0>
        {
            parse_tree_node *ID = ptn->child;
            parse_tree_node *IL0 = ID->sibling;
            process_subtree(IL0);
            ptn->syn_addr = insertAtFront(IL0->syn_addr, ID);
            free(IL0);
            break;
        }
        case 77: // 77: <iL0> -> COMMA ID <iL0>
        {
            parse_tree_node *Comma = ptn->child;
            parse_tree_node *ID = Comma->sibling;
            parse_tree_node *IL0 = ID->sibling;
            process_subtree(IL0);
            ptn->syn_addr = insertAtFront(IL0->syn_addr, ID);
            free(Comma);
            free(IL0);
            break;
        }
        case 78: // 78: <iL0> -> EPSILON
        {
            ptn->syn_addr = NULL;
            break;
        }
        case 79: // 79: <expression> -> <booleanExpr>
        {
            parse_tree_node *BooleanExpr = ptn->child;
            process_subtree(BooleanExpr);
            ptn->addr = BooleanExpr->syn_addr;
            free(BooleanExpr);
            break;
        }
        case 80: // 80: <booleanExpr> -> <arithmeticExpr> <BE00>
        {
            parse_tree_node *ArithmeticExpr = ptn->child;
            parse_tree_node *BE00 = ArithmeticExpr->sibling;
            process_subtree(ArithmeticExpr);
            ptn->addr = ArithmeticExpr->syn_addr;
            BE00->inh_addr = ptn->addr;
            process_subtree(BE00);
            ptn->syn_addr = BE00->syn_addr;
            free(ArithmeticExpr);
            free(BE00);
            break;
        }
        case 81: // 81: <booleanExpr> -> <bools> <BE0>
        {
            parse_tree_node *Bools = ptn->child;
            parse_tree_node *BE0 = Bools->sibling;
            process_subtree(Bools);
            ptn->addr = Bools->addr;
            BE0->inh_addr = ptn->addr;
            process_subtree(BE0);
            ptn->syn_addr = BE0->syn_addr;
            free(Bools);
            free(BE0);
            break;
        }
        case 82: // 82: <BE00> -> <relationalOp> <arithmeticExpr> <BE0>
        {
            parse_tree_node *RelationalOp = ptn->child;
            parse_tree_node *ArithmeticExpr = RelationalOp->sibling;
            parse_tree_node *BE0 = ArithmeticExpr->sibling;
            ptn->addr = RelationalOp->addr;
            process_subtree(ArithmeticExpr);
            ptn->addr->right = ArithmeticExpr->syn_addr;
            process_subtree(BE0);
            ptn->addr->left = ptn->inh_addr;
            BE0->inh_addr = ptn->addr;
            ptn->syn_addr = BE0->syn_addr;
            free(RelationalOp);
            free(ArithmeticExpr);
            free(BE0);
            break;
        }
        case 83: // 83: <BE00> -> <BE0>
        {
            parse_tree_node *BE0 = ptn->child;
            BE0->inh_addr = ptn->inh_addr;
            process_subtree(BE0);
            ptn->syn_addr = BE0->syn_addr;
            free(BE0);
            break;
        }
        case 84: // 84: <BE0> -> <logicalOp> <booleanExpr>
        {
            parse_tree_node *LogicalOp = ptn->child;
            parse_tree_node *BooleanExpr = LogicalOp->sibling;
            ptn->addr = LogicalOp->addr;
            process_subtree(BooleanExpr);
            ptn->addr->left = ptn->inh_addr;
            ptn->addr->right = BooleanExpr->syn_addr;
            ptn->syn_addr = ptn->addr;
            free(LogicalOp);
            free(BooleanExpr);
            break;
        }
        case 85: // 85: <BE0> -> EPSILON
        {
            ptn->syn_addr = ptn->inh_addr;
            break;
        }
        case 86: // 86: <arithmeticExpr> -> <sign> <term> <AE0>
        {
            parse_tree_node *Sign = ptn->child;
            parse_tree_node *Term = Sign->sibling;
            parse_tree_node *AE0 = Term->sibling;
            process_subtree(Sign);
            process_subtree(Term);
            ptn->addr = Sign->addr;
            if (ptn->addr != NULL)
            {
                ptn->addr->right = Term->syn_addr;
            }
            else
            {
                ptn->addr = Term->addr;
            }
            process_subtree(AE0);
            ptn->syn_addr = AE0->syn_addr;
            free(Sign);
            free(Term);
            free(AE0);
            break;
        }
        case 87: // 87: <AE0> -> <op1> <term> <AE0>
        {
            parse_tree_node *Op1 = ptn->child;
            parse_tree_node *Term = Op1->sibling;
            parse_tree_node *AE0 = Term->sibling;
            ptn->addr = Op1->addr;
            process_subtree(Term);
            ptn->addr->right = Term->addr;
            AE0->inh_addr = ptn->addr;
            process_subtree(AE0);
            ptn->syn_addr = AE0->syn_addr;
            free(Op1);
            free(Term);
            free(AE0);
            break;
        }
        case 88: // 88: <AE0> -> EPSILON
        {
            ptn->syn_addr = ptn->inh_addr;
            break;
        }
        case 89: // 89: <term> -> <factor> <T0>
        {
            parse_tree_node *Factor = ptn->child;
            parse_tree_node *T0 = Factor->sibling;
            process_subtree(Factor);
            ptn->addr = Factor->addr;
            T0->inh_addr = ptn->addr;
            process_subtree(T0);
            ptn->syn_addr = T0->syn_addr;
            free(Factor);
            free(T0);
            break;
        }
        case 90: // 90: <T0> -> <op2> <factor> <T0>
        {
            parse_tree_node *Op2 = ptn->child;
            parse_tree_node *Factor = Op2->sibling;
            parse_tree_node *T0 = Factor->sibling;
            ptn->addr = Op2->addr;
            process_subtree(Factor);
            ptn->addr->right = Factor->addr;
            T0->inh_addr = ptn->addr;
            process_subtree(T0);
            ptn->syn_addr = T0->syn_addr;
            free(Op2);
            free(Factor);
            free(T0);
            break;
        }
        case 91: // 91: <T0> -> EPSILON
        {
            ptn->syn_addr = ptn->inh_addr;
            break;
        }
        case 92: // 92: <op1> -> PLUS
        {
            parse_tree_node *Plus = ptn->child;
            ptn->addr = createASTNode(PLUS);
            free(Plus);
            break;
        }
        case 93: // 93: <op1> -> MINUS
        {
            parse_tree_node *Minus = ptn->child;
            ptn->addr = createASTNode(MINUS);
            free(Minus);
            break;
        }
        case 94: // 94: <op2> -> MUL
        {
            parse_tree_node *Mul = ptn->child;
            ptn->addr = createASTNode(MUL);
            free(Mul);
            break;
        }
        case 95: // 95: <op2> -> DIV
        {
            parse_tree_node *Div = ptn->child;
            ptn->addr = createASTNode(DIV);
            free(Div);
            break;
        }
        case 96: // 96: <bools> -> TRUE
        {
            parse_tree_node *TRUE = ptn->child;
            ptn->addr = TRUE;
            break;
        }
        case 97: // 97: <bools> -> FALSE
        {
            parse_tree_node *FALSE = ptn->child;
            ptn->addr = FALSE;
            break;
        }
        case 98: // 98: <factor> -> BO <expression> BC
        {
            parse_tree_node *BO = ptn->child;
            parse_tree_node *Expression = BO->sibling;
            parse_tree_node *BC = Expression->sibling;
            process_subtree(Expression);
            ptn->addr = Expression->addr;
            free(BO);
            free(Expression);
            free(BC);
            break;
        }
        case 99: // 99: <factor> -> NUM
        {
            parse_tree_node *NUM = ptn->child;
            ptn->addr = NUM;
            break;
        }
        case 100: // 100: <factor> -> RNUM
        {
            parse_tree_node *RNUM = ptn->child;
            ptn->addr = RNUM;
            break;
        }
        case 101: // 101: <factor> -> ID <N_11>
        {
            parse_tree_node *ID = ptn->child;
            parse_tree_node *N_11 = ID->sibling;
            ptn->addr = ID;
            if (N_11->child != NULL)
            {
                process_subtree(N_11);
                ptn->addr->right = N_11->syn_addr;
                free(N_11);
            }
            break;
        }
        case 102: // 102: <N_11> -> SQBO <element_index_with_expression> SQBC
        {
            parse_tree_node *SQBO = ptn->child;
            parse_tree_node *Element_index_with_expression = SQBO->sibling;
            parse_tree_node *SQBC = Element_index_with_expression->sibling;
            process_subtree(Element_index_with_expression);
            ptn->addr = Element_index_with_expression->syn_addr;
            free(SQBO);
            free(Element_index_with_expression);
            free(SQBC);
            break;
        }
        case 103: // 103: <N_11> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 104: // 104: <logicalOp> -> AND
        {
            parse_tree_node *AND = ptn->child;
            ptn->addr = createASTNode(AND);
            free(AND);
            break;
        }
        case 105: // 105: <logicalOp> -> OR
        {
            parse_tree_node *Or = ptn->child;
            ptn->addr = createASTNode(OR);
            free(Or);
            break;
        }
        case 106: // 106: <relationalOp> -> EQ
        {
            parse_tree_node *Eq = ptn->child;
            ptn->addr = createASTNode(EQ);
            free(Eq);
            break;
        }
        case 107: // 107: <relationalOp> -> LT
        {
            parse_tree_node *Lt = ptn->child;
            ptn->addr = createASTNode(LT);
            free(Lt);
            break;
        }
        case 108: // 108: <relationalOp> -> LE
        {
            parse_tree_node *Le = ptn->child;
            ptn->addr = createASTNode(LE);
            free(Le);
            break;
        }
        case 109: // 109: <relationalOp> -> GT
        {
            parse_tree_node *Gt = ptn->child;
            ptn->addr = createASTNode(GT);
            free(Gt);
            break;
        }
        case 110: // 110: <relationalOp> -> GE
        {
            parse_tree_node *Ge = ptn->child;
            ptn->addr = createASTNode(GE);
            free(Ge);
            break;
        }
        case 111: // 111: <relationalOp> -> NE
        {
            parse_tree_node *Ne = ptn->child;
            ptn->addr = createASTNode(NE);
            free(Ne);
            break;
        }
        case 112: // 112: <declareStmt> -> DECLARE <idList> COLON <dataType> SEMICOL
        {
            parse_tree_node *Declare = ptn->child;
            parse_tree_node *IdList = Declare->sibling;
            parse_tree_node *COLON = IdList->sibling;
            parse_tree_node *DataType = COLON->sibling;
            parse_tree_node *SEMICOL = DataType->sibling;
            process_subtree(IdList);
            process_subtree(DataType);
            ptn->addr = createASTNode(DECLARE);
            ptn->addr->left = IdList->syn_addr;
            ptn->addr->right = DataType->addr;
            free(Declare);
            free(IdList);
            free(COLON);
            free(DataType);
            free(SEMICOL);
            break;
        }
        case 113: // 113: <conditionalStmt> -> SWITCH BO ID BC START <caseStmts> <default> END
        {
            parse_tree_node *Switch = ptn->child;
            parse_tree_node *BO = Switch->sibling;
            parse_tree_node *ID = BO->sibling;
            parse_tree_node *BC = ID->sibling;
            parse_tree_node *START = BC->sibling;
            parse_tree_node *CaseStmts = START->sibling;
            parse_tree_node *Default = CaseStmts->sibling;
            parse_tree_node *END = Default->sibling;
            process_subtree(CaseStmts);
            process_subtree(Default);
            ptn->addr = createASTNode(SWITCH);
            ptn->addr->left = ID;
            ptn->addr->right = insertAtEnd(CaseStmts->addr, Default->addr);
            free(Switch);
            free(BO);
            free(ID);
            free(BC);
            free(START);
            free(CaseStmts);
            free(Default);
            free(END);
            break;
        }
        case 114: // 114: <caseStmts> -> <caseStmt> <caseStmts>
        {
            parse_tree_node *CaseStmt = ptn->child;
            parse_tree_node *CaseStmts = CaseStmt->sibling;
            process_subtree(CaseStmt);
            process_subtree(CaseStmts);
            ptn->addr = insertAtFront(CaseStmt->addr, CaseStmts->addr);
            free(CaseStmt);
            free(CaseStmts);
            break;
        }
        case 115: // 115: <caseStmts> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        case 116: // 116: <caseStmt> -> CASE <value> COLON <statements> BREAK SEMICOL
        {
            parse_tree_node *Case = ptn->child;
            parse_tree_node *Value = Case->sibling;
            parse_tree_node *COLON = Value->sibling;
            parse_tree_node *Statements = COLON->sibling;
            parse_tree_node *BREAK = Statements->sibling;
            parse_tree_node *SEMICOL = BREAK->sibling;
            process_subtree(Value);
            process_subtree(Statements);
            ptn->addr = createASTNode(CASE);
            ptn->addr->left = Value->addr;
            ptn->addr->right = Statements->syn_addr;
            free(Case);
            free(Value);
            free(COLON);
            free(Statements);
            free(BREAK);
            free(SEMICOL);
            break;
        }
        case 117: // 117: <value> -> NUM
        {
            parse_tree_node *NUM = ptn->child;
            ptn->addr = NUM;
            break;
        }
        case 118: // 118: <value> -> TRUE
        {
            parse_tree_node *TRUE = ptn->child;
            ptn->addr = TRUE;
            break;
        }
        case 119: // 119: <value> -> FALSE
        {
            parse_tree_node *FALSE = ptn->child;
            ptn->addr = FALSE;
            break;
        }
        case 120: // 120: <default> -> DEFAULT COLON <statements> BREAK SEMICOL
        {
            parse_tree_node *Default = ptn->child;
            parse_tree_node *COLON = Default->sibling;
            parse_tree_node *Statements = COLON->sibling;
            parse_tree_node *BREAK = Statements->sibling;
            parse_tree_node *SEMICOL = BREAK->sibling;
            process_subtree(Statements);
            ptn->addr = createASTNode(DEFAULT);
            ptn->addr->right = Statements->syn_addr;
            free(Default);
            free(COLON);
            free(Statements);
            free(BREAK);
            free(SEMICOL);
            break;
        }
        case 121: // 121: <default> -> EPSILON
        {
            ptn->addr = NULL;
            break;
        }
        // TODO check how to handle semantic rules for this
        case 122: // 122: <iterativeStmt> -> FOR BO ID IN NUM RANGEOP NUM BC START <statements> END
        {

        }
        case 123: // 123: <iterativeStmt> -> WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END
        {
            parse_tree_node *While = ptn->child;
            parse_tree_node *BO = While->sibling;
            parse_tree_node *ArithmeticOrBooleanExpr = BO->sibling;
            parse_tree_node *BC = ArithmeticOrBooleanExpr->sibling;
            parse_tree_node *START = BC->sibling;
            parse_tree_node *Statements = START->sibling;
            parse_tree_node *END = Statements->sibling;
            process_subtree(ArithmeticOrBooleanExpr);
            process_subtree(Statements);
            ptn->addr = createASTNode(WHILE);
            ptn->addr->left = ArithmeticOrBooleanExpr->addr;
            ptn->addr->right = Statements->syn_addr;
            free(While);
            free(BO);
            free(ArithmeticOrBooleanExpr);
            free(BC);
            free(START);
            free(Statements);
            free(END);
            break;
        }
        case 124: // 124: 124: <forrange> -> <sign> NUM RANGEOP <sign> NUM
        {
            parse_tree_node *Sign1 = ptn->child;
            parse_tree_node *NUM1 = Sign1->sibling;
            parse_tree_node *RANGEOP = NUM1->sibling;
            parse_tree_node *Sign2 = RANGEOP->sibling;
            parse_tree_node *NUM2 = Sign2->sibling;
            process_subtree(Sign1);
            process_subtree(Sign2);
            ptn->addr = createASTNode(RANGE);
            if (Sign1->addr != NULL)
            {
                ptn->addr->left = Sign1->addr;
                Sign1->addr->right = NUM1->addr;
            }
            else
            {
                ptn->addr->left = NUM1->addr;
            }
            if (Sign2->addr != NULL)
            {
                ptn->addr->right = Sign2->addr;
                Sign2->addr->right = NUM2->addr;
            }
            else
            {
                ptn->addr->right = NUM2->addr;
            }
            free(Sign1);
            free(NUM1);
            free(RANGEOP);
            free(Sign2);
            free(NUM2);
            break;
        }
        case 125: // 125: <range_arr> -> <index_arr> RANGEOP <index_arr>
        {
            parse_tree_node *Index1 = ptn->child;
            parse_tree_node *RANGEOP = Index1->sibling;
            parse_tree_node *Index2 = RANGEOP->sibling;
            process_subtree(Index1);
            process_subtree(Index2);
            ptn->addr = createASTNode(RANGE);
            ptn->addr->left = Index1->addr;
            ptn->addr->right = Index2->addr;
            free(Index1);
            free(RANGEOP);
            free(Index2);
            break;
        }
        case 126: // 126: <index_arr> -> <sign> <new_index>
        {
            parse_tree_node *Sign = ptn->child;
            parse_tree_node *NewIndex = Sign->sibling;
            process_subtree(Sign);
            if (Sign->addr != NULL)
            {
                ptn->addr = Sign->addr;
                Sign->addr->right = NewIndex->addr;
            }
            else
            {
                ptn->addr = NewIndex->addr;
            }
            free(Sign);
            free(NewIndex);
            break;
        }
        case 127: // 127: <new_index> -> NUM
        {
            parse_tree_node *NUM = ptn->child;
            ptn->addr = NUM;
            break;
        }
        case 128: // 128: <new_index> -> ID
        {
            parse_tree_node *ID = ptn->child;
            ptn->addr = ID;
            break;
        }
    return NULL;
}

ast *create_ast(parse_tree *pt)
{
    ast *a = (ast *)malloc(sizeof(ast));
    a->root = process_subtree(pt->root);
    return a;
}