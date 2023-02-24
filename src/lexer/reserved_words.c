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
//         arr[i].tokenID = i;
//         i++;
//     }

//     struct reserved_word hash_table[HASH_TABLE_SIZE];
//     for (int i = 0; i < HASH_TABLE_SIZE; i++)
//     {
//         hash_table[i].word = NULL;
//         hash_table[i].tokenID = -1;
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
//             hash_table[index].tokenID = arr[i].tokenID;
//         }
//         else
//         {
//             numCollisions++;
//             while (hash_table[index].word != NULL)
//             {
//                 index = (index + 1) % HASH_TABLE_SIZE;
//             }
//             hash_table[index].word = arr[i].word;
//             hash_table[index].tokenID = arr[i].tokenID;
//         }
//     }
//     printf("Number of collisions: %d\n", numCollisions);

//     return 0;
// }
