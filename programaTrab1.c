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
    FILE* output_fp = NULL;
    dataReg *reg = NULL;
    int written_bytes = 0;

    scanf("%c", &func_option);
    scanf("%s", input_filename);
    

    input_fp = fopen(input_filename, "r");
    output_fp = fopen("teste.bin", "wb");

    if (input_fp != NULL) {
        printf("Abrindo arquivo %s para leitura...\n", input_filename);
        do {
            reg = read_next_data_reg(input_fp);
            if (reg) {
                write_reg_to_file(reg, output_fp, &written_bytes);
            }
        } while (reg != NULL);
    }

    
    else {
        printf("Não foi possível abrir o arquivo %s para leitura\n", input_filename);
    }

    int num_pag_de_disco = written_bytes / PAGE_SIZE;
    if ((written_bytes % PAGE_SIZE) > 0)
        num_pag_de_disco += 1;
 
    printf("Encerrando o programa...\nForam escritas %d pag de disco.", num_pag_de_disco);

    if (reg)
        free(reg);

    fclose(input_fp);
    fclose(output_fp);

    return 0;
}


/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE* csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_next_data_reg(FILE* csv_file_pointer) {

    // Variáveis para leitura da póxima linha do arquivo usando a função getline da stdlib.
    char * line;                    // Recebe o conteúdo da linha
    size_t len = 0;                 // Indica tamanho da string lida
    ssize_t read;                   // Sinaliza se houve leitura

    // Variáveis para processar a linha recebida, separando-as em campos de acordo com o padrão de arquivo csv 
    char buffer[BUFFER_LEN];        // Buffer para armazenar cada campo
    int field_len;                  // Indica o tamanho do campo lido
    int field_index;                // Indica o índice do último campo lido
    int index;                      // Índice para percorrer os caracteres da linha

    // Aloca estrutura para receber o conteúdo lido do csv
    dataReg *reg = (dataReg*) malloc(sizeof(dataReg));
    
    // Valores iniciais padrão para esta parte do projeto
    reg->removido = '-';
    reg->encadeamento = -1;
    
    // Tenta capturar a próxima linha do arquivo
    read = getline(&line, &len, csv_file_pointer);
    
    // Caso tenha recebido uma linha
    if (read != -1) {

        // Inicializa a contagem de índices
        field_len = 0;
        field_index = 0;
        
        // Percorre a linha
        for (index = 0; index < len; index++) {

            // Preenhe o buffer até encontrar um caractere de separação de campos
            if (line[index] != ',') {
                if (field_len < BUFFER_LEN) {
                    buffer[field_len] = line[index];
                    field_len++;
                }
            }
        
            // Quando encontrou um caractere de separação 
            else {

                // Usa a informação do índice do campo para decidir qual variável preencher na estrutura
                switch (field_index)
                {
                    // Campo nroInscricao
                    case 0:
                        {    
                            // É garantido que o campo existe. Converte para inteiro
                            reg->nroInscricao = atoi(buffer);
                        }
                    break;

                    // Campo nota
                    case 1:
                        {
                            if (field_len == 0) {
                                // Caso campo vazio será representado como -1.0
                                reg->nota = -1.0;
                            }
                            else {
                                // Se leu corretamente, converte para float
                                reg->nota = atof(buffer);
                            } 
                        }
                        break;
                    
                    // Campo data
                    case 2:
                        {
                            if (field_len == 0) {
                                
                                // Caso campo vazio
                                reg->data[0] = '\0';
                                
                                // Atenção!! int j na inicialização do laço é uma expressão do padrão C/C++ 2011
                                // Verificar se o [runcode] aceita
                                for (int j = 1; j < 10; ++j) {

                                    // Preenche o resto do campo com caracteres vazios
                                    reg->data[j] = EMPTY_CHAR;
                                }
                            }

                            else {
                                // Assume que se o campo existe, ele possui o tamanho padrão
                                strcpy(reg->data, buffer);
                            }
                        }
                        break;

                    // Campo cidade
                    case 3:
                        {
                            // Armazena indicador de tamanho
                            reg->indTamanhoCidade = field_len;

                            // Campo vazio aponta para null
                            if (field_len == 0) {
                                reg->cidade = NULL;
                            }

                            else {
                                // Se o campo não é vazio, aloca o espaço neecssário para armazená-lo
                                reg->cidade = (char*) malloc(sizeof(char) * field_len);
                                strncpy(reg->cidade, buffer, field_len);
                            }
                       }   
                        break;

                    // Campo nome da escola
                    case 4:
                        {
                            // Armazena indicador de tamanho
                            reg->indTamanhoEscola = field_len;

                            // Campo vazio aponta para null
                            if (field_len == 0) {
                                reg->nomeEscola = NULL;
                            }
                            
                            else {
                                // Se o campo não é vazio, aloca o espaço para armazená-lo
                                reg->nomeEscola = (char*) malloc(sizeof(char) * field_len);
                                strncpy(reg->nomeEscola, buffer, field_len);
                            }
                        }   
                        break;                    

                    default:
                        break;
                }

                // // Coloca o \0 apenas para fins de exibição na tela para debug, uma vez que, como existe indicado de tamanho,
                // // não e necessário sinalizar final da string
                // buffer[field_len] = '\0';
                // printf("field %d with lenght %d: %s\n",field_index, field_len, buffer);

                // Reseta contador de tamanho do campo, incrementa índice de campo lido
                field_len = 0;
                field_index++;
            }
        }     

        // printf("===================\n");
        
        // Libera memória
        if(line)
            free(line);
        
        // Retorna o registro lido
        return reg;
    }

    // Caso não tenha recebido nenhuma linha
    else {

        // Libera memória
        free(reg);

        // Retorna NULL
        return NULL;
    }          
}


/*
 *
 * 
 * 
 */
void write_header_to_file(fileHeader header, FILE* bin_file_pointer, int *file_size_in_bytes) {

}

/*
 *
 * 
 * 
 */ 
void write_reg_to_file(dataReg *to_write, FILE* bin_file_pointer, int *file_size_in_bytes) {

    size_t size_written; 

    size_written = 0;
    size_written += sizeof(char) * fwrite(&to_write->removido, sizeof(char), 1, bin_file_pointer);
    size_written += sizeof(int) * fwrite(&to_write->encadeamento, sizeof(int), 1, bin_file_pointer);
    size_written += sizeof(int) * fwrite(&to_write->nroInscricao, sizeof(int), 1, bin_file_pointer);
    size_written += sizeof(double) * fwrite(&to_write->nota, sizeof(double), 1, bin_file_pointer);
    size_written += sizeof(char) * fwrite(&to_write->data, sizeof(char), 10, bin_file_pointer);

    if (to_write->indTamanhoCidade > 0) {
        size_written += sizeof(int) * fwrite(&to_write->indTamanhoCidade, sizeof(int), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite("4", sizeof(char), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite(&to_write->cidade, sizeof(char), to_write->indTamanhoCidade, bin_file_pointer);
    }
    
    if (to_write->indTamanhoEscola > 0) {
        size_written += sizeof(int) * fwrite(&to_write->indTamanhoEscola, sizeof(int), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite("4", sizeof(char), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite(&to_write->nomeEscola, sizeof(char), to_write->indTamanhoEscola, bin_file_pointer);
    }

    char c = EMPTY_CHAR;
    for (int i = (int) size_written; i < REG_SIZE; ++i) {
        fputc(c, bin_file_pointer);
        size_written += 1;
    }

    printf("%zu bytes escritos\n", size_written);
    *file_size_in_bytes += (int) size_written; 
}