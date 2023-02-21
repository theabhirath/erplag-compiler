#ifndef PARSER_H
#define PARSER_H
#include "../lexer/lexerdef.h"
#include "../lexer/lexer.h"
#define NUM_TOKENS 57
#define NUM_NONTERMINALS 67
#define NUM_RULES 119
typedef struct token_set token_set;
typedef struct parse_tree parse_tree;
typedef struct parse_tree_node parse_tree_node;
typedef struct linked_list linked_list;
typedef struct linked_list_node linked_list_node;
int isMember(token_set *set, TOKEN tok);
extern int Table[NUM_NONTERMINALS][NUM_TOKENS];
enum NONTERMINAL
{
    <program>,
    <moduleDeclarations>,
    <moduleDeclaration>,
    <otherModules>,
    <driverModule>,
    <module>,
    <ret>,
    <input_plist>,
    <iP0>,
    <output_plist>,
    <oP0>,
    <dataType>,
    <type>,
    <moduleDef>,
    <statements>,
    <statement>,
    <ioStmt>,
    <var3>,
    <var>,
    <whichId>,
    <wI0>,
    <simpleStmt>,
    <assignmentStmt>,
    <whichStmt>,
    <lvalueIDStmt>,
    <lvalueARRStmt>,
    <element_index_with_expression>,
    <sign>,
    <N_10>,
    <arrExpr>,
    <arr_N4>,
    <arrTerm>,
    <arr_N5>,
    <arrFactor>,
    <id_num_rnum>,
    <moduleReuseStmt>,
    <optional>,
    <idList>,
    <iL0>,
    <expression>,
    <booleanExpr>,
    <BE00>,
    <BE0>,
    <arithmeticExpr>,
    <AE0>,
    <term>,
    <T0>,
    <op1>,
    <op2>,
    <bools>,
    <factor>,
    <N_11>,
    <logicalOp>,
    <relationalOp>,
    <declareStmt>,
    <conditionalStmt>,
    <caseStmts>,
    <caseStmt>,
    <value>,
    <default>,
    <iterativeStmt>,
    <forrange>,
    <range>,
    <var2>,
    <range_arr>,
    <index_arr>,
    <new_index>
};
typedef enum NONTERMINAL NONTERMINAL;
typedef union TNT
{
    TOKEN tok;
    NONTERMINAL nonterm;
} TNT;
void computeFirstAndFollowSets(token_set *firstSet, token_set *followSet, linked_list *rules);
void populateParseTable(token_set *firstSet, token_set *followSet, linked_list *rules);
void parseInputSourceCode(char *testcaseFile, char *grammarFile);
void printParseTree(parse_tree *tree, FILE *fp);
linked_list *createRuleList(char *grammarFile);

#endif