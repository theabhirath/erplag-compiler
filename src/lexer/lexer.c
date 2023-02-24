#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexerdef.h"

#define BUFFER_SIZE 32
#define NUM_RESERVED_WORDS 30
#define HASH_TABLE_SIZE 128

// declare global buffers
char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE];
// pointers
int begin, finish;

// line number
int lineNumber = 0;

// brings fixed size of input into memory. use two buffers
FILE *getStream(FILE *fp, char *buf1, char *buf2)
{
    // TODO: implement this function
    return fp;
}

// takes source code, writes clean code to file without comments
// number of lines in clean code is equal to number of lines in source code
// a comment is any number of characters between ** and **
void removeComments(char *test_file, char *clean_file)
{
    FILE *fp1 = fopen(test_file, "r");
    FILE *fp2 = fopen(clean_file, "w");
    char c;
    // 0 = no star, 1 = star, 2 = star star
    // if 2, then comment
    int stars = 0;
    int comment = 0;
    while ((c = fgetc(fp1)) != EOF)
    {
        if (c == '\n')
        {
            fputc(c, fp2);
            continue;
        }
        if (comment == 0)
        {
            fputc(c, fp2);
        }
        if (stars == 0)
        {
            if (c == '*')
            {
                stars = 1;
            }
        }
        else if (stars == 1)
        {
            if (c == '*' && comment == 0)
            {
                stars = 2;
                comment = 1;
                // go back two characters and remove the last two characters
                fseek(fp2, -2, SEEK_CUR);
            }
            else if (c == '*' && comment == 1)
            {
                stars = 0;
                comment = 0;
            }
            else
            {
                stars = 0;
            }
        }
        else if (stars == 2)
        {
            if (c == '*')
            {
                stars = 1;
            }
        }
    }
    fclose(fp1);
    fclose(fp2);
}

tokenInfo allocateToken(TOKEN tokenID, char *lexeme, int lineNumber)
{
    tokenInfo token;
    token.tokenID = tokenID;
    token.val.lexValue = (char *)malloc(sizeof(char) * strlen(lexeme));
    strcpy(token.val.lexValue, lexeme);
    token.lineNumber = lineNumber;
    return token;
}

char getCharFromBuffers()
{
    // TODO: implement this function
}

char *getLexemeFromBuffers()
{
    // TODO : implement this function
}

