#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

// create a new symbol table with the given parent and name
symbol_table *createSymbolTable(symbol_table *parent, char *name)
{
    symbol_table *newTable = (symbol_table *)malloc(sizeof(symbol_table));
    newTable->parent = parent;
    newTable->name = name;
    return newTable;
}

// hash function for the symbol table
int hash(char *name)
{
    int multiplier = 769;
    int hash = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        hash = (hash * multiplier + name[i]) % SYMBOL_TABLE_SIZE;
    }
    return hash;
}

// add a new entry to a given symbol table
void addToSymbolTable(symbol_table *symTab, ST_ENTRY *st_entry)
{
    int hashVal = hash(st_entry->name);
    ST_LL *newNode = (ST_LL *)malloc(sizeof(ST_LL));
    newNode->next = symTab->data[hashVal];
    newNode->data = st_entry;
    symTab->data[hashVal] = newNode;
}

// check if a given symbol table contains a given name
ST_ENTRY *checkSymbolTable(symbol_table *symTab, char *name)
{
    int hashVal = hash(name);
    ST_LL *node = symTab->data[hashVal];
    while (node != NULL)
    {
        if (strcmp(node->data->name, name) == 0)
        {
            return node->data;
        }
        node = node->next;
    }
    return NULL;
}

// check if a given symbol table or any of its parents contain a given name
ST_ENTRY *checkAllSymbolTables(symbol_table *symTab, char *name)
{
    ST_ENTRY *node = checkSymbolTable(symTab, name);
    if (node != NULL)
    {
        return node;
    }
    if (symTab->parent != NULL)
    {
        return checkAllSymbolTables(symTab->parent, name);
    }
    return NULL;
}

// specifically written to check for recursion in module definitions
int checkRecursion(symbol_table *symTab, char *name)
{
    if (strcmp(symTab->name, name) == 0)
    {
        return 1;
    }
    if (symTab->parent == NULL)
    {
        return 0;
    }
    return checkRecursion(symTab->parent, name);
}

// add a new entry to a linked list of symbol table entries
ST_LL *addToST_LL(ST_LL *head, ST_ENTRY *st_entry)
{
    ST_LL *newNode = (ST_LL *)malloc(sizeof(ST_LL));
    newNode->data = st_entry;
    newNode->next = NULL;
    if (head == NULL)
    {
        return newNode;
    }
    ST_LL *node = head;
    while (node->next != NULL)
    {
        node = node->next;
    }
    node->next = newNode;
    return head;
}

ST_ENTRY *addVarToSymTable(symbol_table *symTab, parse_tree_node *var_name, ast_node *var_type)
{
    ST_ENTRY *st_entry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    st_entry->name = var_name->leafNodeInfo.lexeme;
    if (checkSymbolTable(symTab, st_entry->name) != NULL)
    {
        printf("Error: Variable %s found at line number %d has already been declared before.\n",
               st_entry->name, var_name->leafNodeInfo.lineNumber);
        exit(1);
    }
    st_entry->entry_type = VAR_SYM;
    switch (var_type->nodeType)
    {
    case INTEGER_AST:
        st_entry->data.var->type = __NUM__;
        break;
    case REAL_AST:
        st_entry->data.var->type = __RNUM__;
        break;
    case BOOLEAN_AST:
        st_entry->data.var->type = __BOOL__;
        break;
    case ARRAY_AST:
        st_entry->entry_type = ARR_SYM;
        // eltype
        switch (var_type->right->nodeType)
        {
        case INTEGER_AST:
            st_entry->data.arr->type = __NUM__;
            break;
        case REAL_AST:
            st_entry->data.arr->type = __RNUM__;
            break;
        case BOOLEAN_AST:
            st_entry->data.arr->type = __BOOL__;
            break;
        }
        // static or dynamic array
        ast_node *range = var_type->left;
        int lindexSign = range->left->nodeType == MINUS_AST ? -1 : 1;
        int rindexSign = range->right->nodeType == MINUS_AST ? -1 : 1;
        parse_tree_node *lindex = lindexSign == -1 ? range->left->right : range->left;
        parse_tree_node *rindex = rindexSign == -1 ? range->right->right : range->right;
        if (lindex->leafNodeInfo.tokenID == NUM && rindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->arrayType = STATIC;
        }
        else
        {
            st_entry->data.arr->arrayType = DYNAMIC;
        }
        if (lindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->start.staticIndex = lindexSign * lindex->leafNodeInfo.val.intValue;
        }
        else
        {
            ST_ENTRY *lindexEntry = checkAllSymbolTables(&symbolTable, lindex->leafNodeInfo.lexeme);
            // TODO check if this error is too harsh
            if (lindexEntry == NULL)
            {
                printf("Error: Variable %s not declared at line number %d.\n",
                       lindex->leafNodeInfo.lexeme, lindex->leafNodeInfo.lineNumber);
                exit(1);
            }
            else
            {
                st_entry->data.arr->start.dynamicIndex->index = lindexEntry->data.var;
                st_entry->data.arr->start.dynamicIndex->sign = lindexSign;
            }
        }
        if (rindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->end.staticIndex = rindexSign * rindex->leafNodeInfo.val.intValue;
        }
        else
        {
            ST_ENTRY *rindexEntry = checkAllSymbolTables(&symbolTable, rindex->leafNodeInfo.lexeme);
            // TODO check if this error is too harsh
            if (rindexEntry == NULL)
            {
                printf("Error: Variable %s not declared at line number %d.\n",
                       rindex->leafNodeInfo.lexeme, rindex->leafNodeInfo.lineNumber);
                exit(1);
            }
            else
            {
                st_entry->data.arr->end.dynamicIndex->index = rindexEntry->data.var;
                st_entry->data.arr->end.dynamicIndex->sign = rindexSign;
            }
        }
        break;
    }
    addToSymbolTable(symTab, st_entry);
    return st_entry;
}

