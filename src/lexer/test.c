#include <stdio.h>
#include <ctype.h>

int main(void){
    int x = 7;
    switch(x){
        case 2 | 5:
            printf("1 and 5\n");
            break;

        default:
            printf("default");
            break;
    }
    printf("%d\n", isspace('\t'));
}
