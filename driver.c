#include <stdio.h>
#include <stdlib.h>
#include "src/ast/ast.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/ast/symbol_table.h"
#include "src/codegen/codegen.h"
#include <time.h>

void printTimeTaken(char *testCaseFile, char *parseTreeFile)
{
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();

    // invoke your lexer and parser here
    syntacticErrors = 0;
    lexicalErrors = 0;
    parseInputSourceCode(testCaseFile, parseTreeFile);
    if (syntacticErrors || lexicalErrors)
    {
        printf("Compilation failed\n");
    }
    else
    {
        ast *AST;
        AST = create_ast(&parseTree);
        populateSymbolTables(AST);
        destroySymbolTables(&symbolTable);
    }
    end_time = clock();
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    // Print both total_CPU_time and total_CPU_time_in_seconds
    printf("Total CPU time: %lf\n", total_CPU_time);
    printf("Total CPU time in seconds: %lf\n", total_CPU_time_in_seconds);
}

int main(int argc, char *argv[])
{
    int option = 12;
    bufferSize = 1024;
    ast *AST = NULL;
    if (argc != 3)
    {
        printf("Usage: ./driver <inputFile> <codeFile>\n");
        exit(1);
    }
    printf("%s\n", argv[1]);
    while (option != 0)
    {
        printf("0: Exit \n1: Lexer \n2: Parser \n3: AST \n4: Memory \n5. Symbol Table \n6. Activation Record \n7. Static and Dynamic Arrays \n8. Error reporting and Total Compilation Time \n9. Code Generation \n\nEnter your choice: ");
        scanf("%d", &option);
        switch (option)
        {
        case 0:
            return 0;
            break;
        case 1:
            printTokenList(argv[1]);
            break;
        case 2:
            parseInputSourceCode(argv[1], NULL);
            printParseTree(stdout);
            break;
        case 3:
            parseInputSourceCode(argv[1], NULL);
            AST = create_ast(&parseTree);
            print_ast(AST);
            break;
        case 4:
            numParseTreeNodes = 0;
            numASTNodes = 0;
            sizeOfAST = 0;
            parseInputSourceCode(argv[1], NULL);
            AST = create_ast(&parseTree);
            printf("Number of parse tree nodes: %d\n", numParseTreeNodes);
            int parseTreeSize = numParseTreeNodes * sizeof(parse_tree_node) + sizeof(parse_tree);
            printf("Memory used by parse tree: %d bytes\n", parseTreeSize);
            printf("Number of AST nodes: %d\n", numASTNodes);
            printf("Memory used by AST: %d bytes\n", sizeOfAST);
            printf("Compression ratio: %f\n", (parseTreeSize - sizeOfAST) * 100.0f / parseTreeSize);
            break;
        case 5:
            parseInputSourceCode(argv[1], "parseTree.txt");
            AST = create_ast(&parseTree);
            populateSymbolTables(AST);
            printSymbolTable(&symbolTable, 0);
            destroySymbolTables(&symbolTable);
            break;
        case 6:
            parseInputSourceCode(argv[1], "parseTree.txt");
            AST = create_ast(&parseTree);
            populateSymbolTables(AST);
            printActivationRecord();
            destroySymbolTables(&symbolTable);
            break;
        case 7:
            parseInputSourceCode(argv[1], "parseTree.txt");
            AST = create_ast(&parseTree);
            populateSymbolTables(AST);
            printArrs(&symbolTable, 0);
            destroySymbolTables(&symbolTable);
            break;
        case 8:
            printTimeTaken(argv[1], NULL);
            break;
        case 9:
            parseInputSourceCode(argv[1], "parseTree.txt");
            if (syntacticErrors || lexicalErrors)
            {
                printf("Compilation failed\n");
            }
            else
            {
                AST = create_ast(&parseTree);
                populateSymbolTables(AST);
                printf("Generating code...\n");
                generate_code(AST, argv[2]);
                destroySymbolTables(&symbolTable);
            }
            break;
        default:
            printf("Invalid option\n");
        }
    }
}