/* TODO:
1. Handle error cases
2. Handle identifier size
3. Handle pointers (begin and finish). Remember to reset them when you are done with a token.
*/
tokenInfo getNextToken()
{
    // token to be returned
    tokenInfo token;

    // get character from buffer
    char c = getCharFromBuffers();
    int state = 0;

    while (state >= 0)
    {
        c = buf1[finish];
        switch (state)
        {
        // begin
        case 0:
            if (c == ' ' || c == '\t')
            {
                begin++;
                finish++;
                break;
            }
            else if (c == '\n')
            {
                begin++;
                finish++;
                lineNumber++;
                break;
            }
            else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_')
            {
                finish++;
                state = 1;
                break;
            }
            else if (c >= '0' && c <= '9')
            {
                finish++;
                state = 3;
                break;
            }
            else
            {
                switch (c)
                {
                case '(':
                    finish++;
                    state = 13;
                    break;
                case ')':
                    finish++;
                    state = 15;
                    break;
                case ':':
                    finish++;
                    state = 17;
                    break;
                case ';':
                    finish++;
                    state = 21;
                    break;
                case ',':
                    finish++;
                    state = 23;
                    break;
                case '[':
                    finish++;
                    state = 25;
                    break;
                case ']':
                    finish++;
                    state = 27;
                    break;
                case '=':
                    finish++;
                    state = 29;
                    break;
                case '*':
                    finish++;
                    state = 34;
                    break;
                case '.':
                    finish++;
                    state = 40;
                    break;
                case '>':
                    finish++;
                    state = 44;
                    break;
                case '!':
                    finish++;
                    state = 52;
                    break;
                case '<':
                    finish++;
                    state = 56;
                    break;
                case '+':
                    finish++;
                    state = 64;
                    break;
                case '-':
                    finish++;
                    state = 66;
                    break;
                case '/':
                    finish++;
                    state = 68;
                    break;
                default:
                    state = 33;
                    break;
                }
            }
            break;
            // State 0 ends here

        // identifiers
        case 1:
            if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9' | c == '_')
            {
                finish++;
                break;
            }
            else
            {
                state = 2;
                break;
            }

        // generate token for identifier
        case 2:
            // TODO: reserved words table lookup? if not, then identifier

        case 3:
            // TODO: implement handling of numbers
            break;

        // bracket open
        case 13:
        case 14:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(BO, lexeme, lineNumber);
            return token;

        // bracket close
        case 15:
        case 16:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(BC, lexeme, lineNumber);
            return token;

        // colon, assignop
        case 17:
            if (c == '=')
            {
                finish++;
                state = 19;
                break;
            }
            else
            {
                state = 18;
                break;
            }

        // colon
        case 18:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(COLON, lexeme, lineNumber);
            return token;

        // assignop
        case 19:
        case 20:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(ASSIGNOP, lexeme, lineNumber);
            return token;

        // semicolon
        case 21:
        case 22:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(SEMICOL, lexeme, lineNumber);
            return token;

        // comma
        case 23:
        case 24:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(COMMA, lexeme, lineNumber);
            return token;

        // square bracket open
        case 25:
        case 26:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(SQBO, lexeme, lineNumber);
            return token;

        // square bracket close
        case 27:
        case 28:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(SQBC, lexeme, lineNumber);
            return token;

        // equal to
        case 29:
            if (c == '=')
            {
                finish++;
                state = 31;
                break;
            }
            else
            {
                state = 30;
                break;
            }

        // error state, lone equal is an invalid token
        case 30:
            // TODO: handle this error state
            break;

        // equal to
        case 31:
        case 32:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(EQ, lexeme, lineNumber);
            return token;

        // error state, invalid character read
        case 33:
            // TODO: handle this error state
            break;

        // multiplication, comment
        case 34:
            if (c == '*')
            {
                finish++;
                state = 36;
                break;
            }
            else
            {
                state = 35;
                break;
            }

        // multiplication
        case 35:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(MUL, lexeme, lineNumber);
            return token;

        // comment
        // TODO: complete comment handling
        case 36:
            if (c == '*')
            {
                // finish++;
                state = 37;
                break;
            }
            else if (c == '\n')
            {
                finish++;
                lineNumber++;
                break;
            }
            else
            {
                finish++;
                break;
            }

        // comment
        case 37:
            if (c == '*')
            {
                state = 38;
                break;
            }
            else if (c == '\n')
            {
                finish++;
                lineNumber++;
                break;
            }
            else
            {
                finish++;
                state = 36;
                break;
            }

        // comment
        case 38:
        case 39:
            // TODO: discard comment

        // rangeop
        case 40:
            if (c == '.')
            {
                finish++;
                state = 42;
                break;
            }
            else
            {
                state = 41;
                break;
            }

        // error state, lone dot is an invalid token
        case 41:
            // TODO: handle this error state

        // rangeop
        case 42:
        case 43:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(RANGEOP, lexeme, lineNumber);
            break;

        // greater than, greater than or equal to, enddef, driverenddef
        case 44:
            if (c == '=')
            {
                finish++;
                state = 50;
                break;
            }
            else if (c == '>')
            {
                finish++;
                state = 48;
                break;
            }
            else
            {
                state = 45;
                break;
            }

        // greater than
        case 45:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(GT, lexeme, lineNumber);
            break;
        
        // enddef, driverenddef
        case 46:
            if (c == '>')
            {
                finish++;
                state = 48;
                break;
            }
            else
            {
                state = 47;
                break;
            }

        // enddef
        case 47:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(ENDDEF, lexeme, lineNumber);
            return token;

        // driverenddef
        case 48:
        case 49:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(DRIVERENDDEF, lexeme, lineNumber);
            return token;

        // greater than or equal to
        case 50:
        case 51:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(GE, lexeme, lineNumber);
            return token;

        // not equal to
        case 52:
            if (c == '=')
            {
                finish++;
                state = 54;
                break;
            }
            else
            {
                state = 53;
                break;
            }

        // error state, lone exclamation mark is an invalid token
        case 53:
            // TODO: handle this error state

        // not equal to
        case 54:
        case 55:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(NE, lexeme, lineNumber);
            return token;

        // less than, less than or equal to, def, driverdef
        case 56:
            if (c == '=')
            {
                finish++;
                state = 58;
                break;
            }
            else if (c == '<')
            {
                finish++;
                state = 60;
                break;
            }
            else
            {
                state = 57;
                break;
            }

        // less than
        case 57:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(LT, lexeme, lineNumber);
            return token;

        // less than or equal to
        case 58:
        case 59:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(LE, lexeme, lineNumber);
            return token;

        // def, driverdef
        case 60:
            if (c == '<')
            {
                finish++;
                state = 62;
                break;
            }
            else
            {
                state = 61;
                break;
            }

        // def
        case 61:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(DEF, lexeme, lineNumber);
            return token;

        // driverdef
        case 62:
        case 63:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(DRIVERDEF, lexeme, lineNumber);
            return token;

        // plus
        case 64:
        case 65:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(PLUS, lexeme, lineNumber);
            return token;

        // minus
        case 66:
        case 67:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(MINUS, lexeme, lineNumber);
            return token;

        // division
        case 68:
        case 69:
            char *lexeme = getLexemeFromBuffer();
            token = allocateToken(DIV, lexeme, lineNumber);
            return token;
        }
    }
}