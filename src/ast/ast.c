#include <stdio.h>
#include <stdlib.h>

#include "../parser/parserdef.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "ast.h"
#include "symbol_table.h"

int numASTNodes = 0;
int sizeOfAST = 0;

LinkedListASTNode *insertAtFront(LinkedListASTNode *head, ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    numASTNodes++;
    sizeOfAST += sizeof(LinkedListASTNode);
    newNode->data = data;
    newNode->next = head;
    return newNode;
}

LinkedListASTNode *createLinkedListASTNode(ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    numASTNodes++;
    sizeOfAST += sizeof(LinkedListASTNode);
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

LinkedListASTNode *insertAtEnd(LinkedListASTNode *head, ast_node *data)
{
    LinkedListASTNode *newNode = (LinkedListASTNode *)malloc(sizeof(LinkedListASTNode));
    numASTNodes++;
    sizeOfAST += sizeof(LinkedListASTNode);
    newNode->data = data;
    newNode->next = NULL;
    if (head == NULL)
    {
        return newNode;
    }
    LinkedListASTNode *temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = newNode;
    return head;
}

ast_node *createASTNode(enum AST_NODE_TYPE nodeType)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    numASTNodes++;
    sizeOfAST += sizeof(ast_node);
    node->nodeType = nodeType;
    node->left = NULL;
    node->right = NULL;
    node->aux_info = NULL;
    node->isLeafNode = 0;
    return node;
}

void *createProgram(LinkedListASTNode *ModDec, LinkedListASTNode *OtherMod1, ast_node *DriverMod, LinkedListASTNode *OtherMod2)
{
    struct programAuxInfo *aux_info = (struct programAuxInfo *)malloc(sizeof(struct programAuxInfo));
    numASTNodes++;
    sizeOfAST += sizeof(struct programAuxInfo);
    aux_info->ModDec = ModDec;
    aux_info->OtherMod1 = OtherMod1;
    aux_info->DriverMod = DriverMod;
    aux_info->OtherMod2 = OtherMod2;
    return aux_info;
}

void *createModule(parse_tree_node *Id, LinkedListASTNode *input_plist, LinkedListASTNode *ret, LinkedListASTNode *moduleDef)
{
    struct moduleDefAuxInfo *aux_info = (struct moduleDefAuxInfo *)malloc(sizeof(struct moduleDefAuxInfo));
    numASTNodes++;
    sizeOfAST += sizeof(struct moduleDefAuxInfo);
    aux_info->Id = Id;
    aux_info->input_plist = input_plist;
    aux_info->ret = ret;
    aux_info->moduleDef = moduleDef;
    return aux_info;
}

