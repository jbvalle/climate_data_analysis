#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reset_temp(char *temp, int length){

    for(int i = 0; i < length; i++)temp[i] = '\0';
}

typedef struct {

    char datum[15];
    double t_min;
    double t_max;
    double niederschlag;
}cdatas_t;


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

void parse_input_to_struct(FILE *input, cdatas_t *cdata){
    
    //Notch_argument -> Zeichen bei dem der String zerteilt wird
    //bei ; und bei NEWLINE am ende der Zeile
    char *input_argument, temp[100], notch_argument[3] = ";\n";

    //Initialisiere temp array
    reset_temp(temp,sizeof(temp));

    //Liest 17 Zeilen bis zu Beginn der Datensätze und übergibt zeile an Temp
    for(int i = 0; i < 16; i++)fgets(temp,sizeof(temp),input);

    //fgets liest input stream und überschreibt temp-array der laenge von temp
    //Wenn NULL -> END OF FILE -> Dann beende die Schleife
    for(int i = 0; fgets(temp,sizeof(temp),input) != NULL; i++){

        //Ein Argument einer Datensatzzeile wird bis ';' oder '\n' auf input_argument übergeben
        input_argument = strtok(temp,notch_argument);

        for(int j = 0; (j < 4)&&(input_argument != NULL); j++){


            switch(j){

                case 0:
                    strcpy(cdata[i].datum,input_argument);
                    break;
                case 1:
                    cdata[i].t_min = strtod(input_argument,NULL);
                    break;
                case 2:
                    cdata[i].t_max = strtod(input_argument,NULL);
                    break;    
                case 3:
                    cdata[i].niederschlag = strtod(input_argument,NULL);
                    break;
            }
            
            input_argument = strtok(NULL,notch_argument);

        }
        //inhalt von temp wird geleert
        reset_temp(temp,sizeof(temp));
    }
    //Input_arguments freigeben
    //free(input_argument);
}

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
