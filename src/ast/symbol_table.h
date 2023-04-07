#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

#define SYMBOL_TABLE_SIZE 512

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
    DYNAMIC
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
    enum TYPE type;
    int offset;
    enum ARRAY_TYPE arrayType;
    union indexType start;
    union indexType end;
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
} symbol_table;

extern symbol_table symbolTable;

#endif
