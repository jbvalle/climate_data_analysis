#include <stdlib.h>
#include <stdio.h>

void process_input(FILE *input, int *number_datasets, int padding){
    
    //Determine number of datasets
    char temp[100];

    while(fgets(temp,sizeof(temp),input) != NULL){

        //number_datasets wird inkrementiert bei neuer zeile
        //zum bestimmen der climate_datasetssatzanzahl
        *(number_datasets) = *(number_datasets) + 1;
    }
    *number_datasets = *number_datasets - padding - 1;
}


