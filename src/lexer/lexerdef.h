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
    FALSE
    // TODO: add all tokens
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