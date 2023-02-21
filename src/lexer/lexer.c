#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32

// brings fixed size of input into memory. use two buffers
FILE *getStream(FILE *fp, char *buf1, char *buf2){
   // TODO
   return fp;
}

// takes source code, writes clean code to file without comments
// number of lines in clean code is equal to number of lines in source code
// a comment is any number of characters between ** and **
void removeComments(char *test_file, char *clean_file){
    FILE *fp1 = fopen(test_file, "r");
    FILE *fp2 = fopen(clean_file, "w");
    char c;
    // 0 = no star, 1 = star, 2 = star star
    // if 2, then comment
    int stars = 0;
    int comment = 0;
    while ((c = fgetc(fp1)) != EOF){
        if (c == '\n'){
            fputc(c, fp2);
            continue;
        }
        if (comment == 0){
            fputc(c, fp2);
        }
        if (stars == 0){
            if (c == '*'){
                stars = 1;
            }
        }
        else if (stars == 1){
            if (c == '*' && comment == 0){
                stars = 2;
                comment = 1;
                // go back two characters and remove the last two characters
                fseek(fp2, -2, SEEK_CUR);
            }
            else if (c == '*' && comment == 1){
                stars = 0;
                comment = 0;
            }
            else{
                stars = 0;
            }
        }
        else if (stars == 2){
            if (c == '*'){
                stars = 1;
            }
        }
    }
    fclose(fp1);
    fclose(fp2);
}

void getNextToken(){
    // TODO
}