#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    int datum;
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
                {
                    //Einlesen vom Datum in 2 temporäre char pointer
                    char *pread  = input_argument;
                    char *pwrite = input_argument;
                    int x = 0;//y = 0;
                    //Ueberschreibt pwrite mit jedem zeichen von pread solange
                    //es nicht mit '-' uebereinstimmt 
                    while(*pread){
                        //pwrite[x] = pread[y++]
                        *(pwrite + x) = *pread++;
                        //x = x + (logische Aussage)
                        x += (*(pwrite + x) != '-');
                    }
                    *(pwrite + x) = '\0';
                    
                    //Umwandeln vom Datum in ein int
                    cdata[i].datum = strtol(pwrite,NULL,10);
                    break;
                }

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


void filter_tmin(cdatas_t *cdata, int number_datasets, int filter_methode, double t_min_limit1, double t_min_limit2)
{

    switch(filter_methode){
        case 0:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].t_min < t_min_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 1:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].t_min > t_min_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 2:
            for(int i = 0; i <= number_datasets; i++)
            {
                if((cdata[i].t_min > t_min_limit1)&&(cdata[i].t_min < t_min_limit2))
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}


void filter_tmax(cdatas_t *cdata, int number_datasets, int filter_methode, double t_max_limit1, double t_max_limit2)
{

    switch(filter_methode){
        case 0:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].t_max < t_max_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 1:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].t_max > t_max_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 2:
            for(int i = 0; i <= number_datasets; i++)
            {
                if((cdata[i].t_max > t_max_limit1)&&(cdata[i].t_max < t_max_limit2))
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void filter_niederschlag(cdatas_t *cdata, int number_datasets, int filter_methode, double niederschlag_limit1, double niederschlag_limit2)
{

    switch(filter_methode){
        case 0:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].niederschlag < niederschlag_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 1:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].niederschlag > niederschlag_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 2:
            for(int i = 0; i <= number_datasets; i++)
            {
                if((cdata[i].niederschlag > niederschlag_limit1)&&(cdata[i].niederschlag < niederschlag_limit2))
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void filter_datum(cdatas_t *cdata, int number_datasets, int filter_methode, int datum_limit1, int datum_limit2)
{
    switch(filter_methode){
        case 0:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].datum < datum_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 1:
            for(int i = 0; i <= number_datasets; i++)
            {
                if(cdata[i].datum > datum_limit1)
                {
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                }
            }
            break;

        case 2:
            //int avrg = 0;
            //int zeitraum = 0;
            for(int i = 0; i <= number_datasets; i++)
            {
                if((cdata[i].datum >= datum_limit1)&&(cdata[i].datum <= datum_limit2))
                {
                    //zeitraum++;
                    printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                    //avrg = avrg + cdata[i].niederschlag;
                }
            }
            
            //avrg = avrg / zeitraum;
            //printf(Der Mittel von Zeitraum: XX -YYY : %d,avrg);
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void show_data(cdatas_t *cdata, int number_datasets){

    for(int i = 0; i <= number_datasets; i++)
    {
        printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
    }
}
int main(void){
    
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

    filter_datum(cdata, number_datasets, 2, 19480201, 19480230);
    //filter_tmax(cdata, number_datasets, 2, 10, 30);
    //filter_tmin(cdata, number_datasets, 2, 10, 30);
    //filter_niederschlag(cdata, number_datasets, 2, 10, 30);
    
    //show_data(cdata, number_datasets);
    free(cdata);
    return 0;
}

