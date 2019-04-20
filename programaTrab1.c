// Bibliotecas da linguagem
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Bibliotecas criadas para solucionar o trabalho
#include "programaTrab1.h"

int main(int argc, char** argv) {

    char func_option;
    char* input_filename;
    FILE* input_fp = NULL;
    int stop_condition = 0;

    scanf("%c", &func_option);
    scanf("%s", input_filename);
    

    input_fp = fopen(input_filename, "r");
    if (input_fp != NULL) {
        printf("Abrindo arquivo %s para leitura...\n", input_filename);
        while (!stop_condition) {
            stop_condition = read_next_data_reg(input_fp);
        }
    }

    
    else {
        printf("Não foi possível abrir o arquivo %s para leitura\n", input_filename);
    }

    fclose(input_fp);
    return 0;
}

int read_next_data_reg(FILE* csv_file_pointer) {

    char * line;
    char * token;
    size_t len = 0;
    ssize_t read;
    int it;

    int stop_condition = 0;

    dataReg *reg = (dataReg*) malloc(sizeof(dataReg));
    
    read = getline(&line, &len, csv_file_pointer);
    if (read != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);
        char *last_collon = NULL;
        char *next_collon = NULL;
        char *field = NULL;
        int it = 0;

        printf("%s\n", line);
        next_collon = strpbrk(line, ",");
        field = (char *) malloc(sizeof(char) * (next_collon - line + 1));
        strncpy(field, line, next_collon - line);
        printf("field %d = %s \n", it, field);
        
        last_collon = next_collon;
        while (last_collon) {


            next_collon = strpbrk(last_collon + 1, ",");

            if (next_collon) {
                if (field)
                    free(field);
    
                field = (char *) malloc(sizeof(char) * (next_collon - last_collon + 1));
                
                strncpy(field, next_collon + 1, next_collon - last_collon - 1);
                
                last_collon = next_collon;
                it++;
            }
            else {
                last_collon = 0;
            }
        }
        printf("===============================\n");
    
    
        if (field)
            free(field);
    }
    else {
        stop_condition = 1;
    }
    
    if (line)
        free(line);

    return stop_condition;
}

