// Bibliotecas da linguagem
#include <stdio.h>

// Bibliotecas criadas para solucionar o trabalho
#include "programaTrab1.h"

int main(int argc, char** argv) {
    
    int arg_it = 0;
    char* char_pt;

    for (arg_it = 0; arg_it < argc; arg_it++) {
        
        printf("arg %d", arg_it);
        
        for (char_pt = argv[arg_it][0]; &char_pt != '\0'; char_pt++) {
            printf("%c", &char_pt);
        }
        
        printf("\n");
    }    
    
    return 0;
}

