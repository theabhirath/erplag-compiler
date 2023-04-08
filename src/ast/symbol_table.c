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
    // zero out the data
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        newTable->data[i] = NULL;
    }
    newTable->offset = 0;
    printf("Created symbol table %s.\n", name);
    return newTable;
}

// hash function for the symbol table
int hashSymTable(char *name)
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
    int hashVal = hashSymTable(st_entry->name);
    ST_LL *newNode = (ST_LL *)malloc(sizeof(ST_LL));
    newNode->next = symTab->data[hashVal];
    newNode->data = st_entry;
    symTab->data[hashVal] = newNode;
}

// check if a given symbol table contains a given name
ST_ENTRY *checkSymbolTable(symbol_table *symTab, char *name)
{
    printf("Checking symbol table %s for %s\n", symTab->name, name);
    int hashVal = hashSymTable(name);
    ST_LL *node = symTab->data[hashVal];
    while (node != NULL)
    {
        if (strcmp(node->data->name, name) == 0)
        {
            return node->data;
        }
        node = node->next;
    }
    printf("Not found in symbol table %s\n", symTab->name);
    return NULL;
}

// check if a given symbol table or any of its parents contain a given name
ST_ENTRY *checkAllSymbolTables(symbol_table *symTab, char *name)
{
    printf("Checking symbol tables above %s for %s\n", symTab->name, name);
    fflush(stdout);
    ST_ENTRY *node = checkSymbolTable(symTab, name);
    if (node != NULL)
    {
        printf("Found %s in symbol table %s\n", name, symTab->name);
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
    printf("Adding variable %s to symbol table %s\n", var_name->leafNodeInfo.lexeme, symTab->name);
    ST_ENTRY *st_entry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    st_entry->name = var_name->leafNodeInfo.lexeme;
    if (checkSymbolTable(symTab, st_entry->name) != NULL)
    {
        printf("Error: Variable %s found at line number %d has already been declared before.\n",
               st_entry->name, var_name->leafNodeInfo.lineNumber);
        exit(1);
    }
    st_entry->entry_type = VAR_SYM;
    print_ast_node(var_type, 0, stdout);
    switch (var_type->nodeType)
    {
    case INTEGER_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __NUM__;
        break;
    case REAL_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __RNUM__;
        break;
    case BOOLEAN_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __BOOL__;
        break;
    case ARRAY_AST:
        st_entry->data.arr = (struct arr_entry *)malloc(sizeof(struct arr_entry));
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
        printf("lindex: %s, rindex: %s\n", lindex->leafNodeInfo.lexeme, rindex->leafNodeInfo.lexeme);
        fflush(stdout);
        if (lindex->leafNodeInfo.tokenID == NUM && rindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->arrayType = STATIC;
        }
        else
        {
            st_entry->data.arr->arrayType = DYNAMIC;
            st_entry->data.arr->start.dynamicIndex = (struct dynamic_index *)malloc(sizeof(struct dynamicIndex));
            st_entry->data.arr->end.dynamicIndex = (struct dynamic_index *)malloc(sizeof(struct dynamicIndex));
        }
        if (lindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->start.staticIndex = lindexSign * lindex->leafNodeInfo.val.intValue;
        }
        else
        {
            ST_ENTRY *lindexEntry = checkAllSymbolTables(symTab, lindex->leafNodeInfo.lexeme);
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
            ST_ENTRY *rindexEntry = checkAllSymbolTables(symTab, rindex->leafNodeInfo.lexeme);
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
                printf("DD\n");
                fflush(stdout);
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
    printf("Adding parameter list to function symbol table\n");
    fflush(stdout);
    ST_LL *params = NULL;
    while (plist != NULL)
    {
        ST_ENTRY *input_st_entry = addVarToSymTable(funcSymTable, plist->data->left, plist->data->right);
        printf("Added parameter %s to function symbol table\n", input_st_entry->name);
        fflush(stdout);
        params = addToST_LL(params, input_st_entry);
        printf("Added parameter %s to parameter list\n", input_st_entry->name);
        fflush(stdout);
        plist = plist->next;
    }
    return params;
}

// global symbol table
symbol_table symbolTable = {"42", NULL, NULL, 0};

// check if all variables used in an expression are already in the symbol table
void checkExpressionNames(ast_node *expr, symbol_table *symTable)
{
    printf("Checking expression names\n");
    // null check
    if (expr == NULL)
    {
        printf("Null expression\n");
        fflush(stdout);
        return;
    }
    // operators or array element
    if ((expr->nodeType >= PLUS_AST && expr->nodeType <= GE_AST) || expr->nodeType == ARR_ELEM_AST)
    {
        printf("Haan main operator hu\n");
        fflush(stdout);
        checkExpressionNames(expr->left, symTable);
        checkExpressionNames(expr->right, symTable);
    }
    // identifiers
    else if (expr->nodeType == ID)
    {
        parse_tree_node *pt = (parse_tree_node *)expr;
        if (checkAllSymbolTables(symTable, pt->leafNodeInfo.lexeme) == NULL)
        {
            printf("Error: Declaration of variable %s used at line number %d not found.\n",
                   pt->leafNodeInfo.lexeme, pt->leafNodeInfo.lineNumber);
            exit(1);
        }
        printf("Haan main ID hu\n");
        fflush(stdout);
    }
}

// populate the symbol table for a block
void populateBlockSymbolTables(LinkedListASTNode *stmts, symbol_table *blockSymTable)
{
    printf("Populating symbol table for block %s.\n", blockSymTable->name);
    fflush(stdout);
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
            printf("Haan main get_value/print hu\n");
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
                    list = list->next;
                }
            }
            // check an expression
            else
            {
                checkExpressionNames(stmt->right, blockSymTable);
                checkExpressionNames(stmt->left, blockSymTable);
                printf("We have completed equals ast\n");
                fflush(stdout);
            }
            break;
        }
        // declare a variable
        case DECLARE_AST:
        {
            LinkedListASTNode *var_list = stmt->left;
            while (var_list != NULL)
            {
                parse_tree_node *var_node = var_list->data; 
                printf("Adding variable %s to symbol table.\n", var_node->leafNodeInfo.lexeme);
                fflush(stdout);
                ST_ENTRY *var_st_entry = addVarToSymTable(blockSymTable, var_node, stmt->right);
                printf("Added variable %s to symbol table.\n", var_node->leafNodeInfo.lexeme);
                fflush(stdout);
                var_list = var_list->next;
            }
            break;
        }
        // switching
        case SWITCH_AST:
        {
            printf("We have a switch statement\n");
            fflush(stdout);
            // switching variable
            checkExpressionNames(stmt->left, blockSymTable);
            LinkedListASTNode *case_list = stmt->right;
            // case blocks
            while (case_list != NULL)
            {
                printf("We have a case block\n");
                if (case_list->data == NULL)
                {
                    printf("We don't have a default block\n");
                    fflush(stdout);
                    break;
                }
                LinkedListASTNode *stmt_list = case_list->data->right;
                // create a new symbol table for each case block and
                // add it to the parent symbol table. set the name of the symbol table
                // to the parent's name + case number
                char *casestr = malloc(sizeof(char) * 64);
                sprintf(casestr, "%p", case_list->data);
                symbol_table *caseSymTable = createSymbolTable(blockSymTable, casestr);
                // create an entry for the case symbol table in the parent symbol table
                ST_ENTRY *caseSymTableEntry = malloc(sizeof(ST_ENTRY));
                caseSymTableEntry->name = casestr;
                caseSymTableEntry->entry_type = BLOCK_SYM;
                caseSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
                caseSymTableEntry->data.block->body = stmt_list;
                caseSymTableEntry->data.block->symTable = caseSymTable;
                populateBlockSymbolTables(stmt_list, caseSymTable);
                addToSymbolTable(blockSymTable, caseSymTableEntry);
                case_list = case_list->next;
                printf("We have completed case ast\n");
            }
            printf("We have completed switch ast\n");
            break;
        }
        // for loop
        case FOR_AST:
        {
            // loop variable
            parse_tree_node *loopVar = stmt->aux_info;
            printf("We have a for loop with loop variable %s\n", loopVar->leafNodeInfo.lexeme);
            // list of statements in the for loop
            LinkedListASTNode *for_stmt_list = stmt->right;
            char *forstr = malloc(sizeof(char) * 64);
            sprintf(forstr, "%p", stmt);
            symbol_table *forSymTable = createSymbolTable(blockSymTable, forstr);
            // add the loop variable to the symbol table
            ST_ENTRY *loopVarEntry = malloc(sizeof(ST_ENTRY));
            loopVarEntry->name = loopVar->leafNodeInfo.lexeme;
            loopVarEntry->entry_type = VAR_SYM;
            loopVarEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            loopVarEntry->data.var->type = __NUM__; // can only be an integer
            addToSymbolTable(forSymTable, loopVarEntry);
            // create an entry for the for symbol table in the parent symbol table
            ST_ENTRY *forSymTableEntry = malloc(sizeof(ST_ENTRY));
            forSymTableEntry->name = forstr;
            forSymTableEntry->entry_type = BLOCK_SYM;
            forSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
            forSymTableEntry->data.block->body = for_stmt_list;
            forSymTableEntry->data.block->symTable = forSymTable;
            populateBlockSymbolTables(for_stmt_list, forSymTable);
            addToSymbolTable(blockSymTable, forSymTableEntry);
            break;
        }
        // while loop
        case WHILE_AST:
        {
            // condition in the while loop
            checkExpressionNames(stmt->left, blockSymTable);
            // list of statements in the while loop
            LinkedListASTNode *while_stmt_list = stmt->right;
            char *whilestr = malloc(sizeof(char) * 64);
            sprintf(whilestr, "%p", stmt);
            symbol_table *whileSymTable = createSymbolTable(blockSymTable, whilestr);
            // create an entry for the while symbol table in the parent symbol table
            ST_ENTRY *whileSymTableEntry = malloc(sizeof(ST_ENTRY));
            whileSymTableEntry->name = whilestr;
            whileSymTableEntry->entry_type = BLOCK_SYM;
            whileSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
            whileSymTableEntry->data.block->body = while_stmt_list;
            whileSymTableEntry->data.block->symTable = whileSymTable;
            populateBlockSymbolTables(while_stmt_list, whileSymTable);
            addToSymbolTable(blockSymTable, whileSymTableEntry);
            break;
        }
        // TODO maybe remove this error if debugging works out
        default:
        {
            printf("Block issues still haunt you.\n");
            break;
        }
        }
        stmts = stmts->next;
    }
}