// add a list of parameters to a given symbol table
ST_LL *addParamListToFuncSymTable(symbol_table *funcSymTable, LinkedListASTNode *plist)
{
    ST_LL *params = NULL;
    while (plist != NULL)
    {
        ST_ENTRY *input_st_entry = addVarToSymTable(funcSymTable, plist->data->left, plist->data->right);
        params = addToST_LL(params, input_st_entry);
        plist = plist->next;
    }
    return params;
}

// global symbol table
symbol_table symbolTable = {NULL, "42"};

// check if all variables used in an expression are already in the symbol table
void checkExpressionNames(ast_node *expr, symbol_table *symTable)
{
    // null check
    if (expr == NULL)
    {
        return;
    }
    // operators or array element
    if ((expr->nodeType >= PLUS_AST && expr->nodeType <= GE_AST) || expr->nodeType == ARR_ELEM_AST)
    {
        checkExpressionNames(expr->left, symTable);
        checkExpressionNames(expr->right, symTable);
    }
    // identifiers
    else if (expr->nodeType == ID)
    {
        if (checkAllSymbolTables(symTable, expr->leafNodeInfo.lexeme) == NULL)
        {
            printf("Error: Declaration of variable %s used at line number %d not found.\n",
                   expr->leafNodeInfo.lexeme, expr->leafNodeInfo.lineNumber);
            exit(1);
        }
    }
}

void populateBlockSymbolTables(LinkedListASTNode *stmts, symbol_table *blockSymTable)
{
    while (stmts != NULL)
    {
        parse_tree_node *Id;
        ast_node *stmt = stmts->data;
        // extract statement info from the AST node
        switch (stmt->nodeType)
        {
        case GET_VALUE_AST:
        case PRINT_AST:
        {
            Id = stmt->right;
            if (checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme) == NULL)
            {
                printf("Error: Variable %s not declared at line number %d\n", Id->leafNodeInfo.lexeme,
                       Id->leafNodeInfo.lineNumber);
            }
            break;
        }
        case EQUALS_AST:
        {
            if (stmt->right->nodeType == USE_AST)
            {
                Id = stmt->right->left;
                if (checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme) == NULL)
                {
                    printf("Error: Variable %s not declared at line number %d\n", Id->leafNodeInfo.lexeme,
                           Id->leafNodeInfo.lineNumber);
                }
                // check recursion and throw error if detected
                if (checkRecursion(blockSymTable, Id->leafNodeInfo.lexeme))
                {
                    printf("Error: Recursion detected in module %s at line number %d. Please check the code and try again.\n",
                           Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                    exit(1);
                }
                // Check if actual output parameters are declared
                LinkedListASTNode *list = stmt->left;
                while (list != NULL)
                {
                    checkExpressionNames(list->data, blockSymTable);
                }
            }
            // check an expression
            else
            {
                checkExpressionNames(stmt->right, blockSymTable);
                checkExpressionNames(stmt->left, blockSymTable);
            }
            break;
        }
        // declare a variable
        case DECLARE_AST:
        {
            LinkedListASTNode *var_list = stmt->left;
            while (var_list != NULL)
            {
                ast_node *var_node = var_list->data;
                ST_ENTRY *var_st_entry = addVarToSymTable(blockSymTable, var_node, stmt->right);
                var_list = var_list->next;
            }
        }
        // switching
        case SWITCH_AST:
        {
            // switching variable
            checkExpressionNames(stmt->left, blockSymTable);
            LinkedListASTNode *case_list = stmt->right;
            // case counter
            int i = 0;
            // case blocks
            while (case_list != NULL)
            {
                LinkedListASTNode *stmt_list = case_list->data->right;
                // create a new symbol table for each case block and 
                // add it to the parent symbol table. set the name of the symbol table
                // to the parent's name + case number
                char *casestr = malloc(sizeof(char) * 64);
                sprintf(casestr, "case_%d", i);
                symbol_table *caseSymTable = createSymbolTable(blockSymTable, casestr);
                // create an entry for the case symbol table in the parent symbol table
                ST_ENTRY *caseSymTableEntry = malloc(sizeof(ST_ENTRY));
                caseSymTableEntry->name = casestr;
                caseSymTableEntry->entry_type = BLOCK_SYM;
                caseSymTableEntry->data.block->body = stmt_list;
                caseSymTableEntry->data.block->symTable = caseSymTable;
                populateBlockSymbolTables(stmt_list, caseSymTable);
                case_list = case_list->next;
                i++;
            }
            break;
        }
        }
        stmts = stmts->next;
    }
}

