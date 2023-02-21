#include "../src/lexer/lexer.c"

// driver to test removeComments
int main(){
    removeComments("test.txt", "clean.txt");
    return 0;
}