void populateOtherModulesSymbolTables(LinkedListASTNode *node)
{
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
        if ((symTabEntryForModule = checkSymbolTable(&symbolTable, Id->leafNodeInfo.lexeme)) != NULL)
        {
            printf("Module %s already exists in the symbol table\n", Id->leafNodeInfo.lexeme);
            fflush(stdout);
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
            printf("Module %s does not have a definition yet\n", Id->leafNodeInfo.lexeme);
            fflush(stdout);
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
        symTabEntryForModule->data.func = (struct func_entry *)malloc(sizeof(struct func_entry));
        symTabEntryForModule->data.func->inputs = addParamListToFuncSymTable(funcSymTable, input_plist);
        symTabEntryForModule->data.func->outputs = addParamListToFuncSymTable(funcSymTable, ret);
        symTabEntryForModule->data.func->body = moduleDef;
        symTabEntryForModule->data.func->symTable = funcSymTable;
        // function body is a block
        LinkedListASTNode *stmts = moduleDef;
        populateBlockSymbolTables(stmts, funcSymTable);
        node = node->next;
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
    // traverse the linked list, adding other modules (1) to the symbol table
    populateOtherModulesSymbolTables(otherModules1);
    printf("Hello. It's me before driver\n");
    fflush(stdout);
    // driver module (aux_info is a linked list of statements)
    symbol_table *driverSymTable = createSymbolTable(&symbolTable, "driver");
    LinkedListASTNode *driverStmts = driverModule->aux_info;
    ST_ENTRY *driverSymTableEntry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    driverSymTableEntry->name = "driver";
    driverSymTableEntry->entry_type = BLOCK_SYM;
    driverSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
    driverSymTableEntry->data.block->body = driverStmts;
    driverSymTableEntry->data.block->symTable = driverSymTable;
    populateBlockSymbolTables(driverStmts, driverSymTable);
    addToSymbolTable(&symbolTable, driverSymTableEntry);
    printf("Hello. It's me after driver\n");
    fflush(stdout);
    // traverse the linked list, adding other modules (2) to the symbol table
    populateOtherModulesSymbolTables(otherModules2);
}