#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define COLOR1 "\033[1;34m"
#define RESET "\033[0m"
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

typedef struct{

    double a;
    double b;
}linearfit_const_t;


int process_input(FILE *input, int number_datasets, int padding){
    
    //determine number of datasets
    //temporary
    char temp[50];

    while(fgets(temp,sizeof(temp),input) != NULL){

        //number_datasets wird inkrementiert bei neuer zeile
        //zum bestimmen der climate_datasetssatzanzahl
        number_datasets++;
    }
    number_datasets = number_datasets -  padding; 

    return number_datasets;
}

void reset_temp(char *temp, int length){

    for(int i = 0; i < length; i++)temp[i] = '\0';
}

void parse_input_to_struct(FILE *input, cdatas_t *cdata, int number_datasets,int padding){
    
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
    for(int i = 0; (fgets(temp,sizeof(temp),input) != NULL)&&(i < number_datasets); i++){

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
            for(int i = 0 ; i < *number_datasets; i++)
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
            
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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

            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index ;

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
            printf("ERROR: filter_tmax: invalid filter methode");
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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index ;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            printf("ERROR: filter_niederschlag: invalid filter methode");
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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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
            for(int i = 0; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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

            for(int i = *number_datasets - datum_limit1 + 1; i < *number_datasets; i++)
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
            *number_datasets = new_index;

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

int show_data(cdatas_t *cdata, int number_datasets, int total_runs, int *input){

    
    char *token, letter, buff[50], total_runs_str[3];

    //Create File Stream according to number of program executions
    sprintf(total_runs_str, "%d", total_runs);
    char filename[50] = "climate_analysis_output";
    //filename <- anzahl durchlaeufe
    strcat(filename,total_runs_str);
    //filename <- .txt anhaengen
    strcat(filename,".txt");

    FILE *output_f = fopen(filename,"wb+");
    if(input == NULL){
        printf("\033[1;31m");printf("\nERROR: Fehler beim erstellen der Ausgabedatei der gefilterten Werte!\n");printf(RESET);
    }

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';


    printf("\n\n+------------------------+\n");
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
        if(strcmp(token,"datum")==0){
            input[0] = 1;
            fprintf(output_f,"datum;");
        }
        if(strcmp(token,"t_min")==0){
            input[1] = 1;
            fprintf(output_f,"t_min;");
        }
        if(strcmp(token,"t_max")==0){
            input[2] = 1;
            fprintf(output_f,"t_max;");
        }
        if(strcmp(token,"niederschlag")==0){
            input[3] = 1;
            fprintf(output_f,"niederschlag;");
        }

        token = strtok(NULL," ,\n\t");
    }
    
    //Ueberpruefen der eingabe
    int x;
    for(x = 0; x < 4; x++)if(input[x] == 1)break;
    //Wenn keines der Optionen gewaehlt wurde returniere -1 zur fehlerbehandlung
    if(x == 4){

        printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
        return -1;
    }

    for(int i = 0; i < number_datasets; i++)
    {
        if(input[0]){
            int yy = (int)cdata[i].datum/10000;
            int mm = ((int)cdata[i].datum - (yy * 10000)) / 100;
            int dd = ((int)cdata[i].datum - (yy * 10000) - (mm * 100));
            printf(COLOR1);printf("Datum:");printf(RESET);printf("%02d-%02d-%4d  ",dd, mm, yy);
            //Ausgabe ins outputfile
            fprintf(output_f,"%02d-%02d-%4d; ",dd, mm, yy);
        }
        if(input[1]){
            printf(COLOR1);printf("T_MIN:");printf(RESET);printf("%04.1lf  ",cdata[i].t_min);
            //Ausgabe ins outputfile
            fprintf(output_f,"%04.1lf; ",cdata[i].t_min);
        }
        if(input[2]){
            printf(COLOR1);printf("T_MAX:");printf(RESET);printf("%04.1lf  ",cdata[i].t_max);
            //Ausgabe ins outputfile
            fprintf(output_f,"%04.1lf; ",cdata[i].t_max);
        }
        if(input[3]){
            printf(COLOR1);printf("RR:");printf(RESET);printf("%04.1lf  ",cdata[i].niederschlag);
            //Ausgabe ins outputfile
            fprintf(output_f,"%04.1lf; ",cdata[i].niederschlag);
        }
        printf("\n");fprintf(output_f,"\n");
    }

    free(token);
    fclose(output_f);
    output_f = NULL;

    //Ausgabe der erfolgreichen Ausgabe
    printf("\033[1;32m");printf("\nDatei: %s zum Ausgaben der gefilterten Werte wurde erfolgreich erstellt!\n",filename);printf(RESET);
    return 0;
}

int show_averaged_data(cdatas_t *cdata, filter_args_t *args,int number_datasets, int total_runs, int *chosen_cols){

    
    char *token, letter, buff[50], total_runs_str[3];
    int input[4] = {0, 0, 0, 0};
    //Create File Stream according to number of program executions
    sprintf(total_runs_str, "%d", total_runs);
    char filename[50] = "climate_data_averged";
    //filename <- anzahl durchlaeufe
    strcat(filename,total_runs_str);
    //filename <- .txt anhaengen
    strcat(filename,".txt");

    FILE *output_f = fopen(filename,"wb+");
    if(input == NULL){
        printf("\033[1;31m");printf("\nERROR: Fehler beim erstellen der Ausgabedatei der gefilterten Werte!\n");printf(RESET);
    }
    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';


    printf("\n\n+---------------------------------------------------------------+\n");
    printf("| Ausgabeoptionen:                                              |\n");
    printf("|* monatsmittel                                                 |\n");
    printf("|* jahreszeit       -> Mittel ueber eine waehlbare Jahreszeit   |\n");
    printf("|* jahresmittel                                                 |\n");
    printf("|* letztenxjahre    -> Mittel ueber die jeweils letzten n Jahre |\n");
    printf("|                                                               |\n");
    printf("|Info: Option `letztenxjahre` muss in der datum filteroption    |\n");
    printf("|abfrage festgelegt werden!                                     |\n");
    printf("+---------------------------------------------------------------+\n\n");
    printf("Eingabe: ");

    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;
    
    token = strtok(buff," ,\n\t");
    
    while(token != NULL){

        if(strcmp(token,"monatsmittel")==0)input[0] = 1;
        if(strcmp(token,"jahreszeit")==0)input[1] = 1;
        if(strcmp(token,"jahresmittel")==0)input[2] = 1;
        if(strcmp(token,"letztenxjahre")==0)input[3] = 1;

        token = strtok(NULL," ,\n\t");
    }
    
    //Ueberpruefen der eingabe
    int x;
    for(x = 0; x < 4; x++)if(input[x] == 1)break;
    //Wenn keines der Optionen gewaehlt wurde returniere -1 zur fehlerbehandlung
    if(x == 4){

        printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
        return -1;
    }

    double avrg_tmin = 0, avrg_tmax = 0, avrg_niederschlag = 0;
    int mod;
    //Monatsmittel.....alle 30  Tage
    //Jahreszeit.......alle 90  Tage
    //Jahresmittel.....alle 365 Tage
    //letzten X Tage...nach X Tagen
    if(input[0])mod = 30;
    if(input[1])mod = 90;
    if(input[2])mod = 365;
    if(input[3])mod = args[0].arg1;

    for(int i = 0; i <= number_datasets; i++)
    {
        if(chosen_cols[0]){
            
            int yy = (int)cdata[i].datum/10000;
            int mm = ((int)cdata[i].datum - (yy * 10000)) / 100;
            int dd = ((int)cdata[i].datum - (yy * 10000) - (mm * 100));
            if(((i + 1)%mod) == 0){
                printf(COLOR1);printf("Datum: ");printf(RESET);
                printf("%02d-%02d-%4d  ", dd, mm, yy);
                //Ausgabe ins outputfile
                fprintf(output_f,"%02d-%02d-%4d;  ", dd, mm, yy);
            }
        }
        if(chosen_cols[1]){
            
            avrg_tmin += cdata[i].t_min;
            if(((i + 1)%mod) == 0){

                printf(COLOR1);printf("T_MIN: ");printf(RESET);
                printf("%04.1lf  ", avrg_tmin/(double)mod);
                //Ausgabe ins outputfile
                fprintf(output_f,"%04.1lf;  ", avrg_tmin/(double)mod);
                avrg_tmin = 0;
            }
        }
        if(chosen_cols[2]){

            avrg_tmax += cdata[i].t_max;
            if(((i + 1)%mod) == 0){

                printf(COLOR1);printf("T_MAX: ");printf(RESET);
                printf("%04.1lf  ", avrg_tmax/(double)mod);
                //Ausgabe ins outputfile
                fprintf(output_f,"%04.1lf;  ", avrg_tmax/(double)mod);
                //Reset 
                avrg_tmax = 0;
            }
        }
        if(chosen_cols[3]){

            avrg_niederschlag += cdata[i].niederschlag;
            if(((i + 1)%(mod)) == 0){

                printf(COLOR1);printf("RR: ");printf(RESET);
                printf("%04.1lf  ", avrg_niederschlag/(double)mod);
                //Ausgabe ins outputfile
                fprintf(output_f,"%04.1lf;  ", avrg_niederschlag/(double)mod);
                //Reset 
                avrg_niederschlag = 0;
            }
        }
        if(((i + 1)%mod) == 0){
            printf("\n");
            fprintf(output_f,"\n");
        } 
    }

    free(token);
    fclose(output_f);
    output_f = NULL;
    //Ausgabe der erfolgreichen Ausgabe
    printf("\033[1;32m");printf("\nDatei: %s zum Ausgaben der gefilterten Werte wurde erfolgreich erstellt!\n",filename);printf(RESET);
    return 0;
}

int request_datum_args(filter_args_t *args){

    
    char letter, buff[50], temp[50],key[5];
    //Variablen fuer die eingabe eines zeitraums
    int dd1,mm1,yy1;
    int dd2,mm2,yy2;

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';

    printf("\n\n+-------------------------------------------------------------------+\n");
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

    //buff auf temp uebergeben um string korruption von sscanf zu vermeiden
    strcpy(temp, buff);

    //Einlesen vom schluesselwort zur fallunterscheidung
    sscanf(buff,"%s",key);

    //Array zum auffangen vom key
    char dump[5];

    //Wenn key == datum dann 1. Filteroption ausführen
    if(strcmp(key, "datum")==0){
        
        //Entspricht filtermethode 2 filter_datum(2,limit1,limit2)
        sscanf(temp, "%s %d.%d.%d - %d.%d.%d", dump,&dd1, &mm1, &yy1,&dd2,&mm2,&yy2);
        //Filtermethode 2 weahlen
        args[0].method = 2;
        //mm,dd,yy in das format YYYYMMDD umwandeln
        args[0].arg1 = (yy1 * 10000 + mm1 * 100 + dd1);
        args[0].arg2 = (yy2 * 10000 + mm2 * 100 + dd2);
        return 0;
    }


    if(strcmp(key, "bis")==0){
        
        //Entspricht filtermethode 0 filter_datum(0,limit1, 0)
        sscanf(temp, "%s %d.%d.%d", dump,&dd1, &mm1, &yy1);
        //Filtermethode 0 weahlen
        args[0].method = 0;
        //mm,dd,yy in das format YYYYMMDD umwandeln
        args[0].arg1 = (yy1 * 10000 + mm1 * 100 + dd1);
        return 0;
    }

    if(strcmp(key, "ab")   ==0){
        //Entspricht filtermethode 1 filter_datum(1,limit1,limit2)
        sscanf(temp, "%s %d.%d.%d", dump,&dd1, &mm1, &yy1);
        //Filtermethode 1 weahlen
        args[0].method = 1;
        //mm,dd,yy in das format YYYYMMDD umwandeln
        args[0].arg1 = (yy1 * 10000 + mm1 * 100 + dd1);
        return 0;
    }

    if(strcmp(key, "1.")==0){

        int last_x_days = 0;
        //Entspricht filtermethode 3 filter_datum(3,limit1,0)
        sscanf(temp, "%s letzten %d Tage", dump, &last_x_days);
        //Filtermethode 3 weahlen
        args[0].method = 3;
        args[0].arg1 = last_x_days;
        //zweites argument wichtig! Ansonsten erwartet das Programm ein limit2
        args[0].arg2 = 0;
        return 0;
    }

    if(strcmp(key, "2.")==0){

        int last_x_days = 0;
        //Entspricht filtermethode 3 filter_datum(3,limit1,limit2)
        sscanf(temp, "%s letzten %d Tage bis %d.%d.%d", dump, &last_x_days, &dd1, &mm1, &yy1);

        //Filtermethode 3 weahlen
        args[0].method = 3;
        args[0].arg1 = last_x_days;
        //zweites argument wichtig! Ansonsten erwartet das Programm ein limit2
        args[0].arg2 = (yy1 * 10000 + mm1 * 100 + dd1);
        return 0;
    }

    //Falls keine Eingabe mit eines der Schlüsselwoerter uebereinstimmt returniere ERROR
    printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
    return -1;
}

int request_t_min_args(filter_args_t *args){

    
    char letter, buff[50], temp[50],key[5];
    //Variablen fuer die eingabe eines zeitraums
    double limit1, limit2;

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';

    printf("\n\n+-------------------------------------------------------------------+\n");
    printf("| Filterung nach tmin:                                              |\n");
    printf("|                                                                   |\n");
    printf("| Bitte waehlen Sie eines der moeglichen Filtermethoden aus:        |\n");
    printf("|* t_min XX - YY           ->Filterung eines tmin Bereichs          |\n");
    printf("|* bis YY                    ->alle Daten bis ...                   |\n"); 
    printf("|* ab XX                     ->alle Daten ab  ...                   |\n");
    printf("|                                                                   |\n");
    printf("| z.B. t_min -1.2 - 5.5                                             |\n");
    printf("| z.B. bis 3.1                                                      |\n");
    printf("| z.B. ab 2.2                                                       |\n");
    printf("+-------------------------------------------------------------------+\n\n");
    printf("Eingabe: ");

    //Zeichenweises einlesen
    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;

    //buff auf temp uebergeben um string korruption von sscanf zu vermeiden
    strcpy(temp, buff);

    //Einlesen vom schluesselwort zur fallunterscheidung
    sscanf(buff,"%s",key);

    //Array zum auffangen vom key
    char dump[5];

    //Wenn key == datum dann 1. Filteroption ausführen
    if(strcmp(key, "t_min")==0){
        
        //Entspricht filtermethode 2 filter_t_min(2,limit1,limit2)
        sscanf(temp, "%s %lf - %lf", dump, &limit1, &limit2);
        //Filtermethode 2 weahlen
        args[1].method = 2;
        args[1].arg1 = limit1;
        args[1].arg2 = limit2;
        return 0;
    }


    if(strcmp(key, "bis")==0){
        
        //Entspricht filtermethode 0 filter_tmin(0,limit1, 0)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 0 weahlen
        args[1].method = 0;
        args[1].arg1 = limit1;
        return 0;
    }

    if(strcmp(key, "ab")   ==0){
        //Entspricht filtermethode 1 filter_tmin(1,limit1,limit2)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 1 weahlen
        args[1].method = 1;
        args[1].arg1 = limit1;
        return 0;
    }

    //Falls keine Eingabe mit eines der Schlüsselwoerter uebereinstimmt returniere ERROR
    printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
    return -1;
}

int request_t_max_args(filter_args_t *args){

    
    char letter, buff[50], temp[50],key[5];
    //Variablen fuer die eingabe eines zeitraums
    double limit1, limit2;

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';

    printf("\n\n+-------------------------------------------------------------------+\n");
    printf("| Filterung nach tmax:                                              |\n");
    printf("|                                                                   |\n");
    printf("| Bitte waehlen Sie eines der moeglichen Filtermethoden aus:        |\n");
    printf("|* t_max XX - YY           ->Filterung eines tmax Bereichs          |\n");
    printf("|* bis YY                    ->alle Daten bis ...                   |\n"); 
    printf("|* ab XX                     ->alle Daten ab  ...                   |\n");
    printf("|                                                                   |\n");
    printf("| z.B. t_max -1.2 - 5.5                                             |\n");
    printf("| z.B. bis 3.1                                                      |\n");
    printf("| z.B. ab 2.2                                                       |\n");
    printf("+-------------------------------------------------------------------+\n\n");
    printf("Eingabe: ");

    //Zeichenweises einlesen
    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;

    //buff auf temp uebergeben um string korruption von sscanf zu vermeiden
    strcpy(temp, buff);

    //Einlesen vom schluesselwort zur fallunterscheidung
    sscanf(buff,"%s",key);

    //Array zum auffangen vom key
    char dump[5];

    //Wenn key == datum dann 1. Filteroption ausführen
    if(strcmp(key, "t_max")==0){
        
        //Entspricht filtermethode 2 filter_t_max(2,limit1,limit2)
        sscanf(temp, "%s %lf - %lf", dump, &limit1, &limit2);
        //Filtermethode 2 weahlen
        args[2].method = 2;
        args[2].arg1 = limit1;
        args[2].arg2 = limit2;
        return 0;
    }


    if(strcmp(key, "bis")==0){
        
        //Entspricht filtermethode 0 filter_tmax(0,limit1, 0)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 0 weahlen
        args[2].method = 0;
        args[2].arg1 = limit1;
        return 0;
    }

    if(strcmp(key, "ab")   ==0){
        //Entspricht filtermethode 1 filter_tmax(1,limit1,limit2)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 1 weahlen
        args[2].method = 1;
        args[2].arg1 = limit1;
        return 0;
    }

    //Falls keine Eingabe mit eines der Schlüsselwoerter uebereinstimmt returniere ERROR
    printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
    return -1;
}

int request_niederschlag_args(filter_args_t *args){

    
    char letter, buff[50], temp[50],key[5];
    //Variablen fuer die eingabe eines zeitraums
    double limit1, limit2;

    //Initialisiere input buffer -> Benutzer eingabe
    for(int i = 0; i < 50; i++)buff[i] = '\0';

    printf("\n\n+-------------------------------------------------------------------+\n");
    printf("| Filterung nach Niederschlag:                                      |\n");
    printf("|                                                                   |\n");
    printf("| Bitte waehlen Sie eines der moeglichen Filtermethoden aus:        |\n");
    printf("|* niederschlag XX - YY    ->Filterung eines niederschlag Bereichs  |\n");
    printf("|* bis YY                  ->alle Daten bis ...                     |\n"); 
    printf("|* ab XX                   ->alle Daten ab  ...                     |\n");
    printf("|                                                                   |\n");
    printf("| z.B. niederschlag 5 - 7.3                                         |\n");
    printf("| z.B. bis 3.1                                                      |\n");
    printf("| z.B. ab 2.2                                                       |\n");
    printf("+-------------------------------------------------------------------+\n\n");
    printf("Eingabe: ");

    //Zeichenweises einlesen
    for(int i = 0; (letter = getchar()) != '\n';i++)buff[i] = letter;

    //buff auf temp uebergeben um string korruption von sscanf zu vermeiden
    strcpy(temp, buff);

    //Einlesen vom schluesselwort zur fallunterscheidung
    sscanf(buff,"%s",key);

    //Array zum auffangen vom key
    char dump[5];

    //Wenn key == datum dann 1. Filteroption ausführen
    if(strcmp(key, "niederschlag")==0){
        
        //Entspricht filtermethode 2 filter_niederschlag(2,limit1,limit2)
        sscanf(temp, "%s %lf - %lf", dump, &limit1, &limit2);
        //Filtermethode 2 weahlen
        args[3].method = 2;
        args[3].arg1 = limit1;
        args[3].arg2 = limit2;
        return 0;
    }


    if(strcmp(key, "bis")==0){
        
        //Entspricht filtermethode 0 filter_niederschlag(0,limit1, 0)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 0 weahlen
        args[3].method = 0;
        args[3].arg1 = limit1;
        return 0;
    }

    if(strcmp(key, "ab")   ==0){
        //Entspricht filtermethode 1 filter_niederschlag(1,limit1,limit2)
        sscanf(temp, "%s %lf", dump, &limit1);
        //Filtermethode 1 weahlen
        args[3].method = 1;
        args[3].arg1 = limit1;
        return 0;
    }

    //Falls keine Eingabe mit eines der Schlüsselwoerter uebereinstimmt returniere ERROR
    printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
    return -1;
}

int get_input(filter_args_t *args, int *filter_option){

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
    
    //Eingabe der Filtergrenzwerte ueberpruefen
    int validity;

    do{

        if(strcmp(token,"datum")==0){

            validity = -1;
            //Auffüllen der Parameter Daten mit Datum Filter Werten
            filter_option[0] = 1;
            //Solange eingabe ungueltig wiederholen
            while(validity != 0)validity = request_datum_args(args);
        }
        if(strcmp(token,"t_min")==0){

            validity = -1;
            filter_option[1] = 1;
            //Solange eingabe ungueltig wiederholen
            while(validity != 0)validity = request_t_min_args(args);
        }
        
        if(strcmp(token,"t_max")==0){

            validity = -1;
            filter_option[2] = 1;
            //Solange eingabe ungueltig wiederholen
            while(validity != 0)validity = request_t_max_args(args);

        }

        if(strcmp(token,"niederschlag")==0){

            validity = -1;
            filter_option[3] = 1;
            while(validity != 0)validity = request_niederschlag_args(args);
        }

        //Eingabe auf richtigkeit ueberpruefen
        int i;
        for(i = 0; i < 4; i++){
            if(filter_option[i] == 1)break;
        }

        if(i == 4){

            printf("\033[1;31m");printf("\n\nERROR: Bitte geben sie eines der vorgegebenen Eingabeoptionen ein!\n\n");printf("\033[0m");
            return -1;
        }
    }while(((token = strtok(NULL, " ,\n\t"))!=NULL));

    free(token);

    return 0;
}

void linear_fit(){

    char filename[50] = "linear_fit.txt";

    FILE *output_linear_fit = fopen(filename,"wb+");

    if(output_linear_fit == NULL){
        printf("\033[1;31m");printf("\nERROR: Fehler beim erstellen der Ausgabedatei der gefilterten Werte!\n");printf(RESET);
    }

    int i, s;
    double x_mittel=4.5, sumX2=82.5;
    double y_t_min=7.4, y_t_max=15.6, y_rr=1.8;
    double b_t_min=1.15/sumX2, b_t_max=-2.3/sumX2, b_rr=7.25/sumX2;
    double a_t_min = y_t_min - b_t_min * x_mittel, a_t_max = y_t_max - b_t_max * x_mittel, a_rr = y_rr - b_rr * x_mittel; 

    printf("\n\nGeben sie die Anzahl der Jahre an, über die gefittet werden soll:\n");
    scanf("%d", &s);
    getchar();

    for(i=0; i<=s; i++){
        double linear_fit_t_min = a_t_min + b_t_min*i;
        double linear_fit_t_max = a_t_max + b_t_max*i;
        double linear_fit_rr = a_rr + b_rr*i;
        printf("%4d ", 2009+i);
        fprintf(output_linear_fit,"%4d ", 2009+i);

        printf("%04f ", linear_fit_t_min);
        fprintf(output_linear_fit,"%04f ", linear_fit_t_min);

        printf("%04f ", linear_fit_t_max);
        fprintf(output_linear_fit,"%04f ", linear_fit_t_max);

        printf("%04f\n", linear_fit_rr);
        fprintf(output_linear_fit,"%04f\n", linear_fit_rr);    
    }
     
}

int main(void){
    

    char run_condition = 'y';
    int total_runs = 0;
    char filename[] = "input.csv";

    while(run_condition != 'n'){

        //Maximale Azahl an datensaetze welches spaeter in get_input berechnet wird
        int number_datasets = 0, padding = 15;

        //Filteroptionen dient der "aktivierung" der filter von Datum | T_MIN | T_MAX | Niederschlag
        int filter_option[4] = {0,0,0,0};

        //Inputstream von der CSV datei erzeugen
        FILE *input = fopen(filename,"r");

        if(input == NULL){
            printf("\033[1;31m");printf("\nERROR: Fehler beim Einlesen der Datei %s!\n",filename);printf(RESET);
        }else{

            printf("\033[1;32m");printf("\nDatei %s wurde erfolgreich gelesen!\n", filename);printf(RESET);
        }
        
        //===========================================================
        //VORBEREITUNG
        //===========================================================
        //1. Anzahl Datensaetze ermitteln
        number_datasets = process_input(input, number_datasets,padding);

        //freeing input stream
        fclose(input);
        input = NULL;
        
        //2. Allocate Pointer of size number_datasets
        cdatas_t *cdata = (cdatas_t *)malloc(number_datasets * sizeof(cdatas_t));

        //Initialize input param variables
        filter_args_t *args = (filter_args_t *)malloc(4 * sizeof(filter_args_t));
                
        //3. Uebertragen der Datensaetze vom input stream in das struct array
        input = fopen("input.csv","r");
        parse_input_to_struct(input, cdata, number_datasets,padding);

        //Free Inputstream
        fclose(input);input = NULL;

        //Wenn User eingabe Korrekt dann get_input() == 0
        //Bei ungueltiger Eingabe Return wert -1 -> wiederhole Eingabe aufforderung
        while(get_input(args, filter_option) != 0);

        //==========================================================
        //Datenfilterung
        //==========================================================

        if(filter_option[0])filter_datum(cdata, &number_datasets, args[0].method, args[0].arg1, args[0].arg2);
        if(filter_option[1])filter_tmin(cdata, &number_datasets, args[1].method, args[1].arg1, args[1].arg2);
        if(filter_option[2])filter_tmax(cdata, &number_datasets, args[2].method, args[2].arg1, args[2].arg2);
        if(filter_option[3])filter_niederschlag(cdata, &number_datasets, args[3].method, args[3].arg1, args[3].arg2);
        
       
        //==========================================================
        //Ausgabe
        //==========================================================
        //Wiederhole die Anfrage solange bis return wert 0 bei gueltiger eingabe
        int chosen_cols[] = {0,0,0,0};

        total_runs++;
        while(show_data(cdata, number_datasets,total_runs, chosen_cols)!=0);

        printf("\n\nMoechten Sie eine Darstellung der Mittelwert? (y/n) ");

        char avrg_condition = getchar();getchar();

        while((avrg_condition != 'n')&&(show_averaged_data(cdata, args, number_datasets, total_runs, chosen_cols) != 0));

        //=========================================================
        //LINEAR FIT
        //=========================================================
        linear_fit();

        printf("\033[1;31m");printf("\n\nMoechten Sie die Anwendung wiederholen? (y/n) ");
        printf("\033[0m");

        free(cdata);
        cdata = NULL;

        free(args);
        args = NULL;

        run_condition = getchar();getchar();
    }

    return 0;
}
