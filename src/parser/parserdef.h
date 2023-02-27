#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "../lexer/lexerdef.h"

#define NUM_TOKENS 57
#define NUM_NONTERMINALS 67
#define NUM_RULES 128

enum TNT_TYPE
{
    __TERMINAL__,
    __NONTERMINAL__,
    __NONE__,
    __EPSILON__
};
typedef enum TNT_TYPE TNT_TYPE;

extern int Table[NUM_NONTERMINALS][NUM_TOKENS];

enum NONTERMINAL
{
    __program__,
    __moduleDeclarations__,
    __moduleDeclaration__,
    __otherModules__,
    __driverModule__,
    __module__,
    __ret__,
    __input_plist__,
    __iP0__,
    __output_plist__,
    __oP0__,
    __dataType__,
    __type__,
    __moduleDef__,
    __statements__,
    __statement__,
    __ioStmt__,
    __var3__,
    __var__,
    __whichId__,
    __wI0__,
    __simpleStmt__,
    __assignmentStmt__,
    __whichStmt__,
    __lvalueIDStmt__,
    __lvalueARRStmt__,
    __element_index_with_expression__,
    __sign__,
    __N_10__,
    __arrExpr__,
    __arr_N4__,
    __arrTerm__,
    __arr_N5__,
    __arrFactor__,
    __id_num_rnum__,
    __moduleReuseStmt__,
    __optional__,
    __idList__,
    __iL0__,
    __expression__,
    __booleanExpr__,
    __BE00__,
    __BE0__,
    __arithmeticExpr__,
    __AE0__,
    __term__,
    __T0__,
    __op1__,
    __op2__,
    __bools__,
    __factor__,
    __N_11__,
    __logicalOp__,
    __relationalOp__,
    __declareStmt__,
    __conditionalStmt__,
    __caseStmts__,
    __caseStmt__,
    __value__,
    __default__,
    __iterativeStmt__,
    __forrange__,
    __range__,
    __var2__,
    __range_arr__,
    __index_arr__,
    __new_index__
};
typedef enum NONTERMINAL NONTERMINAL;

typedef union TNT
{
    TOKEN tok;
    NONTERMINAL nonterm;
} TNT;

struct hash_table_element
{
    enum TNT_TYPE type;
    TNT tnt;
};
typedef struct hash_table_element hash_table_element;

typedef struct parse_tree parse_tree;
typedef struct parse_tree_node parse_tree_node;
typedef struct linked_list linked_list;
typedef struct linked_list_node linked_list_node;

#endif