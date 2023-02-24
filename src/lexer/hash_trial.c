#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};

struct reserved_word
{
    char *word;
    enum TOKEN token;
};

#define NUM_RESERVED_WORDS 30
#define HASH_TABLE_SIZE 128

int hash(char *word)
{
    int hash = 0;
    for (int i = 0; i < strlen(word); i++)
    {
        hash = (word[i] + hash * 43) % HASH_TABLE_SIZE;
    }
    return hash;
}
int main()
{
    FILE *fp;
    char line[100];
    fp = fopen("reserved_words.csv", "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    struct reserved_word *arr = (struct reserved_word *)malloc(sizeof(struct reserved_word) * NUM_RESERVED_WORDS);
    int i = 0;
    char delim[] = " \n";
    while (fgets(line, 100, fp) != NULL)
    {
        char *token = strtok(line, delim);
        arr[i].word = (char *)malloc(sizeof(char) * strlen(token));
        strcpy(arr[i].word, token);
        token = strtok(NULL, delim);
        arr[i].token = i;
        i++;
    }
    fclose(fp);

    struct reserved_word hash_table[HASH_TABLE_SIZE];
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hash_table[i].word = NULL;
        hash_table[i].token = -1;
    }
    int numCollisions = 0;
    int minCollisions = 30;
    int minTerm = 0;
    for (int i = 0; i < NUM_RESERVED_WORDS; i++)
    {
        int index = hash(arr[i].word);
        if (hash_table[index].word == NULL)
        {
            hash_table[index].word = arr[i].word;
            hash_table[index].token = arr[i].token;
        }
        else
        {
            numCollisions++;
            while (hash_table[index].word != NULL)
            {
                index = (index + 1) % HASH_TABLE_SIZE;
            }
            hash_table[index].word = arr[i].word;
            hash_table[index].token = arr[i].token;
        }
    }
    printf("Number of collisions: %d\n", numCollisions);

    return 0;
}
