#ifndef LEXER_DEF_H
#define LEXER_DEF_H

enum TOKEN
{
    INTEGER,
    REAL,
    BOOLEAN,
    OF,
    ARRAY,
    START,
    END,
    DECLARE,
    MODULE,
    DRIVER,
    PROGRAM,
    GET_VALUE,
    PRINT,
    USE,
    WITH,
    PARAMETERS,
    TAKES,
    INPUT,
    RETURNS,
    FOR,
    IN,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    WHILE,
    AND,
    OR,
    TRUE,
    FALSE,
    DRIVERDEF,
    DEF,
    ID,
    COMMA,
    NUM,
    RNUM,
    SQBO,
    ASSIGNOP,
    PLUS,
    MINUS,
    BO,
    MUL,
    EQ,
    COLON,
    SQBC,
    DIV,
    NE,
    RANGEOP,
    SEMICOL,
    BC,
    LT,
    DRIVERENDDEF,
    ENDDEF,
    GT,
    LE,
    GE
};
typedef enum TOKEN TOKEN;

union Value
{
    int intValue;
    float floatValue;
    char *lexValue;
};

typedef struct tokenInfo
{
    TOKEN tokenID; // the token
    union Value val; // the value of the token
    int lineNumber;  // line number of the token
} tokenInfo;

struct reserved_word
{
    char *word;
    enum TOKEN token;
};

#endif
