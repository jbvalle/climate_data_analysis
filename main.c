#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    char datum[15];
    double t_min;
    double t_max;
    double niederschlag;
}cdatas_t;

int process_input(FILE *input, int number_datasets, int padding){
    
    //determine number of datasets
    //temporary
    char temp[50];

    while(fgets(temp,sizeof(temp),input) != NULL){

        //number_datasets wird inkrementiert bei neuer zeile
        //zum bestimmen der climate_datasetssatzanzahl
        number_datasets++;
    }
    number_datasets = number_datasets -  padding - 1; 

    return number_datasets;
}

void reset_temp(char *temp, int length){

    for(int i = 0; i < length; i++)temp[i] = '\0';
}

void parse_input_to_struct(FILE *input, cdatas_t *cdata, int padding){
    
    //Notch_argument -> Zeichen bei dem der String zerteilt wird
    //bei ; und bei NEWLINE am ende der Zeile
    char *input_argument, temp[50], seperator[3] = ";\n";

    //Initialisiere temp array
    reset_temp(temp,sizeof(temp));

    //Liest 17 Zeilen bis zu Beginn der Datensätze und übergibt zeile an Temp
    for(int i = 0; i < padding; i++){
        fgets(temp,sizeof(temp),input);
    }

    //fgets liest input stream und überschreibt temp-array der laenge von temp
    //Wenn NULL -> END OF FILE -> Dann beende die Schleife
    for(int i = 0; fgets(temp,sizeof(temp),input) != NULL; i++){

        //Ein Argument einer Datensatzzeile wird bis ';' oder '\n' auf input_argument übergeben
        input_argument = strtok(temp,seperator);
        
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
            
            input_argument = strtok(NULL,seperator);

        }
        //inhalt von temp wird geleert
        reset_temp(temp,sizeof(temp));

        //Checking for end of file in input stream
        if(feof(input))break;
    }
    //Input_arguments freigeben
    free(input_argument);
}


//void filter_datum(cdatas_t *cdata, int number_datasets,int minimum, int maximum);
//void filter_tmax(cdatas_t *cdata, int number_datasets,int minimum, int maximum);
//void filter_niederschlag(cdatas_t *cdata, int number_datasets,int minimum, int maximum);

/*
void filter_tmin(cdatas_t *cdata, int number_datasets,int *min, int *max){
    
    int *min_cmp = (int*)malloc(sizeof(int));
    int *max_cmp = (int*)malloc(sizeof(int));

    *min_cmp = (min == NULL) ? 0 : *min;
    *max_cmp = (max == NULL) ? 0 : *max;

    for(int i = 0; i <= number_datasets; i++){

        if(((cdata[i].t_min > *min_cmp) || !min) && ((cdata[i].t_min < *max_cmp)||!max) ){


            printf("%15s %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
        }
    }
}*/


void filter_tmin(cdatas_t *cdata,int chosen_t_min, int number_datasets)
{
    for(int i = 0; i <= number_datasets; i++)
    {
        if(cdata[i].t_min < chosen_t_min)
        {
            printf("%15s %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
        }
    }
}

void filter_tmax(cdatas_t *cdata,int chosen_t_max, int number_datasets)
{
    for(int i = 0; i <= number_datasets; i++)
    {
        if(cdata[i].t_max > chosen_t_max)
        {
            printf("%15s %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
        }
    }
}

int main(void){
    
    //Filter Parameters
    int tmin_min = 0;
    int tmin_max = 8;


    //----------------
    FILE *input;
    int number_datasets = 0, padding = 16;
    //determine number of datasets -> number_datasets
    input = fopen("input.csv","r");

    //Anzahl Datensaetze ermitteln
    number_datasets = process_input(input, number_datasets,padding);
    
    //Allocate Pointer of size number_datasets
    cdatas_t *cdata = (cdatas_t *)malloc(number_datasets * sizeof(cdatas_t));
    
    //Parse dataset arguments to array of structs
    input = fopen("input.csv","r");
    parse_input_to_struct(input, cdata, padding);
    //Show Datasets
    /*
    for(int i = 0; i <= number_datasets; i++){

        printf("%15s %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
    }

    printf("\n\n\n");*/

    filter_tmin(cdata, number_datasets, &tmin_min, &tmin_max);
    //filter_tmin(cdata, 4, number_datasets);
    //filter_tmax(cdata, 20, number_datasets);

    free(cdata);
    return 0;
}
