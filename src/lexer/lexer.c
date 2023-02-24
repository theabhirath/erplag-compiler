#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexerdef.h"

#define BUFFER_SIZE 32
#define NUM_RESERVED_WORDS 30
#define HASH_TABLE_SIZE 128

// /* hash function for the reserved words table */
// int hashRW(char *word)
// {
//     int hash = 0;
//     for (int i = 0; i < strlen(word); i++)
//     {
//         hash = (word[i] + hash * 43) % HASH_TABLE_SIZE;
//     }
//     return hash;
// }

// /* reserved words table */
// int reservedWordsTable()
// {
//     // allocate memory for the reserved words table
//     struct reserved_word *arr = (struct reserved_word *)malloc(sizeof(struct reserved_word) * NUM_RESERVED_WORDS);

//     int i = 0;
//     char delim[] = " \n";
//     while (fgets(line, 100, fp) != NULL)
//     {
//         char *token = strtok(line, delim);
//         arr[i].word = (char *)malloc(sizeof(char) * strlen(token));
//         strcpy(arr[i].word, token);
//         token = strtok(NULL, delim);
//         arr[i].token = i;
//         i++;
//     }

//     struct reserved_word hash_table[HASH_TABLE_SIZE];
//     for (int i = 0; i < HASH_TABLE_SIZE; i++)
//     {
//         hash_table[i].word = NULL;
//         hash_table[i].token = -1;
//     }
//     int numCollisions = 0;
//     int minCollisions = 30;
//     int minTerm = 0;
//     for (int i = 0; i < NUM_RESERVED_WORDS; i++)
//     {
//         int index = hash(arr[i].word);
//         if (hash_table[index].word == NULL)
//         {
//             hash_table[index].word = arr[i].word;
//             hash_table[index].token = arr[i].token;
//         }
//         else
//         {
//             numCollisions++;
//             while (hash_table[index].word != NULL)
//             {
//                 index = (index + 1) % HASH_TABLE_SIZE;
//             }
//             hash_table[index].word = arr[i].word;
//             hash_table[index].token = arr[i].token;
//         }
//     }
//     printf("Number of collisions: %d\n", numCollisions);

//     return 0;
// }

// declare global buffers
char buf1[BUFFER_SIZE], buf2[BUFFER_SIZE];
// pointers
int start, finish;

// line number
int lineNumber = 0;

// brings fixed size of input into memory. use two buffers
FILE *getStream(FILE *fp, char *buf1, char *buf2)
{
    // TODO
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

tokenInfo getNextToken()
{
    // token to be returned
    tokenInfo token;

    // get character from buffer
    char c = buf1[start];
    int state = 0;

    while (state >= 0)
    {
        c = buf1[finish];
        switch (state)
        {
            // start
        case 0:
            if (c == ' ' || c == '\t')
            {
                start++;
                finish++;
                break;
            }
            else if (c == '\n')
            {
                start++;
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
            // TODO reserved words table lookup
            token.tok = ID;
            token.lineNumber = lineNumber;
            token.val.idValue = (char *)malloc(sizeof(char) * (finish - start + 1));
            strncpy(token.val.idValue, buf1 + start, finish - start);

        case 3:
            // TODO
            break;
        }
    }
}