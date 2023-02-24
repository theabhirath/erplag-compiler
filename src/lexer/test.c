#include <stdio.h>

int main(void){
    int x = 7;
    switch(x){
        case 2 | 5:
            printf("1 and 5");
            break;

        default:
            printf("default");
            break;
    }
}
