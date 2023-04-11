#include <stdio.h>
#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../ast/symbol_table.h"
#include "../codegen/codegen.h"

void printTimeTaken(char *testCaseFile, char *parseTreeFile)
{
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();

    // invoke your lexer and parser here

    end_time = clock();
    total_CPU_time = (double) (end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    // Print both total_CPU_time and total_CPU_time_in_seconds 
    printf("Total CPU time: %lf\n", total_CPU_time);
    printf("Total CPU time in seconds: %lf\n", total_CPU_time_in_seconds);
}

int main(int argc, char *argv[])
{
    int option=12;
    while(option!=0)
    {
        printf("0: Exit \n\
                1: Lexer \n\
                2: Parser \n\
                3: AST \n\
                4: Memory \n\
                5. Symbol Table \n\
                6. Activation Record \n\
                7. Static and Dynamic Arrays \n\
                8. Error reporting and Total Compilation Time \n\
                9. Code Generation \n\n\
                Enter your choice: ");
        scanf("%d", &option);
        switch (option)
        {
        case 0:
            exit(0);
            break;
        case 1:
            printTokenList(argv[1]);
            break;
        case 2:
            parseInputSourceCode(argv[1], "parseTree.txt");
            break;
        case 3:
            parseInputSourceCode(argv[1], "parseTree.txt");
            ast *AST = create_AST(&parseTree);
            print_AST(AST);
            break;
        case 4:
            //TODO: memory
            break;
        case 5:
            parseInputSourceCode(argv[1], "parseTree.txt");
            ast *AST = create_AST(&parseTree);
            print_AST(AST);
            populateSymbolTables(AST);
            printSymbolTables(symbolTable, 0);
            break;
        case 6:
            //TODO: activation record
            break;
        case 7:
            //TODO: static and dynamic arrays
            break;
        case 8:
            //TODO: error reporting and total compilation time
            break;
        case 9:
            parseInputSourceCode(argv[1], "parseTree.txt");
            ast *AST = create_AST(&parseTree);
            print_AST(AST);
            populateSymbolTables(AST);
            generate_code(AST, argv[2]);
            break;
        default:
            printf("Invalid option\n");
        }
    }
}