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

// add a variable to the symbol table given the variable name and type
ST_ENTRY *addVarToSymTable(symbol_table *symTab, parse_tree_node *var_name, ast_node *var_type)
{
    ST_ENTRY *st_entry = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
    st_entry->name = getName(var_name);
    if (checkSymbolTable(symTab, st_entry->name) != NULL)
    {
        printf("Error: Variable %s found at line number %d has already been declared before.\n",
               st_entry->name, getLineNumber(var_name));
        return NULL;
    }
    st_entry->entry_type = VAR_SYM;
    switch (var_type->nodeType)
    {
    case INTEGER_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->isConstant = 0;
        st_entry->data.var->lastModifiedLineNumber = -1;
        st_entry->data.var->type = __NUM__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __NUM_SIZE__;
        break;
    case REAL_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->isConstant = 0;
        st_entry->data.var->lastModifiedLineNumber = -1;
        st_entry->data.var->type = __RNUM__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __RNUM_SIZE__;
        break;
    case BOOLEAN_AST:
        st_entry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
        st_entry->data.var->isConstant = 0;
        st_entry->data.var->lastModifiedLineNumber = -1;
        st_entry->data.var->type = __BOOL__;
        st_entry->data.var->offset = symTab->offset;
        symTab->offset += __BOOL_SIZE__;
        break;
    case ARRAY_AST:
        st_entry->data.arr = (struct arr_entry *)malloc(sizeof(struct arr_entry));
        st_entry->entry_type = ARR_SYM;
        st_entry->data.arr->lastModifiedLineNumber = -1;
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
        // static or dynamic array
        if (lindex->leafNodeInfo.tokenID == NUM && rindex->leafNodeInfo.tokenID == NUM)
        {
            int lindexVal = lindexSign * lindex->leafNodeInfo.val.intValue;
            int rindexVal = rindexSign * rindex->leafNodeInfo.val.intValue;
            // check if lindexVal < rindexVal
            if (lindexVal >= rindexVal)
            {
                printf("Error: Array %s at line number %d has invalid range.\n",
                       st_entry->name, getLineNumber(var_name));
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
                    printf("Error: Declaration of variable %s used at line number %d not found.\n",
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
                    printf("Error: Declaration of variable %s used at line number %d not found.\n",
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
    ST_LL *params = NULL;
    while (plist != NULL)
    {
        ST_ENTRY *input_st_entry = addVarToSymTable(funcSymTable, plist->data->left, plist->data->right);
        ST_ENTRY *check = checkSymbolTable(funcSymTable, input_st_entry->name);
        params = addToST_LL(params, input_st_entry);
        plist = plist->next;
    }
    return params;
}

// global symbol table
symbol_table symbolTable = {"42", NULL, NULL, 0};

// match array types
enum ARRAY_ERROR_TYPES matchArrayTypes(struct arr_entry *arr1, struct arr_entry *arr2)
{
    if (arr1->eltype != arr2->eltype)
    {
        return ARRAY_ELTYPE_ERROR;
    }
    if (arr1->arrayType == INDEX_ERROR || arr2->arrayType == INDEX_ERROR)
    {
        return ARRAY_INDEX_ERROR;
    }
    if (arr1->arrayType == STATIC)
    {
        if (arr2->left.staticIndex - arr1->left.staticIndex != arr2->right.staticIndex - arr1->right.staticIndex)
        {
            return ARRAY_STATIC_ASSIGN_ERROR;
        }
    }
    return ARRAY_NO_ERROR;
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
        ST_ENTRY *st_entry = checkAllSymbolTables(st, getName(node));
        if (st_entry != NULL)
        {
            if (st_entry->entry_type == VAR_SYM)
            {
                return st_entry->data.var->type;
            }
            else
            {
                return __TYPE_ERROR__;
            }
        }
        else
        {
            return __NOT_DEFINED_ERROR__;
        }
    }
    case ARR_ELEM_AST:
    {
        ST_ENTRY *st_entry = checkAllSymbolTables(st, getName(node->left));
        if (st_entry != NULL)
        {
            if (st_entry->entry_type == ARR_SYM)
            {
                return st_entry->data.arr->eltype;
            }
            else
            {
                return __TYPE_ERROR__;
            }
        }
        else
        {
            return __NOT_DEFINED_ERROR__;
        }
    }
    default:
        return __TYPE_ERROR__;
    }
}

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
        enum TYPE rightType = getType(expr->right, symTable);
        enum TYPE leftType = rightType;
        // unary minus
        if (expr->left != NULL)
        {
            leftType = getType(expr->left, symTable);
        }
        // not defined error handling
        if (leftType == __NOT_DEFINED_ERROR__ || rightType == __NOT_DEFINED_ERROR__)
        {
            expr->type = __NOT_DEFINED_ERROR__;
            return;
        }
        // type propagation and checking
        switch (expr->nodeType)
        {
        case PLUS_AST:
        case MUL_AST:
        {
            if (leftType == __NUM__ && rightType == __NUM__)
            {
                expr->type = __NUM__;
            }
            else if (leftType == __RNUM__ && rightType == __RNUM__)
            {
                expr->type = __RNUM__;
            }
            else
            {
                expr->type = __TYPE_ERROR__;
            }
            break;
        }
        case MINUS_AST:
        {
            if (expr->left == NULL)
            {
                if (expr->right == NULL)
                {
                    expr->type = __TYPE_ERROR__;
                }
                else
                {
                    expr->type = rightType;
                }
            }
            else if (leftType == __NUM__ && rightType == __NUM__)
            {
                expr->type = __NUM__;
            }
            else if (leftType == __RNUM__ && rightType == __RNUM__)
            {
                expr->type = __RNUM__;
            }
            else
            {
                expr->type = __TYPE_ERROR__;
            }
            break;
        }
        case DIV_AST:
        {
            if (leftType == __RNUM__ && rightType == __RNUM__)
            {
                expr->type = __RNUM__;
            }
            else
            {
                expr->type = __TYPE_ERROR__;
            }
            break;
        }
        case AND_AST:
        case OR_AST:
        {
            if (leftType == __BOOL__ && rightType == __BOOL__)
            {
                expr->type = __BOOL__;
            }
            else
            {
                expr->type = __TYPE_ERROR__;
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
            if (leftType == rightType)
            {
                expr->type = __BOOL__;
            }
            else
            {
                expr->type = __TYPE_ERROR__;
            }
            break;
        }
        case ARR_ELEM_AST:
        {
            // expression checking for the index and the array
            checkExpressionNames(expr->left, symTable);
            checkExpressionNames(expr->right, symTable);
            parse_tree_node *arr_id = expr->left;
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
                    if (rightType != __NUM__)
                    {
                        printf("Error: Index of array %s used at line number %d is not an integer.\n",
                               arr_id->leafNodeInfo.lexeme, arr_id->leafNodeInfo.lineNumber);
                        return;
                    }
                    // bound checking for static arrays with static index
                    int indexCheck = 0;
                    int indexVal;
                    if (expr->right->nodeType == NUM || (expr->right->nodeType == MINUS_AST && expr->right->left == NULL && expr->right->right->nodeType == NUM))
                    {
                        indexCheck = 1;
                        if (expr->right->nodeType == NUM)
                        {
                            indexVal = getNumVal(expr->right);
                        }
                        else
                        {
                            indexVal = -1 * getNumVal(expr->right->right);
                        }
                    }
                    if (indexCheck && arr_entry->data.arr->arrayType == STATIC && rightType == __NUM__)
                    {
                        if (indexVal > arr_entry->data.arr->right.staticIndex || indexVal < arr_entry->data.arr->left.staticIndex)
                        {
                            printf("Error: Index of array %s[%d] used at line number %d is out of bounds.\n",
                                   arr_id->leafNodeInfo.lexeme, indexVal, arr_id->leafNodeInfo.lineNumber);
                            return;
                        }
                    }
                    expr->type = arr_entry->data.arr->eltype;
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
        parse_tree_node *pt = expr;
        if (checkAllSymbolTables(symTable, pt->leafNodeInfo.lexeme) == NULL)
        {
            printf("Error: Declaration of variable %s used at line number %d not found.\n",
                   pt->leafNodeInfo.lexeme, pt->leafNodeInfo.lineNumber);
            return;
        }
    }
}

// type and expression checking for formal parameters of a function
void checkFormalParams(LinkedListASTNode *param_list, ST_LL *formal_param_list, int inOut,
                       symbol_table *symTable)
{
    int paramLineNumber = getLineNumber(param_list->data);
    while (param_list != NULL && formal_param_list != NULL)
    {
        checkExpressionNames(param_list->data, symTable);
        // two cases: formal_param_list is an array or a variable
        if (formal_param_list->data->entry_type == ARR_SYM)
        {
            // type checking for the input parameters
            ST_ENTRY *arr_entry = checkAllSymbolTables(symTable, getName(param_list->data));
            if (arr_entry && matchArrayTypes(arr_entry->data.arr, formal_param_list->data->data.arr))
            {
                printf("Error: Type of actual parameter %s at line number %d does not match the type of formal parameter %s.\n",
                       getName(param_list->data), paramLineNumber, formal_param_list->data->name);
            }
        }
        else
        {
            // type checking for the input parameters
            enum TYPE actual_param_type = getType(param_list->data, symTable);
            if (actual_param_type != __NOT_DEFINED_ERROR__)
            {
                if (actual_param_type != formal_param_list->data->data.var->type)
                {
                    printf("Error: Type of actual parameter %s at line number %d does not match the type of formal parameter %s.\n",
                           getName(param_list->data), paramLineNumber, formal_param_list->data->name);
                }
            }
            ST_ENTRY *var_entry = checkAllSymbolTables(symTable, getName(param_list->data));
            if (var_entry && inOut == 1 && var_entry->entry_type == VAR_SYM)
            {
                var_entry->data.var->lastModifiedLineNumber = paramLineNumber;
                if (var_entry->data.var->isConstant)
                {
                    printf("Error: Variable %s at line number %d is a constant and cannot be assigned a value.\n",
                           var_entry->name, paramLineNumber);
                }
            }
        }
        param_list = param_list->next;
        formal_param_list = formal_param_list->next;
    }
    if (param_list != NULL || formal_param_list != NULL)
    {
        printf("Error: Number of arguments passed do not match the number of parameters at line number %d.\n",
               param_list ? getLineNumber(param_list->data) : paramLineNumber);
    }
}

// Get IDs in an expression
ST_LL *getIDsInExpression(ast_node *root, symbol_table *st)
{
    while (root)
    {
        // printf("nodeType: %d\t", root->nodeType);
        fflush(stdout);
        switch (root->nodeType)
        {
        case ID:
        {
            ST_LL *node = (ST_LL *)malloc(sizeof(ST_LL));
            node->data = checkAllSymbolTables(st, getName(root));
            node->next = NULL;
            return node;
        }

        case PARSE_TREE_AST1:
        case PARSE_TREE_AST2:
        case TRUE_PTN_AST:
        case FALSE_PTN_AST:
            return NULL;
        default:
        {
            ST_LL *left = getIDsInExpression(root->left, st);
            ST_LL *right = getIDsInExpression(root->right, st);
            if (left)
            {
                ST_LL *left_traverser = left;
                while (left_traverser->next)
                {
                    left_traverser = left_traverser->next;
                }
                left_traverser->next = right;
                return left;
            }
            else
            {
                return right;
            }
        }
        }
    }
    return NULL;
}

// populate the symbol table for a block
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
        {
            Id = stmt->right->nodeType == ID ? stmt->right : stmt->right->nodeType == ARR_ELEM_AST ? stmt->right->left
                                                                                                   : NULL;
            ST_ENTRY *get_value_st_entry = checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme);
            if (get_value_st_entry == NULL)
            {
                printf("Error: Declaration of variable %s used at line number %d not found.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
            }
            else
            {
                // Update last modified line number
                if (get_value_st_entry->entry_type == VAR_SYM)
                {
                    get_value_st_entry->data.var->lastModifiedLineNumber = getLineNumber(Id);
                }

                else if (get_value_st_entry->entry_type == ARR_SYM)
                {
                    get_value_st_entry->data.arr->lastModifiedLineNumber = getLineNumber(Id);
                }
            }
            break;
        }
        case PRINT_AST:
        {
            Id = stmt->right->nodeType == ID ? stmt->right : stmt->right->nodeType == ARR_ELEM_AST ? stmt->right->left
                                                                                                   : NULL;
            if (checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme) == NULL)
            {
                printf("Error: Declaration of variable %s used at line number %d not found.\n",
                       Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
            }
            break;
        }
        case EQUALS_AST:
        {
            if (stmt->right->nodeType == USE_AST)
            {
                Id = stmt->right->left;
                ST_ENTRY *equals_st_entry = checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme);
                if (equals_st_entry != NULL)
                {
                    // check if the type is a function
                    if (equals_st_entry->entry_type != FUNC_SYM)
                    {
                        printf("Error: Variable %s at line number %d is not a function and seems to have been used as one.\n",
                               Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
                    }
                    else
                    {
                        // update lastCalledLineNumber
                        equals_st_entry->data.func->lastCalledLineNumber = getLineNumber(Id);
                    }
                }
                else
                {
                    printf("Error: Declaration of variable %s used at line number %d not found.\n",
                           Id->leafNodeInfo.lexeme, Id->leafNodeInfo.lineNumber);
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
                checkFormalParams(input_list, formal_input_list, 0, blockSymTable);
                // check if actual output parameters are declared and type checking
                LinkedListASTNode *output_list = stmt->left;
                ST_LL *formal_output_list = equals_st_entry->data.func->outputs;
                checkFormalParams(output_list, formal_output_list, 1, blockSymTable);
            }
            // check an expression
            else
            {
                checkExpressionNames(stmt->left, blockSymTable);
                checkExpressionNames(stmt->right, blockSymTable);
                if (stmt->left->nodeType == ID)
                {
                    ST_ENTRY *left_st_entry = checkAllSymbolTables(blockSymTable, getName(stmt->left));
                    if (left_st_entry != NULL)
                    {
                        // Update last modified line number
                        if (left_st_entry->entry_type == VAR_SYM)
                        {
                            left_st_entry->data.var->lastModifiedLineNumber = getLineNumber(stmt->left);
                        }
                        else if (left_st_entry->entry_type == ARR_SYM)
                        {
                            left_st_entry->data.arr->lastModifiedLineNumber = getLineNumber(stmt->left);
                        }
                        if (stmt->right->nodeType == ID)
                        {
                            ST_ENTRY *right_st_entry = checkAllSymbolTables(blockSymTable, getName(stmt->right));
                            if (right_st_entry != NULL)
                            {
                                // array assignment
                                if (left_st_entry->entry_type == ARR_SYM && right_st_entry->entry_type == ARR_SYM)
                                {
                                    switch (matchArrayTypes(left_st_entry->data.arr, right_st_entry->data.arr))
                                    {
                                    case ARRAY_ELTYPE_ERROR:
                                        printf("Error: Array element type mismatch at line number %d while trying to assign.\n",
                                               getLineNumber(stmt->left));
                                        break;
                                    case ARRAY_INDEX_ERROR:
                                        printf("Error: One or more arrays at line number %d have an erroneous declaration of indices.\n",
                                               getLineNumber(stmt->left));
                                        break;
                                    case ARRAY_STATIC_ASSIGN_ERROR:
                                        printf("Error: Array %s is not structurally equivalent to the array %s at line number %d and so cannot be assiged to it.\n",
                                               getName(stmt->right), getName(stmt->left), getLineNumber(stmt->left));
                                        break;
                                    }
                                    stmts = stmts->next;
                                    continue;
                                }
                            }
                        }
                        // constant variables, like loop variables, cannot be assigned a value
                        if (left_st_entry->entry_type == VAR_SYM && left_st_entry->data.var->isConstant)
                        {
                            printf("Error: Variable %s at line number %d is a constant and cannot be assigned a value.\n",
                                   getName(stmt->left), getLineNumber(stmt->left));
                            stmts = stmts->next;
                            continue;
                        }
                    }
                }
                enum TYPE leftType = getType(stmt->left, blockSymTable);
                enum TYPE rightType = getType(stmt->right, blockSymTable);
                if (leftType == __NOT_DEFINED_ERROR__ || rightType == __NOT_DEFINED_ERROR__)
                {
                    stmts = stmts->next;
                    continue;
                }
                if (leftType == __TYPE_ERROR__ || rightType == __TYPE_ERROR__)
                {
                    printf("Error: Type mismatch at line number %d.\n", getLineNumber(stmt->left));
                }
                else
                {
                    // check if the types match on both sides of the assignment
                    if (leftType != rightType)
                    {
                        printf("Error: Type mismatch at line number %d.\n", getLineNumber(stmt->left));
                    }
                }
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
                var_list = var_list->next;
            }
            break;
        }
        // switching
        case SWITCH_AST:
        {
            // switching variable
            checkExpressionNames(stmt->left, blockSymTable);
            enum TYPE switchType = getType(stmt->left, blockSymTable);
            if (switchType != __NUM__ && switchType != __BOOL__)
            {
                printf("Error: Switching variable at line number %d is not an integer or a boolean.\n",
                       getLineNumber(stmt->left));
                break;
            }
            LinkedListASTNode *case_list = stmt->right;
            // case blocks
            while (case_list != NULL)
            {
                if (case_list->data == NULL)
                {
                    if (switchType != __BOOL__)
                    {
                        printf("Error: Switching variable at line number %d is not a boolean. Expected default block.\n",
                               getLineNumber(stmt->left));
                    }
                    break;
                }
                if (case_list->data->left == NULL)
                {
                    if (switchType == __BOOL__)
                    {
                        printf("Error: Switching variable at line number %d is a boolean. Default block encountered at line number %d not expected.\n",
                               getLineNumber(stmt->left), case_list->data->beginLine);
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
                                   getLineNumber(case_list->data->left));
                        }
                        break;
                    case __BOOL__:
                        if (case_list->data->left->nodeType != TRUE_PTN_AST &&
                            case_list->data->left->nodeType != FALSE_PTN_AST)
                        {
                            printf("Error: Case value at line number %d is not a boolean.\n",
                                   getLineNumber(case_list->data->left));
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
                caseSymTable->lineBegin = case_list->data->beginLine;
                caseSymTable->lineEnd = case_list->data->endLine;
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
            }
            break;
        }
        // for loop
        case FOR_AST:
        {
            // loop variable
            parse_tree_node *loopVar = stmt->aux_info;
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
                       getLineNumber(end));
            }
            // store start and end values in symbol table for the block
            ST_ENTRY *startEntry = malloc(sizeof(ST_ENTRY));
            startEntry->name = "$start";
            startEntry->entry_type = VAR_SYM;
            startEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            startEntry->data.var->isConstant = 1;
            startEntry->data.var->type = __NUM__;
            startEntry->data.var->val.numValue = startSign * start->leafNodeInfo.val.intValue;
            ST_ENTRY *endEntry = malloc(sizeof(ST_ENTRY));
            endEntry->name = "$end";
            endEntry->entry_type = VAR_SYM;
            endEntry->data.var = (struct var_entry *)malloc(sizeof(struct var_entry));
            endEntry->data.var->isConstant = 1;
            endEntry->data.var->type = __NUM__;
            endEntry->data.var->val.numValue = endSign * end->leafNodeInfo.val.intValue;
            // list of statements in the for loop
            LinkedListASTNode *for_stmt_list = stmt->right;
            char *forstr = malloc(sizeof(char) * 64);
            sprintf(forstr, "%p", stmt);
            symbol_table *forSymTable = createSymbolTable(blockSymTable, forstr);
            forSymTable->lineBegin = stmt->beginLine;
            forSymTable->lineEnd = stmt->endLine;
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
            loopVarEntry->data.var->isConstant = 1;
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
                       getLineNumber(stmt->left));
            }
            // list of statements in the while loop
            LinkedListASTNode *while_stmt_list = stmt->right;
            char *whilestr = malloc(sizeof(char) * 64);
            sprintf(whilestr, "%p", stmt);
            symbol_table *whileSymTable = createSymbolTable(blockSymTable, whilestr);
            whileSymTable->lineBegin = stmt->beginLine;
            whileSymTable->lineEnd = stmt->endLine;
            // create an entry for the while symbol table in the parent symbol table
            ST_ENTRY *whileSymTableEntry = malloc(sizeof(ST_ENTRY));
            whileSymTableEntry->name = whilestr;
            whileSymTableEntry->entry_type = BLOCK_SYM;
            whileSymTableEntry->data.block = (struct block_entry *)malloc(sizeof(struct block_entry));
            whileSymTableEntry->data.block->body = while_stmt_list;
            whileSymTableEntry->data.block->symTable = whileSymTable;
            populateBlockSymbolTables(while_stmt_list, whileSymTable);
            // Get all IDs in the expression
            ST_LL *idList = getIDsInExpression(stmt->left, blockSymTable);
            // exit(1);
            // Check if any ID was modified after whileSymTable->lineBegin and before whileSymTable->lineEnd
            if (idList)
            {
                int modified = 0;
                while (idList)
                {
                    ST_ENTRY *idEntry = idList->data;
                    if (idEntry->data.var->lastModifiedLineNumber > whileSymTable->lineBegin && idEntry->data.var->lastModifiedLineNumber < whileSymTable->lineEnd)
                    {
                        modified = 1;
                        break;
                    }
                    idList = idList->next;
                }
                if (modified == 0)
                {
                    printf("Error: No variable in the condition in while loop at line number %d is modified in the body.\n",
                           whileSymTable->lineBegin);
                }
            }
            addToSymbolTable(blockSymTable, whileSymTableEntry);
            break;
        }
        case USE_AST:
        {
            Id = stmt->left;
            ST_ENTRY *use_st_entry = checkAllSymbolTables(blockSymTable, Id->leafNodeInfo.lexeme);
            if (use_st_entry != NULL)
            {
                // check if the type is a function
                if (use_st_entry->entry_type != FUNC_SYM)
                {
                    printf("Error: Variable %s at line number %d is not a function and seems to have been used as one.\n",
                           getName(Id), getLineNumber(Id));
                }
                else
                {
                    // update the last called line number
                    use_st_entry->data.func->lastCalledLineNumber = getLineNumber(Id);
                }
            }
            else
            {
                printf("Error: Declaration of variable %s used at line number %d not found.\n",
                       getName(Id), getLineNumber(Id));
            }
            // check recursion and throw error if detected
            if (checkRecursion(blockSymTable, Id->leafNodeInfo.lexeme))
            {
                printf("Error: Recursion detected in module %s at line number %d. Please check the code and try again.\n",
                       getName(Id), getLineNumber(Id));
                stmts = stmts->next;
                continue;
            }
            // check if actual input parameters are declared and type checking
            LinkedListASTNode *input_list = stmt->aux_info;
            ST_LL *formal_input_list = use_st_entry->data.func->inputs;
            checkFormalParams(input_list, formal_input_list, 0, blockSymTable);
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
        int forwardDeclaration = 0;
        // module already exists in the symbol table
        if ((symTabEntryForModule = checkSymbolTable(&symbolTable, Id->leafNodeInfo.lexeme)) != NULL)
        {
            forwardDeclaration = 1;
            // ensure type of symbol table entry is a function
            if (symTabEntryForModule->entry_type != FUNC_SYM)
            {
                node = node->next;
                continue;
            }
            // module already has a definition
            else if (symTabEntryForModule->data.func != NULL)
            {
                node = node->next;
                continue;
            }
        }
        // module does not exist in the symbol table
        else
        {
            // create a new symbol table entry for the module
            symTabEntryForModule = (ST_ENTRY *)malloc(sizeof(ST_ENTRY));
            symTabEntryForModule->name = getName(Id);
            symTabEntryForModule->entry_type = FUNC_SYM;
            symTabEntryForModule->data.func = NULL;
            addToSymbolTable(&symbolTable, symTabEntryForModule);
        }
        symbol_table *funcSymTable = createSymbolTable(&symbolTable, symTabEntryForModule->name);
        if (moduleDef != NULL)
        {
            ast_node *moduleDefAST = moduleDef->data;
            funcSymTable->lineBegin = moduleDefAST->beginLine;
            funcSymTable->lineEnd = moduleDefAST->endLine;
        }
        else
        {
            funcSymTable->lineBegin = getLineNumber(Id);
            funcSymTable->lineEnd = getLineNumber(Id);
        }
        // populate the symbol table with the input parameters
        symTabEntryForModule->data.func = (struct func_entry *)malloc(sizeof(struct func_entry));
        symTabEntryForModule->data.func->inputs = addParamListToFuncSymTable(funcSymTable, input_plist);
        symTabEntryForModule->data.func->outputs = addParamListToFuncSymTable(funcSymTable, ret);
        symTabEntryForModule->data.func->body = moduleDef;
        symTabEntryForModule->data.func->symTable = funcSymTable;
        symTabEntryForModule->data.func->lastCalledLineNumber = -1 * forwardDeclaration;
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
        ST_ENTRY *symTabEntryForModule = checkSymbolTable(&symbolTable, getName(Id));
        // ensure type of symbol table entry is a function
        if (symTabEntryForModule->entry_type != FUNC_SYM)
        {
            printf("Error: Name %s at line number %d does not seem to be a function or might be clashing with a variable of the same name. Please check the code and try again.\n",
                   getName(Id), getLineNumber(Id));
            node = node->next;
            continue;
        }
        // module already has a definition
        else
        {
            // module already exists in the symbol table; extract block symbol table
            char *blockname = malloc(sizeof(char) * 64);
            sprintf(blockname, "%s_block", symTabEntryForModule->name);
            ST_ENTRY *blockSymTableEntry = checkSymbolTable(symTabEntryForModule->data.func->symTable, blockname);
            if (blockSymTableEntry != NULL)
            {
                printf("Error: Function %s at line number %d cannot be overloaded. Please check the code and try again.\n",
                       getName(Id), getLineNumber(Id));
                node = node->next;
                continue;
            }
        }
        if (symTabEntryForModule->data.func->lastCalledLineNumber == -1)
        {
            // Module was forward declared but not used before definition
            printf("Error: Function %s defined at line number %d was forward declared but not used before definition. Please check the code and try again.\n",
                   getName(Id), getLineNumber(Id));
        }
        symbol_table *funcSymTable = symTabEntryForModule->data.func->symTable;
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
        blockSymTable->lineBegin = funcSymTable->lineBegin;
        blockSymTable->lineEnd = funcSymTable->lineEnd;
        addToSymbolTable(funcSymTable, blockSymTableEntry);
        // populate the symbol table with the block statements
        populateBlockSymbolTables(blockSymTableEntry->data.block->body, blockSymTable);
        // Check if all outputs are assigned values
        ST_LL *outputList = symTabEntryForModule->data.func->outputs;

        while (outputList)
        {
            if (outputList->data->entry_type == VAR_SYM && outputList->data->data.var->lastModifiedLineNumber == -1 || outputList->data->entry_type == ARR_SYM && outputList->data->data.arr->lastModifiedLineNumber == -1)
            {
                printf("Error: Output parameter %s is not being assigned any value in the body of the function defined at line number %d and ending at at line number %d\n",
                       outputList->data->name, symTabEntryForModule->data.func->symTable->lineBegin, symTabEntryForModule->data.func->symTable->lineEnd);
            }
            outputList = outputList->next;
        }

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
        moduledec_st_entry->name = getName(Id);
        moduledec_st_entry->entry_type = FUNC_SYM;
        moduledec_st_entry->data.func = NULL;
        addToSymbolTable(&symbolTable, moduledec_st_entry);
        node = node->next;
    }
    // traverse the program, adding module signatures to the symbol table
    populateOtherModuleSignatures(otherModules1);
    populateOtherModuleSignatures(otherModules2);
    // populate body of other modules (otherModules1)
    populateOtherModuleBodies(otherModules1);
    // driver module (aux_info is a linked list of statements)
    symbol_table *driverSymTable = createSymbolTable(&symbolTable, "driver");
    driverSymTable->lineBegin = driverModule->beginLine;
    driverSymTable->lineEnd = driverModule->endLine;
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
    // populate body of other modules (otherModules2)
    populateOtherModuleBodies(otherModules2);
}

char *getSymTabName(symbol_table *symTab)
{
    if (symTab->parent != &symbolTable)
    {
        return getSymTabName(symTab->parent);
    }
    else
    {
        return symTab->name;
    }
}
// Print the symbol table
void printSymbolTable(symbol_table *symTab, int level)
{
    if (level == 0)
    {
        printf("Var\tscope\t\t\t\tscope\ttype of element\t\tis_array\tStatic/dynamic\tarray range\twidth\t\toffset\tnesting level\n");
        printf("Name\t(mod name)\t\t (line numbers)\n");
    }
    char *types[] = {"integer", "real", "boolean", "error"};
    int sizes[] = {__NUM_SIZE__, __RNUM_SIZE__, __BOOL_SIZE__, -1};
    char *arr_type_names[] = {"static", "dynamic", "error"};
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        ST_LL *list = symTab->data[i];
        while (list != NULL)
        {
            ST_ENTRY *entry = list->data;
            switch (entry->entry_type)
            {
            case VAR_SYM:
                printf("%s\t", entry->name);
                printf("%s\t\t\t\t", getSymTabName(symTab));
                printf("%d-%d\t", symTab->lineBegin, symTab->lineEnd);
                printf("%s\t\t\t", types[entry->data.var->type]);
                printf("%s\t\t", "no");
                printf("%s\t\t", "**");
                printf("%s\t\t", "**");
                printf("%d\t\t", sizes[entry->data.var->type]);
                printf("%d\t\t", entry->data.var->offset);
                printf("%d\t\t", level);
                printf("\n");
                break;
            case ARR_SYM:
                printf("%s\t", entry->name);
                printf("%s\t\t\t\t", getSymTabName(symTab));
                printf("%d-%d\t", symTab->lineBegin, symTab->lineEnd);
                printf("%s\t\t\t", types[entry->data.arr->eltype]);
                printf("%s\t\t", "yes");
                printf("%s\t\t", arr_type_names[entry->data.arr->arrayType]);
                if (entry->data.arr->arrayType == STATIC)
                {
                    printf("%d-%d\t\t", entry->data.arr->left.staticIndex, entry->data.arr->right.staticIndex);
                    printf("%d\t\t", (entry->data.arr->right.staticIndex - entry->data.arr->left.staticIndex + 2) * sizes[entry->data.arr->eltype]);
                }
                else
                {
                    printf("%s\t\t", "**");
                    printf("%s\t\t", "**");
                }
                printf("%d\t\t", entry->data.arr->offset);
                printf("%d\t\t", level);
                printf("\n");
                break;
            case FUNC_SYM:
                printSymbolTable(entry->data.func->symTable, level + 1);
                break;
            case BLOCK_SYM:
                printSymbolTable(entry->data.block->symTable, level + 1);
                break;
            default:
                break;
            }
            list = list->next;
        }
    }
}

