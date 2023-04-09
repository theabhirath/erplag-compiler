#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

#define SYMBOL_TABLE_SIZE 512

#define __NUM_SIZE__ 1
#define __RNUM_SIZE__ 1
#define __BOOL_SIZE__ 1
#define __DYNAMIC_ARRAY_SIZE__ 1

typedef struct ST_ENTRY ST_ENTRY;

typedef enum ST_ENTRY_TYPE
{
    VAR_SYM,
    ARR_SYM,
    FUNC_SYM,
    BLOCK_SYM
} ST_ENTRY_TYPE;

enum ARRAY_TYPE
{
    STATIC,
    DYNAMIC,
    INDEX_ERROR
};

union value{
    int numValue;
    float rnumValue;
    int boolValue;
};

struct var_entry
{
    enum TYPE type;
    int offset;
    union value val;
};

struct dynamicIndex
{
    int sign;
    struct var_entry* index;
};

union indexType{
    int staticIndex;
    struct dynamicIndex* dynamicIndex;
};

struct arr_entry
{
    enum TYPE eltype;
    int offset;
    enum ARRAY_TYPE arrayType;
    union indexType left;
    union indexType right;
};

typedef struct ST_LL{
    ST_ENTRY *data;
    struct ST_LL *next;
} ST_LL;

struct func_entry
{
    int offset;
    struct ST_LL *inputs;
    struct ST_LL *outputs;
    LinkedListASTNode *body;
    struct symbol_table *symTable;
};

struct block_entry
{
    int offset;
    LinkedListASTNode *body;
    struct symbol_table *symTable;
};

union st_entry_data
{
    struct var_entry *var;
    struct arr_entry *arr;
    struct func_entry *func;
    struct block_entry *block;
};

typedef struct ST_ENTRY
{
    ST_ENTRY_TYPE entry_type;
    union st_entry_data data;
    char *name;
} ST_ENTRY;

typedef struct symbol_table
{
    char *name;
    ST_LL *data[SYMBOL_TABLE_SIZE];
    struct symbol_table *parent;
    int offset;
} symbol_table;

extern symbol_table symbolTable;

void populateSymbolTables(ast *ASTree);
ST_ENTRY *checkSymbolTable(symbol_table *symTab, char *name);
ST_ENTRY *checkAllSymbolTables(symbol_table *symTab, char *name);
enum TYPE getType(ast_node *node, symbol_table *st);

#endif
