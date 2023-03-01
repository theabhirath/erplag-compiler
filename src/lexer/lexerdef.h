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
    GE,
    PROGRAMEND,
    EPSILON
};

// While EPSILON is not a token, for the first and follow sets, we use it as a token
// NUM_TOKENS does not include EPSILON
// Currently, NUM_TOKENS is 56 (0-55) and EPSILON is 56
typedef enum TOKEN TOKEN;

// union for storing numerical values - either int or float
union number
{
    int intValue;
    float floatValue;
};

// represents a token
typedef struct tokenInfo
{
    TOKEN tokenID; // the token
    union number val; // the value of the token
    int lineNumber;  // line number of the token
    char *lexeme; // the lexeme of the token
} tokenInfo;

// reserved word struct
struct reserved_word
{
    char *word;
    enum TOKEN tokenID;
};

// reserved words table
extern struct reserved_word rwtable[];

// buffer size
extern int bufferSize;

#endif