void printArrs(symbol_table *symTab, int level)
{
    if (level == 0)
    {
        printf("Scope\t\t\tScope\tArr Name\t\t\tArr Type\tArr Range\tElement Type\n");
    }
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        ST_LL *list = symTab->data[i];
        while (list != NULL)
        {
            ST_ENTRY *entry = list->data;
            switch (entry->entry_type)
            {
            case ARR_SYM:
                printf("%s\t\t\t", getSymTabName(symTab));
                printf("%d-%d\t", symTab->lineBegin, symTab->lineEnd);
                printf("%s\t\t\t", entry->name);
                printf("%s\t\t", entry->data.arr->arrayType == STATIC ? "static array" : "dynamic array");
                if (entry->data.arr->arrayType == STATIC)
                {
                    printf("[%d-%d]\t\t", entry->data.arr->left.staticIndex, entry->data.arr->right.staticIndex);
                }
                else
                {
                    printf("%s\t\t", "**");
                }
                printf("%s\t\t", entry->data.arr->eltype == INTEGER ? "integer" : entry->data.arr->eltype == REAL ? "real"
                                                                                                                  : "boolean");
                printf("\n");
                break;
            case FUNC_SYM:
                printArrs(entry->data.func->symTable, level + 1);
                break;
            case BLOCK_SYM:
                printArrs(entry->data.block->symTable, level + 1);
                break;
            default:
                break;
            }
            list = list->next;
        }
    }
}

