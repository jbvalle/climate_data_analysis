#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    int datum;
    double t_min;
    double t_max;
    double niederschlag;
}cdatas_t;

typedef struct {
    //filter_args_t args[0]...Parameter für Datum
    //filter_args_t args[1]...Parameter für t_min
    //filter_args_t args[2]...Parameter für t_max
    //filter_args_t args[3]...Parameter für niederschlag
    //methode...Filtermethode
    //arg1...limit1
    //arg2...limit2
    int method;
    double arg1;
    double arg2;
}filter_args_t;

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


void filter_tmin(cdatas_t *cdata, int *number_datasets, int filter_methode, double t_min_limit1, double t_min_limit2)
{
    
    int new_index = 0;
    int temp_num_datasets = *number_datasets;

    switch(filter_methode){
        case 0:
            for(int i = 0 ; i <= *number_datasets; i++)
            {
                if(cdata[i].t_min < t_min_limit1)
                {
                    //printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;
            }


            break;

        case 1:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].t_min > t_min_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }

            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;
            }
            break;

        case 2:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if((cdata[i].t_min > t_min_limit1)&&(cdata[i].t_min < t_min_limit2))
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}


void filter_tmax(cdatas_t *cdata, int *number_datasets, int filter_methode, double t_max_limit1, double t_max_limit2)
{

    //Wenn Filter Daten übereinstimmen -> Überschreiben der datensätze durchverschieben des Arrays
    //Index des neuen Datensatzes
    int new_index = 0;
    int temp_num_datasets = *number_datasets;

    switch(filter_methode){
        case 0:
            for(int i = 0; i <= *number_datasets; i++)
            {
                //Wenn Daten mit Filter übereinstimmen überschreiben der alten datensätze durch verschieben
                if(cdata[i].t_max < t_max_limit1)
                {
                    //printf("%15d %5lf %5lf %5lf\n",cdata[i].datum, cdata[i].t_min,cdata[i].t_max, cdata[i].niederschlag);
                
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 1:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].t_max > t_max_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 2:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if((cdata[i].t_max > t_max_limit1)&&(cdata[i].t_max < t_max_limit2))
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void filter_niederschlag(cdatas_t *cdata, int *number_datasets, int filter_methode, double niederschlag_limit1, double niederschlag_limit2)
{
    //Wenn Filter Daten übereinstimmen -> Überschreiben der datensätze durchverschieben des Arrays
    //Index des neuen Datensatzes
    int new_index = 0;
    int temp_num_datasets = *number_datasets;


    switch(filter_methode){
        case 0:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].niederschlag < niederschlag_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 1:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].niederschlag > niederschlag_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 2:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if((cdata[i].niederschlag > niederschlag_limit1)&&(cdata[i].niederschlag < niederschlag_limit2))
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void filter_datum(cdatas_t *cdata, int *number_datasets, int filter_methode, int datum_limit1, int datum_limit2)
{
    //Wenn Filter Daten übereinstimmen -> Überschreiben der datensätze durchverschieben des Arrays
    //Index des neuen Datensatzes
    int state;
    int new_index = 0;
    int temp_num_datasets = *number_datasets;

    switch(filter_methode){
        case 0:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].datum < datum_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 1:
            for(int i = 0; i <= *number_datasets; i++)
            {
                if(cdata[i].datum > datum_limit1)
                {
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                }
            }
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 2:
            //int avrg = 0;
            //int zeitraum = 0;
            for(int i = 0; i <= *number_datasets; i++)
            {
                if((cdata[i].datum >= datum_limit1)&&(cdata[i].datum <= datum_limit2))
                {
                    //zeitraum++;
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                    //avrg = avrg + cdata[i].niederschlag;
                }
            }
            
            //avrg = avrg / zeitraum;
            //printf(Der Mittel von Zeitraum: XX -YYY : %d,avrg);
            //
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        case 3:
            
            //Die letzten N Tage
            //void filter_datum(cdatas_t *cdata, int *number_datasets, int filter_methode, int datum_limit1, int datum_limit2);
            //datum_limit1 wird zu letzte N Tage
            //datum_limit2 wird zu letzte N Tage bis ZZ Datum | Wenn datum_limit2 == 0 wird ignoriert
            //Wenn datum_limit2 einen Wert besitzt dann state = 0 -> equivalent mit if(cdata < datum)
            //Wenn datum_limit2 gleich 0 ist dann state = 1 -> equivalent mit if(1) datum wird nicht beachtet
            state = (datum_limit2 != 0) ? 0 : 1;

            for(int i = *number_datasets - datum_limit1 + 1; i <= *number_datasets; i++)
            {
                if((cdata[i].datum <= datum_limit2)||state)
                {
                    //zeitraum++;
                    cdata[new_index].datum = cdata[i].datum;
                    cdata[new_index].t_min = cdata[i].t_min;
                    cdata[new_index].t_max = cdata[i].t_max;
                    cdata[new_index].niederschlag = cdata[i].niederschlag;
                    new_index++;
                    //avrg = avrg + cdata[i].niederschlag;
                }
            }
            
            //avrg = avrg / zeitraum;
            //printf(Der Mittel von Zeitraum: XX -YYY : %d,avrg);
            //
            //Restliche "Leere Datensätze" mit 0 auffüllen
            *number_datasets = new_index - 1;

            while(new_index < temp_num_datasets)
            {
                cdata[new_index].datum = 0;
                cdata[new_index].t_min = 0;
                cdata[new_index].t_max = 0;
                cdata[new_index].niederschlag = 0;
                new_index++;

            }
            break;

        default:
            printf("ERROR: filter_tmin: invalid filter methode");
            break;
    }
}

void show_data(cdatas_t *cdata, int number_datasets){

    char *token, letter, buff[50];
    int input[] = {0,0,0,0};

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';


    printf("+------------------------+\n");
    printf("| Ausgabeoptionen:       |\n");
    printf("|* datum                 |\n");
    printf("|* t_min                 |\n");
    printf("|* t_max                 |\n");
    printf("|* niederschlag          |\n");
    printf("|* gesamt                |\n");
    printf("|                        |\n");
    printf("| z.B datum, t_max       |\n");
    printf("+------------------------+\n\n");
    printf("Eingabe: ");

    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;
    
    token = strtok(buff," ,\n\t");
    
    while(token != NULL){

        if(strcmp(token,"gesamt")==0)for(int i = 0; i < 4; i++)input[i] = 1;
        if(strcmp(token,"datum")==0)input[0] = 1;
        if(strcmp(token,"t_min")==0)input[1] = 1;
        if(strcmp(token,"t_max")==0)input[2] = 1;
        if(strcmp(token,"niederschlag")==0)input[3] = 1;

        token = strtok(NULL," ,\n\t");
    }

    for(int i = 0; i <= number_datasets; i++)
    {
        if(input[0]){
            int yy = (int)cdata[i].datum/10000;
            int mm = ((int)cdata[i].datum - (yy * 10000)) / 100;
            int dd = ((int)cdata[i].datum - (yy * 10000) - (mm * 100));
            printf("%02d-%02d-%4d  ", dd, mm, yy);
        }
        if(input[1])printf("%04.1lf  ", cdata[i].t_min);
        if(input[2])printf("%04.1lf  ", cdata[i].t_max);
        if(input[3])printf("%04.1lf  ", cdata[i].niederschlag);
        printf("\n");
    }
}

void request_datum_args(filter_args_t *args){

    
    char *token, letter, buff[50], key[5];
    //Input dient zur fall unterscheidung und zur "aktivierung" der auswahloption
    int input[] = {0,0,0,0};
    //Variablen fuer die eingabe eines zeitraums
    int dd1,mm1,yy1;
    int dd2,mm2,yy2;

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';

    printf("+-------------------------------------------------------------------+\n");
    printf("| Filterung nach Datum: (Datensaetze vom 01.01.1948 - 31.12.2009)   |\n");
    printf("|                                                                   |\n");
    printf("| Bitte waehlen Sie eines der moeglichen Filtermethoden aus:        |\n");
    printf("|* datum TT.MM.JJJJ - TT.MM.JJ           ->Filterung eines Zeitraums|\n");
    printf("|* bis TT.MM.JJJJ                        ->alle Daten bis ...       |\n"); 
    printf("|* ab TT.MM.JJJJ                         ->alle Daten ab  ...       |\n");
    printf("|* 1. letzten X Tage                                                |\n");
    printf("|* 2. letzten X Tage bis TT.MM.JJJJ                                 |\n");
    printf("|                                                                   |\n");
    printf("| z.B. datum 01.01.1995 - 01.01.2000                                |\n");
    printf("| z.B. bis 01.01.1950                                               |\n");
    printf("| z.B. ab 10.10.2005                                                |\n");
    printf("| z.B. 1. letzten 50 Tage                                           |\n");
    printf("| z.B. 2. letzten 100 Tage bis 10.10.2019                           |\n");
    printf("+-------------------------------------------------------------------+\n\n");
    printf("Eingabe: ");

    //Zeichenweises einlesen
    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;

    //Einlesen vom schluesselwort zur fallunterscheidung
    sscanf(buff,"%s",key);

    //Array zum auffangen vom key
    char dump[5];

    //Wenn key == datum dann 1. Filteroption ausführen
    if(strcmp(key, "datum")==0){
        
        //Entspricht filtermethode 2 filter_datum(2,limit1,limit2)
        sscanf(buff, "%s %d.%d.%d - %d.%d.%d", dump,&dd1, &mm1, &yy1,&dd2,&mm2,&yy2);
        //Filtermethode 2 weahlen
        args[0].method = 2;
        //mm,dd,yy in das format YYYYMMDD umwandeln
        args[0].arg1 = (yy1 * 10000 + mm1 * 100 + dd1);
        args[0].arg2 = (yy2 * 10000 + mm2 * 100 + dd2);
    }


    if(strcmp(token, "bis")  ==0){
        printf("2");
    }

    if(strcmp(token, "ab")   ==0)printf("3");

    if(strcmp(token, "1")    ==0)printf("4");

    if(strcmp(token, "2")    ==0)printf("5");

}

void get_input(filter_args_t *args, int *filter_option){

    char *token, letter, buff[50];

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';


    printf("+---------------------------------------------------+\n");
    printf("| Bitten geben Sie die gewuenschten Filter ein      |\n");
    printf("|* datum            ->Filterung nach Datum          |\n");
    printf("|* t_min            ->Filterung nach t_min          |\n"); 
    printf("|* t_max            ->Filterung nach t_max          |\n");
    printf("|* niederschlag     ->Filterung nach niederschlag   |\n");
    printf("|                                                   |\n");
    printf("| z.B. datum, t_max, niederschlag                   |\n");
    printf("+---------------------------------------------------+\n\n");
    printf("Eingabe: ");

    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;
    
    token = strtok(buff," ,\n\t");
    
    while(token != NULL){

        if(strcmp(token,"datum")==0){
            //Auffüllen der Parameter Daten mit Datum Filter Werten
            filter_option[0] = 1;
            request_datum_args(args);
        }
        if(strcmp(token,"t_min")==0)filter_option[1] = 1;
        if(strcmp(token,"t_max")==0)filter_option[2] = 1;
        if(strcmp(token,"niederschlag")==0)filter_option[3] = 1;

        token = strtok(NULL," ,\n\t");
    }


}


int main(void){
    
    //Filteroptionen dient der "aktivierung" der filter
    int filter_option[4] = {0,0,0,0};
    FILE *input;
    int number_datasets = 0, padding = 16;
    //determine number of datasets -> number_datasets
    input = fopen("input.csv","r");

    //Anzahl Datensaetze ermitteln
    number_datasets = process_input(input, number_datasets,padding);
    
    //Allocate Pointer of size number_datasets
    cdatas_t *cdata = (cdatas_t *)malloc(number_datasets * sizeof(cdatas_t));

    //Initialize input param variables
    filter_args_t *args = (filter_args_t *)malloc(4 * sizeof(filter_args_t));
    int filter_options[4] = {0,0,0,0};
    
    //Parse dataset arguments to array of structs
    input = fopen("input.csv","r");
    parse_input_to_struct(input, cdata, padding);

    get_input(args, filter_option);

    if(filter_option[0])filter_datum(cdata, &number_datasets, args[0].method, args[0].arg1, args[0].arg2);
    //if(filter_option[1])filter_tmax(cdata, &number_datasets, 1, 8, 30);
    //if(filter_option[2])filter_tmin(cdata, &number_datasets, 1, 3, 30);
    //if(filter_option[3])filter_niederschlag(cdata, &number_datasets, 1, 5, 30);
    
    show_data(cdata, number_datasets);

    free(cdata);
    return 0;
}

