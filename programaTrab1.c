// Bibliotecas da linguagem
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Bibliotecas criadas para solucionar o trabalho
#include "programaTrab1.h"

int main(int argc, char** argv) {

    char func_option;

    char *field_name;
    char *key_to_search;

    char *filename;
    char *output_filename = "arquivoTrab1si.bin"; 

    FILE *input_fp = NULL;
    FILE *output_fp = NULL;

    dataReg *reg = NULL;
    fileHeader *header = create_initialized_header();


    int written_bytes = 0;

    scanf("%c", &func_option);
    scanf("%s", filename);
    

    switch (func_option)
    {
        case '1':
        {

            input_fp = fopen(filename, "r");
            output_fp = fopen(output_filename, "wb+");

            if (input_fp == NULL) {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }    

            if (output_fp == NULL) {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }
        
            if (input_fp != NULL && output_fp != NULL) {

                write_header_to_bin(header, output_fp, &written_bytes);
                
                // Pula primeira linha do csv
                fseek(input_fp, 41, SEEK_SET);
                
                // Le cada registro do csv e salva no binário
                reg = read_reg_from_csv(input_fp);
                while(reg != NULL) {
                    write_reg_to_bin(reg, output_fp, &written_bytes);
                    safely_free_reg(reg);
                    reg = read_reg_from_csv(input_fp);
                }
                printf("%s", output_filename);
            }

            
            else {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }
            if (reg)
                free(reg);

            if (header)
                free(header);

            fclose(input_fp);
            fclose(output_fp);
        }
        break;

        case '2':
        {
            int read_bytes = 0;
            input_fp = fopen(filename, "rb");
            if (input_fp != NULL) {
               
                // Pula primeira linha do csv
                fseek(input_fp, PAGE_SIZE, SEEK_SET);
                read_bytes += PAGE_SIZE;

                reg = read_reg_from_bin(input_fp, &read_bytes);
                // Le cada registro do csv e salva no binário
                while(reg) {
                    print_reg_to_std(reg);
                    reg = read_reg_from_bin(input_fp, &read_bytes);
                }
                int pag_de_disco = read_bytes / PAGE_SIZE;
                if (read_bytes % PAGE_SIZE) {
                    pag_de_disco++;
                }

                printf("Número de páginas de disco acessadas: %d", pag_de_disco);
            }

            else {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }
        }
            break;
        case '3':
        {
            int i;
            fflush(stdin);
            // scanf("%s", field_name);
            // scanf("%s", key_to_search);
            scanf("%d", &i);
            printf("Seaching for value  in field %d", i);
        }
        break;

    
        default:
            break;
    }
    

    

    return 0;
}

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_reg_from_bin(FILE *bin_file_pointer, int *size_read) {

    size_t read_bytes = 0;
    // Variáveis para leitura da póxima linha do arquivo usando a função getline da stdlib.
    dataReg* reg = (dataReg*) malloc(sizeof(dataReg));
    int indicador_temporario = 0;
    char tag = '0';
    char teste;

    if (fread(&reg->removido, sizeof(char), 1, bin_file_pointer)){ 

        read_bytes += 1; 
        read_bytes += sizeof(int) * fread(&reg->encadeamento, sizeof(int), 1, bin_file_pointer);
        read_bytes += sizeof(int) * fread(&reg->nroInscricao, sizeof(int), 1, bin_file_pointer);
        read_bytes += sizeof(double) * fread(&reg->nota, sizeof(double), 1, bin_file_pointer);
        read_bytes += sizeof(char) * fread(reg->data, sizeof(char), 10, bin_file_pointer);

        reg->indTamanhoCidade = 0;
        reg->cidade = NULL;
        reg->indTamanhoEscola = 0;
        reg->nomeEscola = NULL;

        //Verifica se o proximo campo de tamanho variável existe:
        if ((teste = fgetc(bin_file_pointer)) == EMPTY_CHAR) {
            ungetc(teste, bin_file_pointer);
        }
        else {
            ungetc(teste, bin_file_pointer);
            read_bytes += sizeof(int) * fread(&indicador_temporario, sizeof(int), 1, bin_file_pointer);
            read_bytes += sizeof(char) * fread(&tag, sizeof(char), 1, bin_file_pointer);
            if (tag == '4') {
                reg->indTamanhoCidade = indicador_temporario - 2;
                reg->cidade = (char*) malloc(sizeof(reg->indTamanhoCidade));
                read_bytes += sizeof(char) * fread(reg->cidade, sizeof(char), reg->indTamanhoCidade + 1, bin_file_pointer);

            }
            else if (tag == '5') {
                reg->indTamanhoEscola = indicador_temporario - 2;
                reg->nomeEscola = (char*) malloc(sizeof(reg->indTamanhoEscola));
                read_bytes += sizeof(char) * fread(reg->nomeEscola, sizeof(char), reg->indTamanhoEscola + 1, bin_file_pointer);

            }
            else {
                printf("Deu ruim\n");
            }
        }
        //Verifica se o proximo campo de tamanho variável existe:
        if ((teste = fgetc(bin_file_pointer)) == EMPTY_CHAR) {
            ungetc(teste, bin_file_pointer);
        }
        else {
            ungetc(teste, bin_file_pointer);
            read_bytes += sizeof(int) * fread(&indicador_temporario, sizeof(int), 1, bin_file_pointer);
            read_bytes += sizeof(char) * fread(&tag, sizeof(char), 1, bin_file_pointer);
            if (tag == '4') {
                reg->indTamanhoCidade = indicador_temporario - 2;
                reg->cidade = (char*) malloc(sizeof(reg->indTamanhoCidade));
                read_bytes += sizeof(char) * fread(reg->cidade, sizeof(char), reg->indTamanhoCidade + 1, bin_file_pointer);

            }
            else if (tag == '5') {
                reg->indTamanhoEscola = indicador_temporario - 2;
                reg->nomeEscola = (char*) malloc(sizeof(reg->indTamanhoEscola));
                read_bytes += sizeof(char) * fread(reg->nomeEscola, sizeof(char), reg->indTamanhoEscola + 1, bin_file_pointer);

            }
            else {
                printf("Deu ruim\n");
            }
        }    
    
        int deslocamento = 80 - read_bytes;
        read_bytes += deslocamento;

        fseek(bin_file_pointer, deslocamento, SEEK_CUR);
        return reg;
    }
    
    return NULL;
}



