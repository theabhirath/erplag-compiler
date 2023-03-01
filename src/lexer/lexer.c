#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexerdef.h"

// #define BUFFER_SIZE 32
#define NUM_RESERVED_WORDS 30
#define HASH_TABLE_SIZE 128

// global buffer size
int bufferSize;

// global two buffers
char *buf1, *buf2;

// begin and finish pointers for the two buffers
int begin, finish;

// line number
int lineNumber = 1;

// initialising twin buffers
void initialiseTwinBuffers()
{
    buf1 = (char *)malloc(sizeof(char) * bufferSize);
    buf2 = (char *)malloc(sizeof(char) * bufferSize);
    bzero(buf1, bufferSize);
    bzero(buf2, bufferSize);
    begin = 2 * bufferSize;
    finish = 2 * bufferSize;
}

// takes source code, writes clean code to file without comments
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

// reset pointers after token generation
void resetPointers()
{
    begin = finish;
}

// brings fixed size of input into memory. use two buffers
FILE *getStream(FILE *fp)
{
    // copy buffer 2 to buffer 1
    strncpy(buf1, buf2, bufferSize);
    int nread = fread(buf2, 1, bufferSize, fp);
    // pointers back to "beginning" of buffer
    begin -= bufferSize;
    finish -= bufferSize;
    // end of file
    if (nread < bufferSize)
    {
        // end of file
        buf2[nread] = EOF;
    }
    return fp;
}

// gets single character from buffer
char getCharFromBuffers(FILE *fp)
{
    if (finish >= 2 * bufferSize)
    {
        getStream(fp);
    }
    if (finish < bufferSize)
        return buf1[finish];
    else if (finish < 2 * bufferSize)
        return buf2[finish - bufferSize];
    
}

// gets lexeme from buffer
char *getLexemeFromBuffers(FILE *fp)
{
    char *lexeme = (char *)malloc(sizeof(char) * (finish - begin + 1));
    int i;
    for (i = begin; i < finish; i++)
    {
        if (i < bufferSize)
            lexeme[i - begin] = buf1[i];
        else
            lexeme[i - begin] = buf2[i - bufferSize];
    }
    lexeme[i - begin] = '\0';
    return lexeme;
}

// returns tokenInfo struct
tokenInfo allocateToken(TOKEN tokenID, int lineNumber, FILE *fp)
{
    tokenInfo token;
    token.tokenID = tokenID;
    token.lexeme = getLexemeFromBuffers(fp);
    token.lineNumber = lineNumber;
    resetPointers();
    return token;
}