// traverse the ASTree and populate the symbol table
void populateSymbolTables(ast *ASTree)
{
    // extract programAuxInfo from the AST
    struct programAuxInfo *programAux = (struct programAuxInfo *)ASTree->root->aux_info;
    ast_node *moduleDeclaration = programAux->ModDec;
    ast_node *otherModules1 = programAux->OtherMod1;
    ast_node *driverModule = programAux->DriverMod;
    ast_node *otherModules2 = programAux->OtherMod2;
    // traverse the linked list, adding module declarations to the symbol table
    LinkedListASTNode *node = moduleDeclaration;
    while (node != NULL)
    {
        parse_tree_node *Id = node->data->aux_info;
        ST_ENTRY *moduledec_st_entry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
        moduledec_st_entry->name = Id->leafNodeInfo.lexeme;
        moduledec_st_entry->entry_type = FUNC_SYM;
        moduledec_st_entry->data.func = NULL;
        addToSymbolTable(&symbolTable, moduledec_st_entry);
        node = node->next;
    }
    // traverse the linked list, adding other modules to the symbol table
    node = otherModules1;
    while (node != NULL)
    {
        // extract module info from the AST node
        struct moduleDefAuxInfo *mdai = (struct moduleDefAuxInfo *)node->data->aux_info;
        parse_tree_node *Id = mdai->Id;
        LinkedListASTNode *input_plist = mdai->input_plist;
        LinkedListASTNode *ret = mdai->ret;
        LinkedListASTNode *moduleDef = mdai->moduleDef;
        ST_ENTRY *symTabEntryForModule;
        // module already exists in the symbol table
        if (symTabEntryForModule = checkSymbolTable(&symbolTable, Id->leafNodeInfo.lexeme) != NULL)
        {
            // ensure type of symbol table entry is a function
            // TODO check if these errors are fine
            if (symTabEntryForModule->entry_type != FUNC_SYM)
            {
                printf("Error: Name %s at line number %d does not seem to be a function or might be clashing\
                with a variable of the same name. Please check the code and try again.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                exit(1);
            }
            // module already has a definition
            else if (symTabEntryForModule->data.func != NULL)
            {
                printf("Error: Function %s at line number %d seems to already have been defined. Please check\
                the code and try again.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                exit(1);
            }
        }
        // module does not exist in the symbol table
        else
        {
            // create a new symbol table entry for the module
            symTabEntryForModule = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
            symTabEntryForModule->name = Id->leafNodeInfo.lexeme;
            symTabEntryForModule->entry_type = FUNC_SYM;
            symTabEntryForModule->data.func = NULL;
            addToSymbolTable(&symbolTable, symTabEntryForModule);
        }
        symbol_table *funcSymTable = createSymbolTable(&symbolTable, symTabEntryForModule->name);
        // populate the symbol table with the input parameters
        symTabEntryForModule->data.func->inputs = addParamListToFuncSymTable(funcSymTable, input_plist);
        symTabEntryForModule->data.func->outputs = addParamListToFuncSymTable(funcSymTable, ret);
        symTabEntryForModule->data.func->body = moduleDef;
        // function body is a linked list of statements
        // traverse the linked list, adding declarations to the symbol table
        LinkedListASTNode *stmt = moduleDef;
        // TODO call the block function
        node = node->next;
    }
}