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

    scanf("%c", &func_option);
    scanf("%s", input_filename);
    

    input_fp = fopen(input_filename, "r");
    if (input_fp != NULL) {
        printf("Abrindo arquivo %s para leitura...\n", input_filename);
    }
    else {
        printf("Não foi possível abrir o arquivo %s para leitura\n", input_filename);
    }

    return 0;
}

