#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){

    FILE *input = fopen("input.csv","r");

    char *token, input_argument[60], notch_argument[2] = ";";

    //Liest 17 Zeilen bis zu Beginn der Datensätze
    for(int i = 0; i < 17; i++)fgets(input_argument,60,input);

    token = strtok(input_argument,notch_argument);

    while(token != NULL){

        printf("%s \n", token);
        token = strtok(NULL,notch_argument);
    }
    return 0;
}
