// Bibliotecas da linguagem
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Bibliotecas criadas para solucionar o trabalho
#include "programaTrab1.h"

int main(int argc, char** argv) {

    char func_option;

    char *filename;
    char *output_filename = "ArquivoTrab1si.bin\0"; 

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
                printf("Falha no carregamento do arquivo de entrada.\n");
                exit(EXIT_FAILURE);
            }    

            if (output_fp == NULL) {
                printf("Falha no carregamento do arquivo de saída.\n");
                exit(EXIT_FAILURE);
            }
        
            if (input_fp != NULL && output_fp != NULL) {

                write_header_to_bin(header, output_fp, &written_bytes);
                
                // Pula primeira linha do csv
                fseek(input_fp, 41, SEEK_SET);
                
                // Le cada registro do csv e salva no binário
                reg = read_reg_from_csv(input_fp);
                while(reg != NULL) {
                    print_reg_to_std(reg);
                    write_reg_to_bin(reg, output_fp, &written_bytes);
                    
                    safely_free_reg(reg);
                    reg = read_reg_from_csv(input_fp);
                }
                printf("%s\n", output_filename);
            }

            
            else {
                printf("Falha no carregamento do arquivo.\n");
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
            input_fp = fopen(filename, "rb");
            read_reg_from_bin(input_fp);
            fclose(input_fp);
        }

    
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
void read_reg_from_bin(FILE *bin_file_pointer) {

    // Variáveis para leitura da póxima linha do arquivo usando a função getline da stdlib.
    char chunk[REG_SIZE];                   // Recebe o conteúdo da linha
    char buffer[BUFFER_LEN];                // Buffer para armazenar temporariamente os valores
    char aux;

    int reg_index;                          // Indice para percorrer os caracteres da linha
    int buffer_index;                       // Índice para percorrer os caracteres do buffer
    int indicador_temporario;

    // Aloca estrutura para receber o conteúdo lido do csv
    dataReg *reg = NULL;
    reg = (dataReg*) malloc(sizeof(dataReg));

    // Pula a página do cabeçalho
    fseek(bin_file_pointer, PAGE_SIZE, SEEK_SET);
    
    while (fread(chunk, sizeof(char), 80, bin_file_pointer) != 0) {
        
        reg_index = 0;

        // Nro da inscricao
        memset(buffer, '\0', BUFFER_LEN);
        for (buffer_index = 0; buffer_index < 4; buffer_index++) {
            buffer[buffer_index] = chunk[reg_index];
            reg_index++;
        }
        reg->nroInscricao = atoi(buffer);

        // Nota
        memset(buffer, '\0', BUFFER_LEN);
        for (buffer_index = 0; buffer_index < 4; buffer_index++) {
            buffer[buffer_index] = chunk[reg_index];
            reg_index++;
        }
        reg->nota = atof(buffer);

        // Data
        memset(buffer, '\0', BUFFER_LEN);
        for (buffer_index = 0; buffer_index < 10; buffer_index++) {
            buffer[buffer_index] = chunk[reg_index];
            reg_index++;
        }
        strncpy(reg->data, buffer, 10);

        // Procura pelo próximo campo de tamanho variável
        memset(buffer, '\0', BUFFER_LEN);
        for (buffer_index = 0; buffer_index < 4; buffer_index++) {
            buffer[buffer_index] = chunk[reg_index];
            reg_index++;
        }

        // Encontrou um campo
        if (strncmp(buffer, "@@@@", 4) != 0) {
            indicador_temporario = atoi(buffer);
            
            // Verifica se é cidade
            if (chunk[reg_index++] == '4') {
                
                // Verifica o indicador de tamanho
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < 4; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                reg->indTamanhoCidade = atoi(buffer);

                // Salva o valor
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < reg->indTamanhoCidade; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                strncpy(reg->cidade, buffer, reg->indTamanhoCidade);

            }
            else {
                 // Verifica o indicador de tamanho
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < 4; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                reg->indTamanhoEscola = atoi(buffer);

                // Salva o valor
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < reg->indTamanhoEscola; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                strncpy(reg->nomeEscola, buffer, reg->indTamanhoEscola);
            }  
        }
         // Procura pelo próximo campo de tamanho variável
        memset(buffer, '\0', BUFFER_LEN);
        for (buffer_index = 0; buffer_index < 4; buffer_index++) {
            buffer[buffer_index] = chunk[reg_index];
            reg_index++;
        }

        // Encontrou um campo
        if (strncmp(buffer, "@@@@", 4) != 0) {
            indicador_temporario = atoi(buffer);
        
            // Verifica se é cidade
            if (chunk[reg_index++] == '5') {
                 // Verifica o indicador de tamanho
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < 4; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                reg->indTamanhoEscola = atoi(buffer);

                // Salva o valor
                memset(buffer, '\0', BUFFER_LEN);
                for (buffer_index = 0; buffer_index < reg->indTamanhoEscola; buffer_index++) {
                    buffer[buffer_index] = chunk[reg_index];
                    reg_index++;
                }
                strncpy(reg->nomeEscola, buffer, reg->indTamanhoEscola);
            }    
        }

        print_reg_to_std(reg);
        safely_free_reg(reg);
        reg = (dataReg*) malloc(sizeof(dataReg));
    }         
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

    for (int i = (int) size_written; i < PAGE_SIZE; ++i) {
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
    size_written += sizeof(char) * fwrite(&to_write->data, sizeof(char), 10, bin_file_pointer);

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

        printf("%s", line);

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
                memset(&buffer[0], '\0', BUFFER_LEN);

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
    if (reg != NULL) { 

        if (reg->cidade != NULL){
            free(reg->cidade);
        }
        if (reg->nomeEscola != NULL){
            free(reg->nomeEscola);
        }
        free(reg);
    }
}