// basically the entire lexer; generates tokens one-by-one from source code
tokenInfo getNextToken(FILE *fp)
{
    // token to be returned
    tokenInfo token;

    // declarations
    char c;
    int state = 0;
    char *lexeme;

    while (state >= 0)
    {
        // get character from buffer
        c = getCharFromBuffers(fp);
        switch (state)
        {
        // start state
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
                case EOF:
                    state = 70;
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

        // identifiers and reserved words
        case 2:
            lexeme = getLexemeFromBuffers(fp);
            // check if identifier length exceeds 20 characters
            if (strlen(lexeme) > 20)
            {
                fprintf(stderr, "Error: Identifier length exceeds 20 characters at line %d\n", lineNumber);
                free(lexeme);
                resetPointers();
                state = 0;
                break;
            }
            // check if reserved word by looking up hash table rwtable
            int tok = getReservedWordToken(lexeme);
            token.tokenID = tok == -1 ? ID : tok;
            token.lexeme = lexeme;
            token.lineNumber = lineNumber;
            resetPointers();
            return token;

        case 3:
            if ('0' <= c && c <= '9')
            {
                finish++;
                break;
            }
            else if (c == '.')
            {
                finish++;
                state = 5;
                break;
            }
            else
            {
                state = 4;
                break;
            }

        // number (integer)
        case 4:
            lexeme = getLexemeFromBuffers(fp);
            token.tokenID = NUM;
            token.val.intValue = atoi(lexeme);
            token.lexeme = lexeme;
            token.lineNumber = lineNumber;
            resetPointers();
            return token;

        // reals, both . notation and e notation
        case 5:
            if ('0' <= c && c <= '9')
            {
                state = 8;
                finish++;
                break;
            }
            else if (c == '.')
            {
                state = 7;
                break;
            }
            else
            {
                state = 6;
                break;
            }

        // error state; expected a digit or another . after .
        case 6:
            fprintf(stderr, "Error: Expected a digit or another . after . at line %d\n", lineNumber);
            resetPointers();
            state = 0;
            break;

        // number (integer) followed by range operator, needs to retract
        case 7:
            finish--;
            lexeme = getLexemeFromBuffers(fp);
            token.tokenID = NUM;
            token.val.intValue = atoi(lexeme);
            token.lexeme = lexeme;
            token.lineNumber = lineNumber;
            resetPointers();
            return token;

        // reals, both . notation and e notation
        case 8:
            if ('0' <= c && c <= '9')
            {
                finish++;
                break;
            }
            else if (c == 'e' || c == 'E')
            {
                finish++;
                state = 10;
                break;
            }
            else
            {
                state = 9;
                break;
            }

        // reals, both . notation and e notation
        case 9:
            lexeme = getLexemeFromBuffers(fp);
            token.tokenID = RNUM;
            token.val.floatValue = atof(lexeme);
            token.lexeme = lexeme;
            token.lineNumber = lineNumber;
            resetPointers();
            return token;

        // reals, e notation
        case 10:
            if ('0' <= c && c <= '9')
            {
                finish++;
                state = 12;
                break;
            }
            else if (c == '+' || c == '-')
            {
                finish++;
                state = 11;
                break;
            }
            else
            {
                state = 6;
                break;
            }

        // reals, e notation
        case 11:
            if ('0' <= c && c <= '9')
            {
                finish++;
                state = 12;
                break;
            }
            else
            {
                state = 6;
                break;
            }

        // reals, e notation
        case 12:
            if ('0' <= c && c <= '9')
            {
                finish++;
                break;
            }
            else
            {
                state = 9;
                break;
            }

        // bracket open
        case 13:
        case 14:
            token = allocateToken(BO, lineNumber, fp);
            return token;

        // bracket close
        case 15:
        case 16:
            token = allocateToken(BC, lineNumber, fp);
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
            token = allocateToken(COLON, lineNumber, fp);
            return token;

        // assignop
        case 19:
        case 20:
            token = allocateToken(ASSIGNOP, lineNumber, fp);
            return token;

        // semicolon
        case 21:
        case 22:
            token = allocateToken(SEMICOL, lineNumber, fp);
            return token;

        // comma
        case 23:
        case 24:
            token = allocateToken(COMMA, lineNumber, fp);
            return token;

        // square bracket open
        case 25:
        case 26:
            token = allocateToken(SQBO, lineNumber, fp);
            return token;

        // square bracket close
        case 27:
        case 28:
            token = allocateToken(SQBC, lineNumber, fp);
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

        // error state; lone equal is an invalid token
        case 30:
            fprintf(stderr, "Error: Lone equal is an invalid symbol at line %d. Did you mean to write ':=' or '=='?\n", lineNumber);
            state = 0;
            break;

        // equal to
        case 31:
        case 32:
            token = allocateToken(EQ, lineNumber, fp);
            return token;

        // error state; invalid character read
        case 33:
            fprintf(stderr, "Error: Invalid character '%c' at line %d.\n", c, lineNumber);
            finish++;
            state = 0;
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
            token = allocateToken(MUL, lineNumber, fp);
            return token;

        // comment
        case 36:
            if (c == '*')
            {
                finish++;
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
                finish++;
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
            resetPointers();
            state = 0;
            break;

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

        // error state; lone dot is an invalid token
        case 41:
            fprintf(stderr, "Error: Lone dot is an invalid symbol at line %d. Did you mean to write '..'?\n", lineNumber);
            resetPointers();
            state = 0;
            break;

        // rangeop
        case 42:
        case 43:
            token = allocateToken(RANGEOP, lineNumber, fp);
            return token;

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
                state = 46;
                break;
            }
            else
            {
                state = 45;
                break;
            }

        // greater than
        case 45:
            token = allocateToken(GT, lineNumber, fp);
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
            token = allocateToken(ENDDEF, lineNumber, fp);
            return token;

        // driverenddef
        case 48:
        case 49:
            token = allocateToken(DRIVERENDDEF, lineNumber, fp);
            return token;

        // greater than or equal to
        case 50:
        case 51:
            token = allocateToken(GE, lineNumber, fp);
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

        // error state; lone exclamation mark is an invalid token
        case 53:
            fprintf(stderr, "Error: Lone exclamation mark is an invalid symbol at line %d. Did you mean to write '!='?\n", lineNumber);
            resetPointers();
            state = 0;
            break;

        // not equal to
        case 54:
        case 55:
            token = allocateToken(NE, lineNumber, fp);
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
            token = allocateToken(LT, lineNumber, fp);
            return token;

        // less than or equal to
        case 58:
        case 59:
            token = allocateToken(LE, lineNumber, fp);
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
            token = allocateToken(DEF, lineNumber, fp);
            return token;

        // driverdef
        case 62:
        case 63:
            token = allocateToken(DRIVERDEF, lineNumber, fp);
            return token;

        // plus
        case 64:
        case 65:
            token = allocateToken(PLUS, lineNumber, fp);
            return token;

        // minus
        case 66:
        case 67:
            token = allocateToken(MINUS, lineNumber, fp);
            return token;

        // division
        case 68:
        case 69:
            token = allocateToken(DIV, lineNumber, fp);
            return token;

        // EOF
        case 70:
            token = allocateToken(PROGRAMEND, lineNumber, fp);
            return token;

        // added later; to handle unexpected symbols
        case 71:
            lexeme = getLexemeFromBuffers(fp);
            fprintf(stderr, "Error: Unexpected symbol after '%s' at line %d\n", lexeme, lineNumber);
            free(lexeme);
            resetPointers();
            state = 0;
            break;
        }
    }
    return token;
}

void printTokenList(FILE *fp)
{
    tokenInfo token;
    while (1)
    {
        token = getNextToken(fp);
        if (token.tokenID == PROGRAMEND)
            break;
        printf("Line Number\tToken ID\tLexeme");
        printf("%d\t%d\t%s\n", token.lineNumber, token.lineNumber, token.lexeme);
    }
}

int main()
{
    printf("Enter the buffer size: ");
    scanf("%d", &bufferSize);
    initialiseTwinBuffers();
    reservedWordsTable();
    FILE *fp = fopen("../../tests/t3.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return -1;
    }
    int flag = 1;
    while (flag)
    {
        tokenInfo token = getNextToken(fp);
        if (token.tokenID == PROGRAMEND)
            break;
        printf("%d %d\n", token.tokenID, token.lineNumber);
        if (token.tokenID == NUM){
            printf("%d\n", token.val.intValue);
        } else if (token.tokenID == RNUM){
            printf("%f\n", token.val.floatValue);
        }
        printf("DO you want to continue? (1/0): ");
        scanf("%d", &flag);
    }
    fclose(fp);
    return 0;
}