/*
 *
 * 
 * 
 */
fileHeader * create_initialized_header() {

    int fixed_len = 55;
    size_t string_len;

    fileHeader *new_header;
    new_header = (fileHeader*) malloc(sizeof(fileHeader));

    new_header->status = '0';
    new_header->topoPilha = -1;

    // Campo 1 

    // Tag 
    new_header->tagCampo1 = '1';

    // Escreve descrição
    strcpy(new_header->desCampo1, "numero de inscricao do participante do ENEM\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo1) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo1 + string_len), EMPTY_CHAR, (fixed_len - string_len));
    // ====================================================================================

    // Campo 2 
    
    // Tag 
    new_header->tagCampo2 = '2';

    // Escreve descrição
    strcpy(new_header->desCampo2, "nota do participante do ENEM na prova de matematica\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo2) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo2 + string_len), EMPTY_CHAR, (fixed_len - string_len));
    // ====================================================================================

    // Campo 3 

    // Tag 
    new_header->tagCampo3 = '3';

    // Escreve descrição
    strcpy(new_header->desCampo3, "data\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo3) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo3 + string_len), EMPTY_CHAR, (fixed_len - string_len));
    // ====================================================================================

    // Campo 4 

    // Tag 
    new_header->tagCampo4 = '4';

    // Escreve descrição
    strcpy(new_header->desCampo4, "cidade na qual o participante do ENEM mora\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo4) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo4 + string_len), EMPTY_CHAR, (fixed_len - string_len));
    // ====================================================================================

    // Campo 5 

    // Tag 
    new_header->tagCampo4 = '5';

    // Escreve descrição
    strcpy(new_header->desCampo5, "nome da escola de ensino medio\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo5) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo5 + string_len), EMPTY_CHAR, (fixed_len - string_len));
    // ====================================================================================

    return new_header;
}


/*
 *
 * 
 * 
 */
void write_header_to_bin(fileHeader *header, FILE *bin_file_pointer, int *file_size_in_bytes) {

    size_t size_written = 0;

    size_written += sizeof(char) * fwrite(&header->status,    sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(int)  * fwrite(&header->topoPilha, sizeof(int),  1,  bin_file_pointer);

    size_written += sizeof(char) * fwrite(&header->tagCampo1, sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(char) * fwrite(&header->desCampo1, sizeof(char), 55, bin_file_pointer);
    
    size_written += sizeof(char) * fwrite(&header->tagCampo2, sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(char) * fwrite(&header->desCampo2, sizeof(char), 55, bin_file_pointer);
    
    size_written += sizeof(char) * fwrite(&header->tagCampo3, sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(char) * fwrite(&header->desCampo3, sizeof(char), 55, bin_file_pointer);
    
    size_written += sizeof(char) * fwrite(&header->tagCampo4, sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(char) * fwrite(&header->desCampo4, sizeof(char), 55, bin_file_pointer);
    
    size_written += sizeof(char) * fwrite(&header->tagCampo5, sizeof(char), 1,  bin_file_pointer);
    size_written += sizeof(char) * fwrite(&header->desCampo5, sizeof(char), 55, bin_file_pointer);

    while (size_written < PAGE_SIZE) {
        fputc(EMPTY_CHAR, bin_file_pointer);
        size_written += (size_t) 1;
    }

    // printf("%zu\n", size_written);
    *file_size_in_bytes += (int) size_written;
}

/*
 *  
 * 
 */ 
void write_reg_to_bin(dataReg *to_write, FILE *bin_file_pointer, int *file_size_in_bytes) {

    size_t size_written; 

    size_written = 0;

    size_written += sizeof(char) * fwrite(&to_write->removido, sizeof(char), 1, bin_file_pointer);
    size_written += sizeof(int) * fwrite(&to_write->encadeamento, sizeof(int), 1, bin_file_pointer);
    size_written += sizeof(int) * fwrite(&to_write->nroInscricao, sizeof(int), 1, bin_file_pointer);
    size_written += sizeof(double) * fwrite(&to_write->nota, sizeof(double), 1, bin_file_pointer);
    size_written += sizeof(char) * fwrite(to_write->data, sizeof(char), 10, bin_file_pointer);

    // Se campo é nulo, não escreve nada
    if (to_write->indTamanhoCidade > 0) {
        // Senão, escreve, na ordem, indicador de tamanho, tag do campo e valor
        size_written += sizeof(int) * fwrite(&to_write->indTamanhoCidade, sizeof(int), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite("4", sizeof(char), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite(to_write->cidade, sizeof(char), strlen(to_write->cidade), bin_file_pointer);
    }

    // Se campo é nulo, não escreve nada    
    if (to_write->indTamanhoEscola > 0) {
        // Senão, escreve, na ordem, indicador de tamanho, tag do campo e valor
        size_written += sizeof(int) * fwrite(&to_write->indTamanhoEscola, sizeof(int), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite("5", sizeof(char), 1, bin_file_pointer);
        size_written += sizeof(char) * fwrite(to_write->nomeEscola, sizeof(char), to_write->indTamanhoEscola, bin_file_pointer);
    }

    // Preenche com EMPTY_CHAR até o final do registro
    for (int i = (int) size_written; i < REG_SIZE; ++i) {
        fputc(EMPTY_CHAR, bin_file_pointer);
        size_written += (size_t) 1;
    }

    // Atualiza contador de bytes do arquivo    
    *file_size_in_bytes += (int) size_written; 
}

// Imprime registro na tela conforme especificação
void print_reg_to_std(dataReg *reg) 
{
    // Sempre exite
	printf("%d ", reg->nroInscricao);
    
    // Se for valor nulo (-1) não imprime
    if (reg->nota > 0) {
        printf("%.1f ", reg->nota);
    }

    // Se comprimento maior que zero, imprime
    if (strlen(reg->data))
        printf("%s ", reg->data);
    
    // Se comprimento maior que zero, imprime número de caracteres e string
    if (reg->indTamanhoCidade > 0) {
        printf("%d ", reg->indTamanhoCidade);
        printf("%s ", reg->cidade);    
    }
    
    // Se comprimento maior que zero, imprime número de caracteres e string
    if (reg->indTamanhoEscola > 0) {
        printf("%d ", reg->indTamanhoEscola);
        printf("%s", reg->nomeEscola);   
    }
    
    printf("\n");

}

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_reg_from_csv(FILE *csv_file_pointer) {

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
    dataReg *reg = NULL;
    reg = (dataReg*) malloc(sizeof(dataReg));
    
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
        memset(buffer, '\0', BUFFER_LEN);

        // Percorre a linha
        for (index = 0; index < read; ++index) {

            // Preenhe o buffer até encontrar um caractere de separação de campos
            if ((line[index] != ',') && (line[index] != '\n')) {
                buffer[field_len] = line[index];
                field_len++;
            }
        
            // Quando encontrou um caractere de separação 
            else {
                
                // Usa a informação do índice do campo para decidir qual variável preencher na estrutura
                switch (field_index)
                {
                    // Campo nroInscricao
                    case 0:
                        {    
                            
                            char nro[field_len + 1];
                            strncpy(nro, buffer, field_len+1);
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
                                strncpy(reg->data, buffer, 10);
                            }
                        }
                        break;

                    // Campo cidade
                    case 3:
                        {
                            // Armazena indicador de tamanho
                            reg->indTamanhoCidade = field_len;

                            // Campo vazio aponta para null
                            if (reg->indTamanhoCidade == 0) {
                                reg->cidade = NULL;
                            }
                            else {
                                // Se o campo não é vazio, aloca o espaço neecssário para armazená-lo
                                reg->cidade = (char*) malloc(sizeof(char) * reg->indTamanhoCidade + 1);
                                strncpy(reg->cidade, buffer, reg->indTamanhoCidade + 1);
                            }
                       }   
                        break;

                    // Campo nome da escola
                    case 4:
                        {
                            // Armazena indicador de tamanho
                            reg->indTamanhoEscola = field_len;

                            // Campo vazio aponta para null
                            if (reg->indTamanhoEscola == 0) {
                                reg->nomeEscola = NULL;
                            }
                            
                            else {
                                // Se o campo não é vazio, aloca o espaço para armazená-lo
                                reg->nomeEscola = (char*) malloc(sizeof(char) * reg->indTamanhoEscola + 1);
                                strncpy(reg->nomeEscola, buffer, reg->indTamanhoEscola + 1);
                            }
                        }   
                        break;                    
                    default:
                        break;
                }
                
                // Reseta contador de tamanho do campo, incrementa índice de campo lido
                field_len = 0;
                field_index++;
                memset(buffer, '\0', BUFFER_LEN);

            }
        }     

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

void safely_free_reg(dataReg *reg) {

    // Evita free em memória não alocada    
    if (reg) { 

        if (reg->cidade != NULL){
            free(reg->cidade);
        }
        if (reg->nomeEscola != NULL){
            free(reg->nomeEscola);
        }
        free(reg);
    }
}