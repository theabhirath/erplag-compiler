#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lexer/lexer.h"
#include "parser/parser.h"

// times input 
void printTimeTaken(char *testCaseFile, char *parseTreeFile)
{
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();

    // invoke your lexer and parser here
    parseInputSourceCode(testCaseFile, parseTreeFile);

    end_time = clock();
    total_CPU_time = (double) (end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    // Print both total_CPU_time and total_CPU_time_in_seconds 
    printf("Total CPU time: %lf\n", total_CPU_time);
    printf("Total CPU time in seconds: %lf\n", total_CPU_time_in_seconds);
}

int main(int argc, char *argv[]) 
{
    // check for correct number of arguments
    if (argc < 3) {
        printf("Usage: <testcase.txt> <parsetreeOutFile.txt> <size_of_buffer>\n");
        return 1;
    }
    // get buffer size
    bufferSize = atoi(argv[3]);
    // open source code file
    FILE *fp1 = fopen(argv[1], "r");
    if (fp1 == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    int choice = -1;
    while (choice != 0)
    {
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 0:
                break;
            case 1:
                removeComments(argv[1], "cleaned.txt");
                // print cleaned file
                FILE *fp = fopen("cleaned.txt", "r");
                char *line = NULL;
                size_t len = 0;
                ssize_t read;
                while ((read = getline(&line, &len, fp)) != -1) {
                    printf("%s", line);
                }
                break;
            case 2:
                printTokenList(argv[1]);
                break;
            case 3:
                parseInputSourceCode(argv[1], argv[2]);
                break;
            case 4:
                printTimeTaken(argv[1], argv[2]);
                break;
            default:
                printf("Invalid choice\n. Please choose a number between 0 and 4\n");
                break;
        }
    }
}