#define NUM_RESERVED_WORDS 30
#define HASH_TABLE_SIZE 128

#include <stdio.h>
#include <string.h>

#include "lexerdef.h"

struct reserved_word rwtable[HASH_TABLE_SIZE];

/* hash function for the reserved words table */
int hashRW(char *word)
{
    int hash = 0;
    for (int i = 0; i < strlen(word); i++)
    {
        hash = (word[i] + hash * 43) % HASH_TABLE_SIZE;
    }
    return hash;
}

/* reserved words table */
int reservedWordsTable()
{
    // allocate memory for the reserved words table
    struct reserved_word arr[NUM_RESERVED_WORDS] = {
        {"integer", INTEGER},
        {"real", REAL},
        {"boolean", BOOLEAN},
        {"of", OF},
        {"array", ARRAY},
        {"start", START},
        {"end", END},
        {"declare", DECLARE},
        {"module", MODULE},
        {"driver", DRIVER},
        {"program", PROGRAM},
        {"get_value", GET_VALUE},
        {"print", PRINT},
        {"use", USE},
        {"with", WITH},
        {"parameters", PARAMETERS},
        {"takes", TAKES},
        {"input", INPUT},
        {"returns", RETURNS},
        {"for", FOR},
        {"in", IN},
        {"switch", SWITCH},
        {"case", CASE},
        {"break", BREAK},
        {"default", DEFAULT},
        {"while", WHILE},
        {"AND", AND},
        {"OR", OR},
        {"true", TRUE},
        {"false", FALSE},
    };

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        rwtable[i].word = NULL;
        rwtable[i].tokenID = -1;
    }
    int numCollisions = 0;
    int minCollisions = 30;
    int minTerm = 0;
    for (int i = 0; i < NUM_RESERVED_WORDS; i++)
    {
        int index = hashRW(arr[i].word);
        if (rwtable[index].word == NULL)
        {
            rwtable[index].word = arr[i].word;
            rwtable[index].tokenID = arr[i].tokenID;
        }
        else
        {
            numCollisions++;
            while (rwtable[index].word != NULL)
            {
                index = (index + 1) % HASH_TABLE_SIZE;
            }
            rwtable[index].word = arr[i].word;
            rwtable[index].tokenID = arr[i].tokenID;
        }
    }
    printf("Number of collisions: %d\n", numCollisions);

    return 0;
}

// int isPresentinRWTable(char *word)
// {
//     int index = hashRW(word);
//     if (rwtable[index].word != NULL)
//     {
//         if (strcmp(rwtable[index].word, word) == 0)
//         {
//             return 1;
//         }
//     }
//     return 0;
// }

int getReservedWordToken(char *word)
{
    int index = hashRW(word);
    if (rwtable[index].word != NULL)
    {
        if (strcmp(rwtable[index].word, word) == 0)
        {
            return rwtable[index].tokenID;
        }
    }
    return -1;
}