int getActivationRecordSize(symbol_table *symTab)
{
    int size = symTab->offset;
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        ST_LL *list = symTab->data[i];
        while (list != NULL)
        {
            ST_ENTRY *entry = list->data;
            switch (entry->entry_type)
            {
            case FUNC_SYM:
                size += getActivationRecordSize(entry->data.func->symTable);
                break;
            case BLOCK_SYM:
                size += getActivationRecordSize(entry->data.block->symTable);
                break;
            default:
                break;
            }
            list = list->next;
        }
    }
    return size;
}

void printActivationRecord()
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        ST_LL *list = symbolTable.data[i];
        while (list != NULL)
        {
            ST_ENTRY *entry = list->data;
            switch (entry->entry_type)
            {
            case FUNC_SYM:
                printf("%s %d\n", entry->name, getActivationRecordSize(entry->data.func->symTable));
                break;
            case BLOCK_SYM:
                printf("%s %d\n", entry->name, getActivationRecordSize(entry->data.block->symTable));
                break;
            default:
                break;
            }
            list = list->next;
        }
    }
}

void destroySymbolTables(symbol_table *symTab)
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++)
    {
        ST_LL *list = symTab->data[i];
        while (list != NULL)
        {
            ST_ENTRY *entry = list->data;
            switch (entry->entry_type)
            {
            case FUNC_SYM:
                destroySymbolTables(entry->data.func->symTable);
                free(entry->data.func->symTable);
                break;
            case BLOCK_SYM:
                destroySymbolTables(entry->data.block->symTable);
                free(entry->data.block->symTable);
                break;
            default:
                break;
            }
            list = list->next;
        }
        symTab->data[i] = NULL;
    }
}