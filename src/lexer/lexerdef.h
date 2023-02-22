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
    ENDEF,
    GT,
    LE,
    GE
};
typedef enum TOKEN TOKEN;
union value
{
    int intValue;
    float floatValue;
    char *idValue;
};
typedef struct token_info
{
    TOKEN tok;       // the token
    union value val; // the value of the token
    int lineNumber;  // line number of the token
} token;
#endif