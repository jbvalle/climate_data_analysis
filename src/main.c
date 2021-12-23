#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){

    FILE *input = fopen("input.csv","r");

    char *input_argument, temp[60], notch_argument[2] = ";";

    //Liest 17 Zeilen bis zu Beginn der Datensätze
    for(int i = 0; i < 17; i++)fgets(temp,60,input);

    input_argument = strtok(temp,notch_argument);

    while(input_argument != NULL){

        printf("%s \n", input_argument);
        input_argument = strtok(NULL,notch_argument);
    }
    return 0;
}
