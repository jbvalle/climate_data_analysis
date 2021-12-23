#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cdatas_t.h"
#include "../include/process_input.h"
#include "../include/parse_input_to_struct.h"

int main(void){

    FILE *input;
    int number_datasets = 0, padding = 16;
    //determine number of datasets -> number_datasets
    input = fopen("input.csv","r");
    process_input(input, &number_datasets,padding);
    
    //Allocate Pointer of size number_datasets
    cdatas_t *cdata = (cdatas_t *)malloc(number_datasets * sizeof(cdatas_t));
    
    //Parse dataset arguments to array of structs
    input = fopen("input.csv","r");
    parse_input_to_struct(input, cdata);


    
    for(int i = 0; i < 8; i++){

        printf("%15s %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
    
    }

    free(cdata);
    return 0;
}
