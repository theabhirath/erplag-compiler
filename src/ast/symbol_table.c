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
    newTable->offset = parent == NULL ? 0 : parent->offset;
    printf("Created symbol table %s.\n", name);
    return newTable;
}

// update offset
void updateOffset(symbol_table *symTab, int offset)
{
    // Update offsets of all symbol tables
    // above the current symbol table
    symbol_table *curr = symTab;
    while (curr != NULL)
    {
        curr->offset = offset;
        curr = curr->parent;
    }
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

// add a variable to the symbol table given the variable name and type
ST_ENTRY *addVarToSymTable(symbol_table *symTab, parse_tree_node *var_name, ast_node *var_type)
{
    printf("Adding variable %s to symbol table %s\n", var_name->leafNodeInfo.lexeme, symTab->name);
    ST_ENTRY *st_entry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    st_entry->name = var_name->leafNodeInfo.lexeme;
    if (checkSymbolTable(symTab, st_entry->name) != NULL)
    {
        printf("Error: Variable %s found at line number %d has already been declared before.\n",
               st_entry->name, var_name->leafNodeInfo.lineNumber);
        return NULL;
    }
    st_entry->entry_type = VAR_SYM;
    print_ast_node(var_type, 0, stdout);
    switch (var_type->nodeType)
    {
    case INTEGER_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __NUM__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __NUM_SIZE__;
        break;
    case REAL_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __RNUM__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __RNUM_SIZE__;
        break;
    case BOOLEAN_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->type = __BOOL__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __BOOL_SIZE__;
        break;
    case ARRAY_AST:
        st_entry->data.arr = (struct arr_entry *)malloc(sizeof(struct arr_entry));
        st_entry->entry_type = ARR_SYM;
        // eltype
        switch (var_type->right->nodeType)
        {
        case INTEGER_AST:
            st_entry->data.arr->eltype = __NUM__;
            break;
        case REAL_AST:
            st_entry->data.arr->eltype = __RNUM__;
            break;
        case BOOLEAN_AST:
            st_entry->data.arr->eltype = __BOOL__;
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
        // static or dynamic array
        if (lindex->leafNodeInfo.tokenID == NUM && rindex->leafNodeInfo.tokenID == NUM)
        {
            int lindexVal = lindexSign * lindex->leafNodeInfo.val.intValue;
            int rindexVal = rindexSign * rindex->leafNodeInfo.val.intValue;
            // check if lindexVal < rindexVal
            if (lindexVal >= rindexVal)
            {
                printf("Error: Array %s at line number %d has invalid range.\n",
                       st_entry->name, var_name->leafNodeInfo.lineNumber);
                st_entry->data.arr->eltype = INDEX_ERROR;
                return st_entry;
            }
            else
            {
                // early return since we know the array is static
                st_entry->data.arr->arrayType = STATIC;
                st_entry->data.arr->left.staticIndex = lindexVal;
                st_entry->data.arr->right.staticIndex = rindexVal;
                st_entry->data.arr->offset = symTab->offset;
                symTab->offset += (rindexVal - lindexVal + 1) * (st_entry->data.arr->eltype == __NUM__ ? __NUM_SIZE__ : st_entry->data.arr->eltype == __RNUM__ ? __RNUM_SIZE__
                                                                                                                                                               : __BOOL_SIZE__);
                addToSymbolTable(symTab, st_entry);
                printf("Added static array %s to symbol table %s\n", st_entry->name, symTab->name);
                printf("left: %d, right: %d\n", st_entry->data.arr->left.staticIndex, st_entry->data.arr->right.staticIndex);
                printf("eltype: %d\n", st_entry->data.arr->eltype);
                printf("arrayType: %d\n", st_entry->data.arr->arrayType);
                fflush(stdout);
                return st_entry;
            }
        }
        ST_ENTRY *lindexEntry = checkAllSymbolTables(symTab, lindex->leafNodeInfo.lexeme);
        ST_ENTRY *rindexEntry = checkAllSymbolTables(symTab, rindex->leafNodeInfo.lexeme);
        if (lindex->leafNodeInfo.tokenID == ID || rindex->leafNodeInfo.tokenID == ID)
        {
            // type checking for the variables
            if (lindex->leafNodeInfo.tokenID == ID)
            {
                if (lindexEntry == NULL)
                {
                    printf("Error: Variable %s not declared at line number %d.\n",
                           lindex->leafNodeInfo.lexeme, lindex->leafNodeInfo.lineNumber);
                    st_entry->data.arr->eltype = INDEX_ERROR;
                }
                else if (lindexEntry->entry_type != VAR_SYM || lindexEntry->data.var->type != __NUM__)
                {
                    printf("Error: Variable %s at line number %d is not of type integer.\n",
                           lindex->leafNodeInfo.lexeme, lindex->leafNodeInfo.lineNumber);
                    st_entry->data.arr->eltype = INDEX_ERROR;
                }
            }
            if (rindex->leafNodeInfo.tokenID == ID)
            {
                if (rindexEntry == NULL)
                {
                    printf("Error: Variable %s not declared at line number %d.\n",
                           rindex->leafNodeInfo.lexeme, rindex->leafNodeInfo.lineNumber);
                    st_entry->data.arr->eltype = INDEX_ERROR;
                }
                else if (rindexEntry->entry_type != VAR_SYM || rindexEntry->data.var->type != __NUM__)
                {
                    printf("Error: Variable %s at line number %d is not of type integer.\n",
                           rindex->leafNodeInfo.lexeme, rindex->leafNodeInfo.lineNumber);
                    st_entry->data.arr->eltype = INDEX_ERROR;
                }
            }
            st_entry->data.arr->arrayType = DYNAMIC;
        }
        else
        {
            printf("Array indices of array %s at line number %d are not valid.\n",
                   st_entry->name, var_name->leafNodeInfo.lineNumber);
            st_entry->data.arr->eltype = INDEX_ERROR;
        }
        // assign values to the indices
        if (lindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->left.staticIndex = lindexSign * lindex->leafNodeInfo.val.intValue;
        }
        else
        {
            st_entry->data.arr->left.dynamicIndex = (struct dynamic_index *)malloc(sizeof(struct dynamicIndex));
            st_entry->data.arr->left.dynamicIndex->index = lindexEntry->data.var;
            st_entry->data.arr->left.dynamicIndex->sign = lindexSign;
        }
        if (rindex->leafNodeInfo.tokenID == NUM)
        {
            st_entry->data.arr->right.staticIndex = rindexSign * rindex->leafNodeInfo.val.intValue;
        }
        else
        {
            st_entry->data.arr->right.dynamicIndex = (struct dynamic_index *)malloc(sizeof(struct dynamicIndex));
            st_entry->data.arr->right.dynamicIndex->index = rindexEntry->data.var;
            st_entry->data.arr->right.dynamicIndex->sign = rindexSign;
        }
        st_entry->data.arr->offset = symTab->offset;
        symTab->offset += __DYNAMIC_ARRAY_SIZE__;
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
        ST_ENTRY *check = checkSymbolTable(funcSymTable, input_st_entry->name);
        printf("Checking if parameter %s was added to function %s symbol table successfully\n", input_st_entry->name, funcSymTable->name);
        fflush(stdout);
        printf("Check: %d\n", NULL == check);
        printf("Added parameter %s to function %s symbol table\n", input_st_entry->name, funcSymTable->name);
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

// match array types
int matchArrayTypes(struct arr_entry *arr1, struct arr_entry *arr2)
{
    printf("Trying to match array types\n");
    if (arr1->eltype != arr2->eltype || arr1->arrayType != arr2->arrayType ||
        arr1->arrayType == INDEX_ERROR || arr2->arrayType == INDEX_ERROR)
    {
        printf("Array element or array types do not match, or there's an index error\n");
        printf("arr1 eltype: %d, arr2 eltype: %d, arr1 arrayType: %d, arr2 arrayType: %d\n",
               arr1->eltype, arr2->eltype, arr1->arrayType, arr2->arrayType);
        return 1;
    }
    if (arr1->arrayType == STATIC)
    {
        if (arr1->left.staticIndex != arr2->left.staticIndex ||
            arr1->right.staticIndex != arr2->right.staticIndex)
        {
            printf("Static array indices do not match\n");
            printf("arr1 left: %d, arr1 right: %d, arr2 left: %d, arr2 right: %d\n",
                   arr1->left.staticIndex, arr1->right.staticIndex, arr2->left.staticIndex, arr2->right.staticIndex);
            return 1;
        }
    }
    else
    {
        if (arr1->left.dynamicIndex->sign != arr2->left.dynamicIndex->sign ||
            arr1->right.dynamicIndex->sign != arr2->right.dynamicIndex->sign)
        {
            return 1;
        }
        if (arr1->left.dynamicIndex->index->type != arr2->left.dynamicIndex->index->type ||
            arr1->right.dynamicIndex->index->type != arr2->right.dynamicIndex->index->type)
        {
            return 1;
        }
    }
    return 0;
}

// get the type of an ast node given the symbol table
enum TYPE getType(ast_node *node, symbol_table *st)
{
    switch (node->nodeType)
    {
    case PLUS_AST:
    case MINUS_AST:
    case MUL_AST:
    case DIV_AST:
    case AND_AST:
    case OR_AST:
    case LT_AST:
    case GT_AST:
    case LE_AST:
    case GE_AST:
    case EQ_AST:
    case NE_AST:
    case ARR_ELEM_AST:
        return node->type;
    case NUM:
        return __NUM__;
    case RNUM:
        return __RNUM__;
    case TRUE_PTN_AST:
    case FALSE_PTN_AST:
        return __BOOL__;
    case ID:
    {
        printf("Getting type of ID\n");
        ST_ENTRY *st_entry = checkAllSymbolTables(st, ((parse_tree_node *)node)->leafNodeInfo.lexeme);
        if (st_entry && st_entry->entry_type == VAR_SYM)
        {
            return st_entry->data.var->type;
        }
        printf("Returning error\n");
        return __ERROR__;
    }
    default:
        return __ERROR__;
    }
}

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
        printf("Operator: %d\n", expr->nodeType);
        fflush(stdout);
        checkExpressionNames(expr->left, symTable);
        printf("Left checked\n");
        checkExpressionNames(expr->right, symTable);
        printf("Right checked\n");
        // type propagation and checking
        switch (expr->nodeType)
        {
        case PLUS_AST:
        case MUL_AST:
        {
            if (getType(expr->left, symTable) == __NUM__ && getType(expr->right, symTable) == __NUM__)
            {
                expr->type = __NUM__;
            }
            else if (getType(expr->left, symTable) == __RNUM__ && getType(expr->right, symTable) == __RNUM__)
            {
                expr->type = __RNUM__;
            }
            else
            {
                expr->type = __ERROR__;
            }
            printf("Plus/Mul type: %d\n", expr->type);
            break;
        }
        case MINUS_AST:
        {
            printf("Minus\n");
            if (expr->left == NULL)
            {
                printf("Left null\n");
                if (expr->right == NULL)
                {
                    expr->type = __ERROR__;
                }
                else
                {
                    expr->type = getType(expr->right, symTable);
                    printf("Right type: %d\n", expr->type);
                }
            }
            else if (getType(expr->left, symTable) == __NUM__ && getType(expr->right, symTable) == __NUM__)
            {
                printf("Left num and right num\n");
                expr->type = __NUM__;
            }
            else if (getType(expr->left, symTable) == __RNUM__ && getType(expr->right, symTable) == __RNUM__)
            {
                printf("Left rnum and right rnum\n");
                expr->type = __RNUM__;
            }
            else
            {
                printf("Error\n");
                expr->type = __ERROR__;
            }
            break;
        }
        case DIV_AST:
        {
            if (getType(expr->left, symTable) == __RNUM__ && getType(expr->right, symTable) == __RNUM__)
            {
                expr->type = __RNUM__;
            }
            else
            {
                expr->type = __ERROR__;
            }
            break;
        }
        case AND_AST:
        case OR_AST:
        {
            if (getType(expr->left, symTable) == __BOOL__ && getType(expr->right, symTable) == __BOOL__)
            {
                expr->type = __BOOL__;
            }
            else
            {
                expr->type = __ERROR__;
            }
            break;
        }
        case EQ_AST:
        case NE_AST:
        case LT_AST:
        case LE_AST:
        case GT_AST:
        case GE_AST:
        {
            if (getType(expr->left, symTable) == getType(expr->right, symTable))
            {
                expr->type = __BOOL__;
            }
            else
            {
                expr->type = __ERROR__;
            }
            break;
        }
        case ARR_ELEM_AST:
        {
            // expression checking for the index and the array
            checkExpressionNames(expr->left, symTable);
            checkExpressionNames(expr->right, symTable);
            parse_tree_node *arr_id = (parse_tree_node *)expr->left;
            ST_ENTRY *arr_entry = checkAllSymbolTables(symTable, arr_id->leafNodeInfo.lexeme);
            if (arr_entry != NULL)
            {
                // type checking for array variable
                if (arr_entry->entry_type != ARR_SYM)
                {
                    printf("Error: Array %s used at line number %d is not an array.\n",
                           arr_id->leafNodeInfo.lexeme, arr_id->leafNodeInfo.lineNumber);
                    return;
                }
                else
                {
                    // type checking for array index
                    if (getType(expr->right, symTable) != __NUM__)
                    {
                        printf("Error: Index of array %s used at line number %d is not an integer.\n",
                               arr_id->leafNodeInfo.lexeme, arr_id->leafNodeInfo.lineNumber);
                        return;
                    }
                    // bound checking for static arrays and static index
                    // TODO: handle negative indices
                    int indexCheck = 0;
                    int indexVal;
                    if (expr->right->nodeType == NUM || (expr->right->nodeType == MINUS_AST && expr->right->left == NULL && expr->right->right->nodeType == NUM))
                    {
                        indexCheck = 1;
                        if (expr->right->nodeType == NUM)
                        {
                            indexVal = expr->right->leafNodeInfo.val.intValue;
                        }
                        else
                        {
                            indexVal = -1 * expr->right->right->leafNodeInfo.val.intValue;
                        }
                    }
                    if (indexCheck && arr_entry->data.arr->arrayType == STATIC && getType(expr->right, symTable) == __NUM__)
                    {
                        if (indexVal > arr_entry->data.arr->right.staticIndex || indexVal < arr_entry->data.arr->left.staticIndex)
                        {
                            printf("Error: Index of array %s %d used at line number %d is out of the bounds %d to %d.\n",
                                   arr_id->leafNodeInfo.lexeme, indexVal, arr_id->leafNodeInfo.lineNumber, arr_entry->data.arr->left.staticIndex,
                                   arr_entry->data.arr->right.staticIndex);
                            return;
                        }
                    }
                    expr->type = arr_entry->data.arr->eltype;
                    printf("Array element type: %d\n", expr->type);
                }
            }
            else
            {
                printf("Error: Declaration of array %s used at line number %d not found.\n",
                       arr_id->leafNodeInfo.lexeme, arr_id->leafNodeInfo.lineNumber);
                return;
            }
            break;
        }
        }
    }
    // identifiers
    else if (expr->nodeType == ID)
    {
        parse_tree_node *pt = (parse_tree_node *)expr;
        if (checkAllSymbolTables(symTable, pt->leafNodeInfo.lexeme) == NULL)
        {
            printf("Error: Declaration of variable %s used at line number %d not found.\n",
                   pt->leafNodeInfo.lexeme, pt->leafNodeInfo.lineNumber);
            return;
        }
        printf("Haan main ID hu. Mera naam %s hai. Line Number %d\n", pt->leafNodeInfo.lexeme, pt->leafNodeInfo.lineNumber);
        fflush(stdout);
    }
}

// type and expression checking for formal parameters of a function
void checkFormalParams(LinkedListASTNode *param_list, ST_LL *formal_param_list, symbol_table *symTable)
{
    while (param_list != NULL && formal_param_list != NULL)
    {
        checkExpressionNames(param_list->data, symTable);
        // two cases: formal_param_list is an array or a variable
        if (formal_param_list->data->entry_type == ARR_SYM)
        {
            // type checking for the input parameters
            ST_ENTRY *arr_entry = checkAllSymbolTables(symTable, ((parse_tree_node *)param_list->data)->leafNodeInfo.lexeme);
            if (arr_entry && matchArrayTypes(arr_entry->data.arr, formal_param_list->data->data.arr))
            {
                printf("Error: Array type mismatch at line number %d.\n",
                       ((parse_tree_node *)param_list->data)->leafNodeInfo.lineNumber);
            }
            else
            {
                printf("Type match for array param list arg.\n");
            }
        }
        else
        {
            // type checking for the input parameters
            if (getType(param_list->data, symTable) != formal_param_list->data->data.var->type)
            {
                printf("Error: Var Type mismatch at line number %d.\n",
                       ((parse_tree_node *)param_list->data)->leafNodeInfo.lineNumber);
            }
            else
            {
                printf("Type match for param list arg.\n");
            }
        }
        param_list = param_list->next;
        formal_param_list = formal_param_list->next;
    }
    if (param_list != NULL || formal_param_list != NULL)
    {
        printf("Error: Number of arguments passed do not match the number of parameters.\n");
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
            Id = stmt->right->nodeType == ID ? stmt->right : stmt->right->nodeType == ARR_ELEM_AST ? stmt->right->left : NULL;
            if (checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme) == NULL)
            {
                printf("Error: Variable %s not declared at line number %d\n", Id->leafNodeInfo.lexeme,
                       Id->leafNodeInfo.lineNumber);
            }
            break;
        }
        case EQUALS_AST:
        {
            printf("In equals\n");
            fflush(stdout);
            if (stmt->right->nodeType == USE_AST)
            {
                Id = stmt->right->left;
                ST_ENTRY *equals_st_entry = checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme);
                if (equals_st_entry != NULL)
                {
                    // check if the type is a function
                    if (equals_st_entry->entry_type != FUNC_SYM)
                    {
                        printf("Error: Variable %s at line number %d is not a function and seems\
                               to have been used as one.\n",
                               Id->leafNodeInfo.lexeme,
                               Id->leafNodeInfo.lineNumber);
                    }
                }
                else
                {
                    printf("Error: Variable %s not declared at line number %d\n", Id->leafNodeInfo.lexeme,
                           Id->leafNodeInfo.lineNumber);
                }
                // check recursion and throw error if detected
                if (checkRecursion(blockSymTable, Id->leafNodeInfo.lexeme))
                {
                    printf("Error: Recursion detected in module %s at line number %d. Please check the code and try again.\n",
                           Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                    stmts = stmts->next;
                    continue;
                }
                // check if actual input parameters are declared and type checking
                LinkedListASTNode *input_list = stmt->right->right;
                ST_LL *formal_input_list = equals_st_entry->data.func->inputs;
                checkFormalParams(input_list, formal_input_list, blockSymTable);
                // check if actual output parameters are declared and type checking
                LinkedListASTNode *output_list = stmt->left;
                ST_LL *formal_output_list = equals_st_entry->data.func->outputs;
                checkFormalParams(output_list, formal_output_list, blockSymTable);
            }
            // check an expression
            else
            {
                checkExpressionNames(stmt->left, blockSymTable);
                checkExpressionNames(stmt->right, blockSymTable);
                printf("We have checked expression names on both sides of equal\n");
                // TODO make sure both aren't error - also handle array equality
                // check if the types match on both sides of the assignment
                if (getType(stmt->left, blockSymTable) != getType(stmt->right, blockSymTable))
                {
                    printf("Error: Type mismatch at line number %d.\n", ((parse_tree_node *)stmt->left)->leafNodeInfo.lineNumber);
                }
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
            enum TYPE switchType = getType(stmt->left, blockSymTable);
            if (switchType != __NUM__ && switchType != __BOOL__)
            {
                printf("Error: Switching variable at line number %d is not an integer or a boolean.\n",
                       ((parse_tree_node *)stmt->left)->leafNodeInfo.lineNumber);
            }
            LinkedListASTNode *case_list = stmt->right;
            // case blocks
            while (case_list != NULL)
            {
                printf("We have a case block\n");
                fflush(stdout);
                if (case_list->data == NULL)
                {
                    printf("Missing default block.\n");
                    if (switchType != __BOOL__)
                    {
                        printf("Error: Switching variable at line number %d is not a boolean.\
                               Expected default block.\n",
                               stmt->left->leafNodeInfo.lineNumber);
                    }
                    break;
                }
                if (case_list->data->left == NULL)
                {
                    printf("In default block\n");
                    fflush(stdout);
                    if (switchType == __BOOL__)
                    {
                        printf("Error: Switching variable at line number %d is a boolean.\
                               Default block not expected.\n",
                               stmt->left->leafNodeInfo.lineNumber);
                    }
                }
                else
                {
                    // check if the case value is of the same type as the switching variable
                    switch (switchType)
                    {
                    case __NUM__:
                        if (case_list->data->left->nodeType != NUM)
                        {
                            printf("Error: Case value at line number %d is not an integer.\n",
                                   case_list->data->left->leafNodeInfo.lineNumber);
                        }
                        break;
                    case __BOOL__:
                        if (case_list->data->left->nodeType != TRUE_PTN_AST &&
                            case_list->data->left->nodeType != FALSE_PTN_AST)
                        {
                            printf("Error: Case value at line number %d is not a boolean.\n",
                                   case_list->data->left->leafNodeInfo.lineNumber);
                        }
                        break;
                    }
                }
                LinkedListASTNode *stmt_list = case_list->data->right;
                // create a new symbol table for each case block and
                // add it to the parent symbol table. set the name of the symbol table
                // to the parent's name + case number
                char *casestr = malloc(sizeof(char) * 64);
                sprintf(casestr, "%p", case_list->data);
                symbol_table *caseSymTable = createSymbolTable(blockSymTable, casestr);
                updateOffset(caseSymTable, blockSymTable->offset + __NUM_SIZE__);
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
            // extract the range
            ast_node *range = stmt->left;
            int startSign = range->left->nodeType == MINUS_AST ? -1 : 1;
            int endSign = range->right->nodeType == MINUS_AST ? -1 : 1;
            parse_tree_node *start = startSign == -1 ? range->left->right : range->left;
            parse_tree_node *end = endSign == -1 ? range->right->right : range->right;
            // type check the range
            if (start->leafNodeInfo.tokenID != NUM)
            {
                printf("Error: Start of range is not an integer at line number %d.\n",
                       start->leafNodeInfo.lineNumber);
            }
            if (end->leafNodeInfo.tokenID != NUM)
            {
                printf("Error: End of range is not an integer at line number %d.\n",
                       end->leafNodeInfo.lineNumber);
            }
            // store start and end values in symbol table for the block
            ST_ENTRY *startEntry = malloc(sizeof(ST_ENTRY));
            startEntry->name = "$start";
            startEntry->entry_type = VAR_SYM;
            startEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            startEntry->data.var->type = __NUM__;
            startEntry->data.var->val.numValue = startSign * start->leafNodeInfo.val.intValue;

            ST_ENTRY *endEntry = malloc(sizeof(ST_ENTRY));
            endEntry->name = "$end";
            endEntry->entry_type = VAR_SYM;
            endEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            endEntry->data.var->type = __NUM__;
            endEntry->data.var->val.numValue = endSign * end->leafNodeInfo.val.intValue;

            // list of statements in the for loop
            LinkedListASTNode *for_stmt_list = stmt->right;
            char *forstr = malloc(sizeof(char) * 64);
            sprintf(forstr, "%p", stmt);
            symbol_table *forSymTable = createSymbolTable(blockSymTable, forstr);
            // set offsets
            startEntry->data.var->offset = forSymTable->offset;
            updateOffset(forSymTable, forSymTable->offset + __NUM_SIZE__);
            endEntry->data.var->offset = forSymTable->offset;
            updateOffset(forSymTable, forSymTable->offset + __NUM_SIZE__);
            // add the start and end values to the symbol table
            addToSymbolTable(forSymTable, startEntry);
            addToSymbolTable(forSymTable, endEntry);

            // add the loop variable to the symbol table
            ST_ENTRY *loopVarEntry = malloc(sizeof(ST_ENTRY));
            loopVarEntry->name = loopVar->leafNodeInfo.lexeme;
            loopVarEntry->entry_type = VAR_SYM;
            loopVarEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            loopVarEntry->data.var->type = __NUM__; // can only be an integer
            loopVarEntry->data.var->offset = forSymTable->offset;
            updateOffset(forSymTable, forSymTable->offset + __NUM_SIZE__);
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
            // type check that the condition can only be a boolean
            if (getType(stmt->left, blockSymTable) != __BOOL__)
            {
                printf("Error: Condition in while loop is not a boolean at line number %d.\n",
                       stmt->left->leafNodeInfo.lineNumber);
            }
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
        // TODO: Function call that returns nothing
        case USE_AST:
        {

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

// populate symbol tables for other modules
void populateOtherModuleSignatures(LinkedListASTNode *node)
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
            if (symTabEntryForModule->entry_type != FUNC_SYM)
            {
                printf("Error: Name %s at line number %d does not seem to be a function or might be clashing\
                with a variable of the same name. Please check the code and try again.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                node = node->next;
                continue;
            }
            // module already has a definition
            else if (symTabEntryForModule->data.func != NULL)
            {
                printf("Error: Function %s at line number %d seems to already have been defined. Please check\
                the code and try again.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                node = node->next;
                continue;
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
        ST_ENTRY *blockSymTableEntry = malloc(sizeof(ST_ENTRY));
        char *blockname = malloc(sizeof(char) * 64);
        sprintf(blockname, "%s_block", symTabEntryForModule->name);
        blockSymTableEntry->name = blockname;
        blockSymTableEntry->entry_type = BLOCK_SYM;
        blockSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
        blockSymTableEntry->data.block->body = moduleDef;
        // create a new symbol table for the block
        symbol_table *blockSymTable = createSymbolTable(funcSymTable, blockSymTableEntry->name);
        blockSymTableEntry->data.block->symTable = blockSymTable;
        addToSymbolTable(funcSymTable, blockSymTableEntry);
        // LinkedListASTNode *stmts = moduleDef;
        // populateBlockSymbolTables(stmts, blockSymTable);
        node = node->next;
    }
}

// populate symbol tables for all module bodies
void populateOtherModuleBodies(LinkedListASTNode *node)
{
    while (node != NULL)
    {
        // extract module info from the AST node
        struct moduleDefAuxInfo *mdai = (struct moduleDefAuxInfo *)node->data->aux_info;
        parse_tree_node *Id = mdai->Id;
        LinkedListASTNode *input_plist = mdai->input_plist;
        LinkedListASTNode *ret = mdai->ret;
        LinkedListASTNode *moduleDef = mdai->moduleDef;
        ST_ENTRY *symTabEntryForModule = checkSymbolTable(&symbolTable, Id->leafNodeInfo.lexeme);
        // module already exists in the symbol table; extract block symbol table
        char *blockname = malloc(sizeof(char) * 64);
        sprintf(blockname, "%s_block", symTabEntryForModule->name);
        ST_ENTRY *blockSymTableEntry = checkSymbolTable(symTabEntryForModule->data.func->symTable, blockname);
        symbol_table *blockSymTable = blockSymTableEntry->data.block->symTable;
        // populate the symbol table with the block statements
        populateBlockSymbolTables(blockSymTableEntry->data.block->body, blockSymTable);
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
    populateOtherModuleSignatures(otherModules1);
    populateOtherModuleSignatures(otherModules2);
    // populate OtherModule1 body
    populateOtherModuleBodies(otherModules1);
    // driver module (aux_info is a linked list of statements)
    symbol_table *driverSymTable = createSymbolTable(&symbolTable, "driver");
    LinkedListASTNode *driverStmts = driverModule->aux_info;
    ST_ENTRY *driverSymTableEntry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    driverSymTableEntry->name = "driver";
    driverSymTableEntry->entry_type = BLOCK_SYM;
    driverSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
    driverSymTableEntry->data.block->body = driverStmts;
    driverSymTableEntry->data.block->symTable = driverSymTable;
    driverSymTableEntry->data.block->offset = 0;
    populateBlockSymbolTables(driverStmts, driverSymTable);
    addToSymbolTable(&symbolTable, driverSymTableEntry);
    // populate OtherModule2 body
    populateOtherModuleBodies(otherModules2);
}