ast_node *process_subtree(parse_tree_node *ptn)
{
    int ruleNo = ptn->ruleNumber;
    // printf("Rule No: %d, Node: %s, Line No: %d\n", ruleNo + 1,\
    //        ptn->leafNodeFlag ? terminals[ptn->tnt.tok] : nonterminals[ptn->tnt.nonterm],\
    //        ptn->leafNodeInfo.lineNumber);
    // fflush(stdout);
    switch (ruleNo + 1)
    {
    case 1: // 1: <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules> PROGRAMEND

    {
        // printf("Rule 1\n");
        ast_node *cur_node = createASTNode(PROGRAM_AST);
        // printf("%s\n", cur_node->nodeType == PROGRAM_AST ? "Program" : "Not Program");
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
        // printf("Rule 1 Done\n");
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
        parse_tree_node *Id = Module->sibling;
        parse_tree_node *SemiCol = Id->sibling;
        ast_node *cur_node = createASTNode(MODULE_DECLARATION_AST);
        cur_node->aux_info = Id;
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
        ptn->addr = createASTNode(DRIVER_MODULE_AST);
        ptn->addr->aux_info = ModDef->addr;
        if (ModDef->addr != NULL)
        {
            LinkedListASTNode *body = ModDef->addr;
            ast_node *front = body->data;
            ptn->addr->beginLine = front->beginLine;
            ptn->addr->endLine = front->endLine;
        }
        free(DriverDef);
        free(Driver);
        free(Program);
        free(DriverEndDef);
        free(ModDef);
        break;
    }
    case 8: // 8: <module> -> DEF MODULE Id ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
    {
        parse_tree_node *Def = ptn->child;
        parse_tree_node *Module = Def->sibling;
        parse_tree_node *Id = Module->sibling;
        parse_tree_node *EndDef = Id->sibling;
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
        ast_node *cur_node = createASTNode(MODULE_DEF_AST);
        cur_node->aux_info = createModule(Id, InputPlist->syn_addr, Ret->addr, ModDef->addr);
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
    case 11: // 11: <input_plist> -> Id COLON <dataType> <iP0>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *Colon = Id->sibling;
        parse_tree_node *DataType = Colon->sibling;
        parse_tree_node *IP0 = DataType->sibling;
        process_subtree(DataType);
        process_subtree(IP0);
        ast_node *cur_node = createASTNode(FORMALPARAM_AST);
        cur_node->left = Id;
        cur_node->right = DataType->addr;
        ptn->syn_addr = insertAtFront(IP0->syn_addr, cur_node);
        free(Colon);
        free(IP0);
        break;
    }
    case 12: // 12: <iP0> -> COMMA Id COLON <dataType> <iP0>
    {
        parse_tree_node *Comma = ptn->child;
        parse_tree_node *Id = Comma->sibling;
        parse_tree_node *Colon = Id->sibling;
        parse_tree_node *DataType = Colon->sibling;
        parse_tree_node *IP0 = DataType->sibling;
        process_subtree(DataType);
        process_subtree(IP0);
        ast_node *cur_node = createASTNode(FORMALPARAM_AST);
        cur_node->left = Id;
        cur_node->right = DataType->addr;
        ptn->syn_addr = insertAtFront(IP0->syn_addr, cur_node);
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
    case 14: // 14: <output_plist> -> Id COLON <type> <oP0>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *Colon = Id->sibling;
        parse_tree_node *Type = Colon->sibling;
        parse_tree_node *OP0 = Type->sibling;
        process_subtree(Type);
        process_subtree(OP0);
        ast_node *cur_node = createASTNode(FORMALPARAM_AST);
        cur_node->left = Id;
        cur_node->right = Type->addr;
        ptn->syn_addr = insertAtFront(OP0->syn_addr, cur_node);
        free(Colon);
        free(OP0);
        break;
    }
    case 15: // 15: <oP0> -> COMMA Id COLON <type> <oP0>
    {
        parse_tree_node *Comma = ptn->child;
        parse_tree_node *Id = Comma->sibling;
        parse_tree_node *Colon = Id->sibling;
        parse_tree_node *Type = Colon->sibling;
        parse_tree_node *OP0 = Type->sibling;
        process_subtree(Type);
        process_subtree(OP0);
        ast_node *cur_node = createASTNode(FORMALPARAM_AST);
        cur_node->left = Id;
        cur_node->right = Type->addr;
        ptn->syn_addr = insertAtFront(OP0->syn_addr, cur_node);
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
        ptn->addr = createASTNode(INTEGER_AST);
        break;
    }
    case 18: // 18: <dataType> -> REAL
    {
        ptn->addr = createASTNode(REAL_AST);
        break;
    }
    case 19: // 19: <dataType> -> BOOLEAN
    {
        ptn->addr = createASTNode(BOOLEAN_AST);
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
        ptn->addr = createASTNode(ARRAY_AST);
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
        ptn->addr = createASTNode(INTEGER_AST);
        break;
    }
    case 22: // 22: <type> -> REAL
    {
        ptn->addr = createASTNode(REAL_AST);
        break;
    }
    case 23: // 23: <type> -> BOOLEAN
    {
        ptn->addr = createASTNode(BOOLEAN_AST);
        break;
    }
    case 24: // 24: <moduleDef> -> START <statements> END
    {
        parse_tree_node *Start = ptn->child;
        parse_tree_node *Statements = Start->sibling;
        parse_tree_node *End = Statements->sibling;
        process_subtree(Statements);
        ptn->addr = Statements->syn_addr;
        if (ptn->addr)
        {
            ast_node *front_node = ((LinkedListASTNode *)ptn->addr)->data;
            front_node->beginLine = getLineNumber(Start);
            front_node->endLine = getLineNumber(End);
        }
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
        ptn->syn_addr = insertAtFront(Statements->syn_addr, Statement->addr);
        free(Statement);
        free(Statements);
        break;
    }
    case 26: // 26: <statements> -> EPSILON
    {
        ptn->syn_addr = NULL;
        break;
    }
    case 27: // 27: <statement> -> <ioStmt>
    {
        parse_tree_node *IOStmt = ptn->child;
        process_subtree(IOStmt);
        ptn->addr = IOStmt->addr;
        free(IOStmt);
        break;
    }
    case 28: // 28: <statement> -> <simpleStmt>
    {
        parse_tree_node *SimpleStmt = ptn->child;
        process_subtree(SimpleStmt);
        ptn->addr = SimpleStmt->addr;
        free(SimpleStmt);
        break;
    }
    case 29: // 29: <statement> -> <declareStmt>
    {
        parse_tree_node *DeclareStmt = ptn->child;
        process_subtree(DeclareStmt);
        ptn->addr = DeclareStmt->addr;
        free(DeclareStmt);
        break;
    }
    case 30: // 30: <statement> -> <conditionalStmt>
    {
        parse_tree_node *ConditionalStmt = ptn->child;
        process_subtree(ConditionalStmt);
        ptn->addr = ConditionalStmt->addr;
        free(ConditionalStmt);
        break;
    }
    case 31: // 31: <statement> -> <iterativeStmt>
    {
        parse_tree_node *IterativeStmt = ptn->child;
        process_subtree(IterativeStmt);
        ptn->addr = IterativeStmt->addr;
        free(IterativeStmt);
        break;
    }
    case 32: // 32: <ioStmt> -> GET_VALUE BO Id BC SEMICOL
    {
        parse_tree_node *GetValue = ptn->child;
        parse_tree_node *Bo = GetValue->sibling;
        parse_tree_node *Id = Bo->sibling;
        parse_tree_node *Bc = Id->sibling;
        parse_tree_node *Semicol = Bc->sibling;
        ptn->addr = createASTNode(GET_VALUE_AST);
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
        ptn->addr = createASTNode(PRINT_AST);
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
    case 36: // 36: <var> -> Id <whichId>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *WhichId = Id->sibling;
        ptn->addr = Id;
        WhichId->inh_addr = ptn->addr;
        process_subtree(WhichId);
        ptn->syn_addr = WhichId->syn_addr;
        free(WhichId); // check
        break;
    }
    case 37: // 37: <var> -> NUM
    {
        parse_tree_node *Num = ptn->child;
        ptn->syn_addr = Num;
        break;
    }
    case 38: // 38: <var> -> RNUM
    {
        parse_tree_node *Rnum = ptn->child;
        ptn->syn_addr = Rnum;
        break;
    }
    case 39: // 39: <whichId> -> SQBO <wI0>
    {
        parse_tree_node *SqBo = ptn->child;
        parse_tree_node *WI0 = SqBo->sibling;
        process_subtree(WI0);
        ptn->syn_addr = createASTNode(ARR_ELEM_AST);
        ptn->syn_addr->left = ptn->inh_addr;
        ptn->syn_addr->right = WI0->addr;
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
        ptn->addr = AssignStmt->syn_addr;
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
    case 44: // 44: <assignmentStmt> -> Id <whichStmt>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *WhichStmt = Id->sibling;
        ptn->addr = Id;
        WhichStmt->inh_addr = ptn->addr;
        process_subtree(WhichStmt);
        ptn->syn_addr = WhichStmt->syn_addr;
        free(WhichStmt);
        break;
    }
    case 45: // 45: <whichStmt> -> <lvalueIdStmt>
    {
        parse_tree_node *LvalueIdStmt = ptn->child;
        LvalueIdStmt->inh_addr = ptn->inh_addr;
        process_subtree(LvalueIdStmt);
        ptn->syn_addr = LvalueIdStmt->syn_addr;
        free(LvalueIdStmt);
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
    case 47: // 47: <lvalueIdStmt> -> ASSIGNOP <expression> SEMICOL
    {
        parse_tree_node *AssignOp = ptn->child;
        parse_tree_node *Expression = AssignOp->sibling;
        parse_tree_node *SemiCol = Expression->sibling;
        free(AssignOp);
        ptn->addr = createASTNode(EQUALS_AST);
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
        ptn->syn_addr = createASTNode(EQUALS_AST);
        process_subtree(ElementIndexWithExpression);
        process_subtree(Expression);
        ast_node *temp = createASTNode(ARR_ELEM_AST);
        parse_tree_node *temp1 = ptn->inh_addr;
        temp->left = ptn->inh_addr;
        temp->right = ElementIndexWithExpression->syn_addr;
        if (ElementIndexWithExpression->syn_addr == NULL)
        {
            // printf("rule is wrong\n");
            // fflush(stdout);
        }
        else
        {
            if (temp->right->nodeType == MINUS_AST)
            {
                parse_tree_node *temp2 = temp->right->right;
            }
        }
        ptn->syn_addr->right = Expression->addr;
        ptn->syn_addr->left = temp;
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
        ptn->addr = createASTNode(MINUS_AST);
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
        ptn->syn_addr = ptn->inh_addr;
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
    case 59: // 59: <arr_N5> -> <op2> <sign> <arrFactor> <arr_N5>
    {
        parse_tree_node *Op2 = ptn->child;
        parse_tree_node *Sign = Op2->sibling;
        parse_tree_node *ArrFactor = Sign->sibling;
        parse_tree_node *Arr_N5 = ArrFactor->sibling;
        process_subtree(Op2);
        ptn->addr = Op2->addr;
        process_subtree(Sign);
        if(Sign->addr != NULL)
        {
            process_subtree(ArrFactor);
            Sign->addr->right = ArrFactor->syn_addr;
            ptn->addr->right = Sign->addr;
        }
        else{
            process_subtree(ArrFactor);
            ptn->addr->right = ArrFactor->syn_addr;
        }
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
        ptn->syn_addr = ptn->inh_addr;
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
        ptn->syn_addr = ptn->addr;
        free(Id_num);
        break;
    }
    case 63: // 63: <id_num> -> Id
    {
        parse_tree_node *Id = ptn->child;
        ptn->addr = Id;
        break;
    }
    case 64: // 64: <id_num> -> NUM
    {
        parse_tree_node *NUM = ptn->child;
        ptn->addr = NUM;
        break;
    }
    case 65: // 65: <arrFactor> -> <bools>
    {
        parse_tree_node *Bools = ptn->child;
        process_subtree(Bools);
        ptn->addr = Bools->addr;
        free(Bools);
        break;
    }
    case 66: // 66: <moduleReuseStmt> -> SQBO <idList> SQBC ASSIGNOP USE MODULE Id WITH PARAMETERS <actual_para_list> SEMICOL
    {
        parse_tree_node *Sqbo = ptn->child;
        parse_tree_node *IdList = Sqbo->sibling;
        parse_tree_node *Sqbc = IdList->sibling;
        parse_tree_node *Assignop = Sqbc->sibling;
        parse_tree_node *Use = Assignop->sibling;
        parse_tree_node *Module = Use->sibling;
        parse_tree_node *Id = Module->sibling;
        parse_tree_node *With = Id->sibling;
        parse_tree_node *Parameters = With->sibling;
        parse_tree_node *Actual_para_list = Parameters->sibling;
        parse_tree_node *Semicol = Actual_para_list->sibling;
        ptn->addr = createASTNode(EQUALS_AST);
        process_subtree(IdList);
        process_subtree(Actual_para_list);
        ptn->addr->left = IdList->syn_addr;
        ast_node *temp = createASTNode(USE_AST);
        temp->left = Id;
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
    case 67: // 67: <moduleReuseStmt> -> USE MODULE Id WITH PARAMETERS <actual_para_list> SEMICOL
    {
        parse_tree_node *Use = ptn->child;
        parse_tree_node *Module = Use->sibling;
        parse_tree_node *Id = Module->sibling;
        parse_tree_node *With = Id->sibling;
        parse_tree_node *Parameters = With->sibling;
        parse_tree_node *Actual_para_list = Parameters->sibling;
        parse_tree_node *Semicol = Actual_para_list->sibling;
        ptn->addr = createASTNode(USE_AST);
        ptn->addr->left = Id;
        process_subtree(Actual_para_list);
        ptn->addr->aux_info = Actual_para_list->syn_addr;
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
        // ptn->addr->aux_info = insertAtFront(AP1->addr->aux_info, AP00->addr);
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
        // LinkedListASTNode *temp = createLinkedListASTNode(AP1->syn_addr);
        // printf("ghaplaa 2 \n");
        // fflush(stdout);
        // LinkedListASTNode *temp = (LinkedListASTNode *) (AP1->addr->aux_info);
        // temp = insertAtFront(temp, AP00->addr);
        // printf("ghaplaa 2 mid \n");
        // fflush(stdout);
        // ptn->addr->aux_info = temp;
        // printf("ghaplaa 2 over \n");
        // fflush(stdout);
        free(Comma);
        free(AP00);
        free(AP1);
        break;
    }
    case 70: // 70: <aP1> -> EPSILON
    {
        ptn->syn_addr = NULL;
        // ptn->addr = createASTNode(PARSE_TREE_AST1);
        // printf("ghaplaa \n");
        // fflush(stdout);
        // ptn->addr->aux_info = createLinkedListASTNode(ptn->addr);
        // printf("ghaplaa here\n");
        // fflush(stdout);
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
    case 75: // 75: <aP0> -> Id <N_11>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *N_11 = Id->sibling;
        process_subtree(N_11);
        ptn->addr = Id;
        if (N_11->addr != NULL)
        {
            ptn->addr = createASTNode(ARR_ELEM_AST);
            ptn->addr->left = Id;
            ptn->addr->right = N_11->addr;
        }
        free(N_11);
        break;
    }
    case 76: // 76: <idList> -> Id <iL0>
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *IL0 = Id->sibling;
        process_subtree(IL0);
        ptn->syn_addr = insertAtFront(IL0->syn_addr, Id);
        free(IL0);
        break;
    }
    case 77: // 77: <iL0> -> COMMA Id <iL0>
    {
        parse_tree_node *Comma = ptn->child;
        parse_tree_node *Id = Comma->sibling;
        parse_tree_node *IL0 = Id->sibling;
        process_subtree(IL0);
        ptn->syn_addr = insertAtFront(IL0->syn_addr, Id);
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
        process_subtree(RelationalOp);
        // printf("relop reached\n");
        // fflush(stdout);
        ptn->addr = RelationalOp->addr;
        process_subtree(ArithmeticExpr);
        ptn->addr->right = ArithmeticExpr->syn_addr;
        // process_subtree(BE0);
        ptn->addr->left = ptn->inh_addr;
        BE0->inh_addr = ptn->addr;
        process_subtree(BE0);
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
        process_subtree(LogicalOp);
        ptn->addr = LogicalOp->addr;
        process_subtree(BooleanExpr);
        ptn->addr->right = BooleanExpr->syn_addr;
        ptn->addr->left = ptn->inh_addr;
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
            ptn->addr = Term->syn_addr;
        }
        AE0->inh_addr = ptn->addr;
        process_subtree(AE0);
        ptn->syn_addr = AE0->syn_addr;
        fflush(stdout);
        if (ptn->addr->nodeType == 32 || ptn->addr->nodeType == 34)
        {
            parse_tree_node *temp = ptn->addr;
        }
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
        process_subtree(Op1);
        ptn->addr = Op1->addr;
        process_subtree(Term);
        ptn->addr->right = Term->syn_addr;
        AE0->inh_addr = ptn->addr;
        ptn->addr->left = ptn->inh_addr;
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
    case 90: // 90: <T0> -> <op2> <sign> <factor> <T0>
    {
        parse_tree_node *Op2 = ptn->child;
        parse_tree_node *Sign = Op2->sibling;
        parse_tree_node *Factor = Sign->sibling;
        parse_tree_node *T0 = Factor->sibling;
        process_subtree(Op2);
        ptn->addr = Op2->addr;
        process_subtree(Sign);
        if(Sign->addr != NULL)
        {
            process_subtree(Factor);
            Sign->addr->right = Factor->addr;
            ptn->addr->right = Sign->addr;

        }
        else
        {
            process_subtree(Factor);
            ptn->addr->right = Factor->addr;
        } 
        
        //Don't touch below
        T0->inh_addr = ptn->addr;
        ptn->addr->left = ptn->inh_addr;
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
        ptn->addr = createASTNode(PLUS_AST);
        free(Plus);
        break;
    }
    case 93: // 93: <op1> -> MINUS
    {
        parse_tree_node *Minus = ptn->child;
        ptn->addr = createASTNode(MINUS_AST);
        free(Minus);
        break;
    }
    case 94: // 94: <op2> -> MUL
    {
        parse_tree_node *Mul = ptn->child;
        ptn->addr = createASTNode(MUL_AST);
        free(Mul);
        break;
    }
    case 95: // 95: <op2> -> DIV
    {
        parse_tree_node *Div = ptn->child;
        ptn->addr = createASTNode(DIV_AST);
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
    case 101: // 101: <factor> -> Id <N_11> //TODO: sahi nahi hai
    {
        parse_tree_node *Id = ptn->child;
        parse_tree_node *N_11 = Id->sibling;
        ptn->addr = Id;
        process_subtree(N_11);
        if (N_11->addr != NULL)
        {
            ptn->addr = createASTNode(ARR_ELEM_AST);
            ptn->addr->left = Id;
            ptn->addr->right = N_11->addr;
        }
        free(N_11);
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
        ptn->addr = createASTNode(AND_AST);
        free(AND);
        break;
    }
    case 105: // 105: <logicalOp> -> OR
    {
        parse_tree_node *Or = ptn->child;
        ptn->addr = createASTNode(OR_AST);
        free(Or);
        break;
    }
    case 106: // 106: <relationalOp> -> EQ
    {
        parse_tree_node *Eq = ptn->child;
        ptn->addr = createASTNode(EQ_AST);
        free(Eq);
        break;
    }
    case 107: // 107: <relationalOp> -> LT
    {
        parse_tree_node *Lt = ptn->child;
        ptn->addr = createASTNode(LT_AST);
        free(Lt);
        break;
    }
    case 108: // 108: <relationalOp> -> LE
    {
        parse_tree_node *Le = ptn->child;
        ptn->addr = createASTNode(LE_AST);
        free(Le);
        break;
    }
    case 109: // 109: <relationalOp> -> GT
    {
        parse_tree_node *Gt = ptn->child;
        ptn->addr = createASTNode(GT_AST);
        free(Gt);
        break;
    }
    case 110: // 110: <relationalOp> -> GE
    {
        parse_tree_node *Ge = ptn->child;
        ptn->addr = createASTNode(GE_AST);
        free(Ge);
        break;
    }
    case 111: // 111: <relationalOp> -> NE
    {
        parse_tree_node *Ne = ptn->child;
        ptn->addr = createASTNode(NE_AST);
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
        ptn->addr = createASTNode(DECLARE_AST);
        ptn->addr->left = IdList->syn_addr;
        ptn->addr->right = DataType->addr;
        free(Declare);
        free(IdList);
        free(COLON);
        free(DataType);
        free(SEMICOL);
        break;
    }
    case 113: // 113: <conditionalStmt> -> SWITCH BO Id BC START <caseStmts> <default> END
    {
        parse_tree_node *Switch = ptn->child;
        parse_tree_node *BO = Switch->sibling;
        parse_tree_node *Id = BO->sibling;
        parse_tree_node *BC = Id->sibling;
        parse_tree_node *START = BC->sibling;
        parse_tree_node *CaseStmts = START->sibling;
        parse_tree_node *Default = CaseStmts->sibling;
        parse_tree_node *END = Default->sibling;
        process_subtree(CaseStmts);
        process_subtree(Default);
        ptn->addr = createASTNode(SWITCH_AST);
        ptn->addr->left = Id;
        ptn->addr->right = insertAtEnd(CaseStmts->addr, Default->addr);
        free(Switch);
        free(BO);
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
        ptn->addr = insertAtFront(CaseStmts->addr, CaseStmt->addr);
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
        parse_tree_node *Colon = Value->sibling;
        parse_tree_node *Statements = Colon->sibling;
        parse_tree_node *Break = Statements->sibling;
        parse_tree_node *Semicol = Break->sibling;
        process_subtree(Value);
        process_subtree(Statements);
        ptn->addr = createASTNode(CASE_AST);
        ptn->addr->beginLine = getLineNumber(Colon);
        ptn->addr->endLine = getLineNumber(Break);
        ptn->addr->left = Value->addr;
        ptn->addr->right = Statements->syn_addr;
        free(Case);
        free(Value);
        free(Colon);
        free(Statements);
        free(Break);
        free(Semicol);
        break;
    }
    case 117: // 117: <value> -> NUM
    {
        parse_tree_node *Num = ptn->child;
        ptn->addr = Num;
        break;
    }
    case 118: // 118: <value> -> TRUE
    {
        parse_tree_node *True = ptn->child;
        ptn->addr = True;
        break;
    }
    case 119: // 119: <value> -> FALSE
    {
        parse_tree_node *False = ptn->child;
        ptn->addr = False;
        break;
    }
    case 120: // 120: <default> -> DEFAULT COLON <statements> BREAK SEMICOL
    {
        parse_tree_node *Default = ptn->child;
        parse_tree_node *Colon = Default->sibling;
        parse_tree_node *Statements = Colon->sibling;
        parse_tree_node *Break = Statements->sibling;
        parse_tree_node *SEMICOL = Break->sibling;
        process_subtree(Statements);
        ptn->addr = createASTNode(DEFAULT_AST);
        ptn->addr->beginLine = getLineNumber(Colon);
        ptn->addr->endLine = getLineNumber(Break);
        ptn->addr->left = NULL;
        ptn->addr->right = Statements->syn_addr;
        free(Default);
        free(Colon);
        free(Statements);
        free(Break);
        free(SEMICOL);
        break;
    }
    case 121: // 121: <default> -> EPSILON
    {
        ptn->addr = NULL;
        break;
    }
    case 122: // 122: <iterativeStmt> -> FOR BO Id IN <forrange> BC START <statements> END
    {
        parse_tree_node *For = ptn->child;
        parse_tree_node *Bo = For->sibling;
        parse_tree_node *Id = Bo->sibling;
        parse_tree_node *In = Id->sibling;
        parse_tree_node *Forrange = In->sibling;
        parse_tree_node *Bc = Forrange->sibling;
        parse_tree_node *Start = Bc->sibling;
        parse_tree_node *Statements = Start->sibling;
        parse_tree_node *End = Statements->sibling;
        process_subtree(Forrange);
        process_subtree(Statements);
        ptn->addr = createASTNode(FOR_AST);
        ptn->addr->left = Forrange->addr;
        ptn->addr->right = Statements->syn_addr;
        ptn->addr->aux_info = Id;
        ptn->addr->beginLine = getLineNumber(Start);
        ptn->addr->endLine = getLineNumber(End);
        free(For);
        free(Bo);
        free(In);
        free(Forrange);
        free(Bc);
        free(Start);
        free(Statements);
        free(End);
        break;
    }
    case 123: // 123: <iterativeStmt> -> WHILE BO <booleanExpr> BC START <statements> END
    {
        parse_tree_node *While = ptn->child;
        parse_tree_node *BO = While->sibling;
        parse_tree_node *BooleanExpr = BO->sibling;
        parse_tree_node *BC = BooleanExpr->sibling;
        parse_tree_node *START = BC->sibling;
        parse_tree_node *Statements = START->sibling;
        parse_tree_node *END = Statements->sibling;
        process_subtree(BooleanExpr);
        process_subtree(Statements);
        ptn->addr = createASTNode(WHILE_AST);
        ptn->addr->left = BooleanExpr->syn_addr;
        ptn->addr->right = Statements->syn_addr;
        ptn->addr->beginLine = getLineNumber(START);
        ptn->addr->endLine = getLineNumber(END);
        free(While);
        free(BO);
        free(BooleanExpr);
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
        ptn->addr = createASTNode(RANGE_AST);
        if (Sign1->addr != NULL)
        {
            ptn->addr->left = Sign1->addr;
            Sign1->addr->right = NUM1;
        }
        else
        {
            ptn->addr->left = NUM1;
        }
        if (Sign2->addr != NULL)
        {
            ptn->addr->right = Sign2->addr;
            Sign2->addr->right = NUM2;
        }
        else
        {
            ptn->addr->right = NUM2;
        }
        free(Sign1);
        free(RANGEOP);
        free(Sign2);
        break;
    }
    case 125: // 125: <range_arr> -> <index_arr> RANGEOP <index_arr>
    {
        parse_tree_node *Index1 = ptn->child;
        parse_tree_node *RANGEOP = Index1->sibling;
        parse_tree_node *Index2 = RANGEOP->sibling;
        process_subtree(Index1);
        process_subtree(Index2);
        ptn->addr = createASTNode(RANGE_AST);
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
        process_subtree(NewIndex);
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
        parse_tree_node *Num = ptn->child;
        ptn->addr = Num;
        break;
    }
    case 128: // 128: <new_index> -> Id
    {
        parse_tree_node *Id = ptn->child;
        ptn->addr = Id;
        break;
    }
    }
    return NULL;
}

ast *create_ast(parse_tree *pt)
{
    ast *a = (ast *)malloc(sizeof(ast));
    numASTNodes = 0;
    sizeOfAST = sizeof(ast);
    a->root = process_subtree(pt->root);
    return a;
}

char *getName(ast_node *node)
{
    parse_tree_node *ptn = node;
    return ptn->leafNodeInfo.lexeme;
}

int getNumVal(ast_node *node)
{
    parse_tree_node *ptn = node;
    return ptn->leafNodeInfo.val.intValue;
}

int getLineNumber(ast_node *node)
{
    if (node->nodeType == ARR_ELEM_AST)
    {
        return getLineNumber(node->left);
    }
    parse_tree_node *ptn = node;
    return ptn->leafNodeInfo.lineNumber;
}

void print_ll(LinkedListASTNode *head, int depth, FILE *fp)
{
    LinkedListASTNode *temp = head;
    while (temp != NULL)
    {
        print_ast_node(temp->data, depth, fp);
        temp = temp->next;
    }
}

void print_ll_pt(LinkedListASTNode *head, int depth, FILE *fp)
{
    LinkedListASTNode *temp = head;
    while (temp != NULL)
    {
        print_parse_tree_node(temp->data, depth, fp);
        temp = temp->next;
    }
}

void print_parse_tree_node(parse_tree_node *node, int depth, FILE *fp)
{
    if (node == NULL)
    {
        return;
    }
    fprintf(fp, "%*s", depth, "");
    fprintf(fp, "%s\n", node->leafNodeInfo.lexeme); // segfault
    fflush(fp);
}

void print_ast_node(ast_node *node, int depth, FILE *fp)
{
    if (node == NULL)
    {
        return;
    }
    fprintf(fp, "%*s", depth, "");
    fflush(fp);
    switch (node->nodeType)
    {
    case PROGRAM_AST:
    {
        fprintf(fp, "PROGRAM_AST\n\n");
        fflush(fp);
        struct programAuxInfo *pai = (struct programAuxInfo *)node->aux_info;
        print_ll(pai->ModDec, depth + 1, fp);
        print_ll(pai->OtherMod1, depth + 1, fp);
        print_ast_node(pai->DriverMod, depth + 1, fp);
        print_ll(pai->OtherMod2, depth + 1, fp);
        break;
    }
    case MODULE_DECLARATION_AST:
    {
        fprintf(fp, "MODULE_DECLARATION_AST\n");
        fflush(fp);
        parse_tree_node *id = node->aux_info;
        fprintf(fp, "Module Name: %s\n\n", id->leafNodeInfo.lexeme);
        break;
    }
    case MODULE_DEF_AST:
    {
        fprintf(fp, "MODULE_DEF_AST\n\n");
        fflush(fp);
        struct moduleDefAuxInfo *mdai = (struct moduleDefAuxInfo *)node->aux_info;
        print_ll(mdai->input_plist, depth + 1, fp);
        print_ll(mdai->ret, depth + 1, fp);
        print_ll(mdai->moduleDef, depth + 1, fp);
        break;
    }
    case DRIVER_MODULE_AST:
    {
        fprintf(fp, "DRIVER_MODULE_AST\n\n");
        fflush(fp);
        print_ll(node->aux_info, depth + 1, fp); // this is a LLASTNode
        break;
    }
    case RANGE_AST:
    {
        fprintf(fp, "RANGE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case GET_VALUE_AST:
    {
        fprintf(fp, "GET_VALUE_AST\n\n");
        fflush(fp);
        break;
    }
    case PRINT_AST:
    {
        fprintf(fp, "PRINT_AST\n\n");
        fflush(fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case EQUALS_AST:
    {
        fprintf(fp, "EQUALS_AST\n\n");
        fflush(fp);
        if (node->right->nodeType == USE_AST)
        {
            print_ll_pt(node->left, depth + 1, fp);
        }
        else if (node->left->nodeType == ARR_ELEM_AST)
        {
            print_ast_node(node->left, depth + 1, fp);
        }
        else
        {
            print_parse_tree_node(node->left, depth + 1, fp);
        }
        if (node->right->nodeType == USE_AST)
        {
            print_ast_node(node->right, depth + 1, fp);
        }
        else if (node->right->nodeType >= PLUS_AST && node->right->nodeType <= GE_AST)
        {
            print_ast_node(node->right, depth + 1, fp);
        }
        else
        {
            if (node->right != NULL)
            {
                print_ast_node(node->right, depth + 1, fp);
            }
        }
        break;
    }
    case USE_AST:
    {
        fprintf(fp, "USE_AST\n\n");
        fflush(fp);
        print_parse_tree_node(node->left, depth + 1, fp);
        LinkedListASTNode *ll = (LinkedListASTNode *)node->right;
        print_ll(ll, depth + 1, fp);
        break;
    }
    case DECLARE_AST:
    {
        fprintf(fp, "DECLARE_AST\n\n");
        fflush(fp);
        print_ll_pt(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case SWITCH_AST:
    {
        fprintf(fp, "SWITCH_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ll(node->right, depth + 1, fp);
        break;
    }
    case CASE_AST:
    {
        fprintf(fp, "CASE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ll(node->right, depth + 1, fp);
        break;
    }
    case DEFAULT_AST:
    {
        fprintf(fp, "DEFAULT_AST\n\n");
        fflush(fp);
        print_ll(node->right, depth + 1, fp);
        break;
    }
    case FOR_AST:
    {
        fprintf(fp, "FOR_AST\n\n");
        fflush(fp);
        print_ast_node(node->aux_info, depth + 1, fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ll(node->right, depth + 1, fp);
        break;
    }
    case WHILE_AST:
    {
        fprintf(fp, "WHILE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ll(node->right, depth + 1, fp);
        break;
    }
    case PLUS_AST:
    {
        fprintf(fp, "PLUS_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case MINUS_AST:
    {
        fprintf(fp, "MINUS_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case MUL_AST:
    {
        fprintf(fp, "MUL_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case DIV_AST:
    {
        fprintf(fp, "DIV_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case AND_AST:
    {
        fprintf(fp, "AND_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case OR_AST:
    {
        fprintf(fp, "OR_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case EQ_AST:
    {
        fprintf(fp, "EQ_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case NE_AST:
    {
        fprintf(fp, "NE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case LT_AST:
    {
        fprintf(fp, "LT_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case LE_AST:
    {
        fprintf(fp, "LE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case GT_AST:
    {
        fprintf(fp, "GT_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case GE_AST:
    {
        fprintf(fp, "GE_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case LINKED_LIST_AST:
    {
        fprintf(fp, "LINKED_LIST_AST\n\n");
        fflush(fp);
        // print_ast_node(node->next, depth, fp);
        break;
    }
    case INTEGER_AST:
    {
        fprintf(fp, "INTEGER_AST\n\n");
        fflush(fp);
        break;
    }
    case REAL_AST:
    {
        fprintf(fp, "REAL_AST\n\n");
        fflush(fp);
        break;
    }
    case BOOLEAN_AST:
    {
        fprintf(fp, "BOOLEAN_AST\n\n");
        fflush(fp);
        break;
    }
    case TRUE_PTN_AST:
    {
        fprintf(fp, "TRUE_PTN_AST\n\n");
        fflush(fp);
        break;
    }
    case FALSE_PTN_AST:
    {
        fprintf(fp, "FALSE_PTN_AST\n\n");
        fflush(fp);
        break;
    }
    case ARRAY_AST:
    {
        fprintf(fp, "ARRAY_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case FORMALPARAM_AST:
    {
        fprintf(fp, "FORMALPARAM_AST\n\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case ARR_ELEM_AST:
    {
        fprintf(fp, "ARR_ELEM_AST\n");
        fflush(fp);
        print_ast_node(node->left, depth + 1, fp);
        print_ast_node(node->right, depth + 1, fp);
        break;
    }
    case PARSE_TREE_AST:
    case PARSE_TREE_AST1:
    case PARSE_TREE_AST2:
    {
        print_parse_tree_node(node, depth + 1, fp);
        break;
    }
    default:
    {
        fprintf(fp, "UNKNOWN_AST\n\n");
        fflush(fp);
        break;
    }
    }
}

void print_ast(ast *a)
{
    FILE *fp = stdout;
    fprintf(fp, "AST\n\n");
    fflush(fp);
    ast_node *root0 = a->root;
    print_ast_node(root0, 0, fp);
    // fclose(fp);
}

// int main()
// {
//     bufferSize = 1024;
//     parseInputSourceCode("tests/stage2/t7.txt", "src/parser/parseTree.txt");
//     printf("parse tree created successfully.\n");
//     fflush(stdout);
//     ast *AST = create_ast(&parseTree);
//     printf("AST created successfully.\n");
//     print_ast(AST);
//     printf("AST printed successfully.\n");  
//     fflush(stdout);
//     // populate symbol tables
//     populateSymbolTables(AST);
//     printf("Nah. No way.\n");
//     fflush(stdout);
//     return 0;
// }
