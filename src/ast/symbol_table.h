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

union indexType{
    int staticIndex;
    struct var_entry* dynamicIndex;
};

struct arr_entry
{
    enum TYPE type;
    int offset;
    enum ARRAY_TYPE arrayType;
    union indexType start;
    union indexType end;
};

struct param_entry{
    ST_ENTRY *data;
    struct param_entry *next;
};

struct func_entry
{
    int offset;
    struct param_entry *inputs;
    struct param_entry *outputs;
    struct symbol_table *symTable;
};

struct block_entry
{
    int offset;
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

struct symbol_table
{
    ST_ENTRY data[SYMBOL_TABLE_SIZE];
    struct symbol_table *parent;
};

extern struct symbol_table global_sym_table;

#endif
