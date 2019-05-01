// ======================================================================== //
//  Disciplina: SCC0203 - Algoritmos e Estruturas de Dados II               //
//  Projeto Prático 1                                                       //
//                                                                          //
//  Elaborado por: Leonardo Giovanni Prati              #USP: 8300079       //
//  Data: 30-04-2019                                                        //
// ======================================================================== //

// Bibliotecas da linguagem
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "programaTrab1.h"

int main() {

    char func_option;                               // Armazenará a opção escolhida

    // Argumentos da operação 3
    char *field_name;                               // Campo usado para busca
    char *key_to_search;                            // Valor usado na comparação

    char *filename;                                 // Nome de arquivo de entrada
    char *output_filename = "arquivoTrab1si.bin";   // Nome padrão do arquivo de saída da operação 1

    // Estruturas de cabeçalho e registro 
    dataReg *reg = NULL;                            // Usado em todas as operações
    fileHeader *header = NULL;                      // Usado na operação 1

    // Ponteiros para arquivo
    FILE *input_fp = NULL;                          // Usado em todas as operações
    FILE *output_fp = NULL;                         // Usado na operação 2

    int written_bytes = 0;                          // Contador de bytes escritos    
    int read_bytes = 0;                             // Contador de bytes lidos
    int pag_de_disco = 0;                           // Contador de páginas de disco percorridas

    // Contador de comparações positivas para busca (operação 3)
    int found = 0;     

    // RRN selecionado para exibição (operação 4)
    int rrn; 

    // Contador e vetor de argumentos
    int argc = 0;
    char **argv;

    // Recebe os argumentos da entrada padrão
    argv = get_args(&argc);

    // Operação selecionada
    func_option = argv[0][0];
    
    // Desvia o fluxo de acordo com a operação
    switch (func_option)
    {
        case '1':
        {

            // Abre arquivos de entrada e saída
            filename = argv[1];
            input_fp = fopen(filename, "r");
            output_fp = fopen(output_filename, "wb+");

            // Garante abertura dos arquivos        
            if (input_fp != NULL && output_fp != NULL) {
                
                // Cria e preenche estrutura de cabeçalho
                header = create_initialized_header();
                // Escreve o cabeçalho no arquivo binário
                write_header_to_bin(header, output_fp, &written_bytes);
                
                // Pula primeira linha do csv, que contém apenas os nomes dos campos
                fseek(input_fp, 41, SEEK_SET);
                
                // Le cada registro do csv e salva no binário
                // Lê primeiro registro
                reg = read_reg_from_csv(input_fp);
                // Se for não nulo
                while(reg != NULL) {
                    // Escreve registro no arquivo binário
                    write_reg_to_bin(reg, output_fp, &written_bytes);
                    // Libera memória
                    safely_free_reg(reg);

                    // Repete
                    reg = read_reg_from_csv(input_fp);
                }
                // Especificação: imprime nome do arquivo de saída na saída padrão
                printf("%s", output_filename);
            }
            // Mensagem em caso de falha na abertura de algum arquivo
            else {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }

            // Libera os ponteiros
            if (reg)
                free(reg);

            if (header)
                free(header);

            // Fecha os arquivos
            fclose(input_fp);

            // Indica coerência do arquivo criado antes de fechá-lo
            fseek(output_fp, 0, SEEK_SET);
            fputc('1', output_fp);
            fclose(output_fp);
        }
        break;

        // Operação 2: Recupera registros de arquivo binário e imprime na tela
        case '2':
        {

            // Abre arquivo em modo binário
            filename = argv[1];
            input_fp = fopen(filename, "rb");

            // Garante condição do ponteiro
            if (input_fp != NULL) {
               
                // Verifica byte de consistência do arquivo
                if (fgetc(input_fp) == '0') {
                    printf("Falha no processamento do arquivo.");
                    exit(0);
                }
                read_bytes += 1;

                // Pula cabeçalho
                fseek(input_fp, PAGE_SIZE, SEEK_SET);

                // Percorre arquivo binário imprimindo os registros recuperados na saída padrão
                reg = read_reg_from_bin(input_fp, &read_bytes);
                while(reg) {
                    print_reg_to_std(reg);
                    safely_free_reg(reg);
                    reg = read_reg_from_bin(input_fp, &read_bytes);

                }

                // Conta quantas páginas de disco foram percorridas
                pag_de_disco = read_bytes / PAGE_SIZE;
                
                // Verifica se alguma página foi percorrida parcialmente e contabiliza também
                if (read_bytes % PAGE_SIZE) {
                    pag_de_disco++;
                }

                // Imprime páginas acessadas
                printf("Número de páginas de disco acessadas: %d", pag_de_disco);
            }

            // Caso abertura de arquivo tenha falhado
            else {
                printf("Falha no processamento do arquivo.");
                exit(0);
            }

            // Libera memória
            if (reg)
                free(reg);
            
            // Fecha arquivo
        
            fclose(input_fp);
        }
        break;

        // Operação '3': Busca por campo/valor
        case '3':
        {
            
            read_bytes = 0;                              // Contabiliza bytes percorridos no arquivo
            found = 0;                                   // Contabiliza entradas que corresponderam à busca

            filename = argv[1];                         // argv[1] é o nome do arquivo de entrada
            field_name = argv[2];                       // field_name é o campo que será usado para busca
            key_to_search = argv[3];                    // key_to_search é o valor pelo qual se procura 

            // Abre arquivo binário para leitura
            input_fp = fopen(argv[1], "rb");
            
            // Garante condição do ponteiro
            if (input_fp != NULL) {
            
                // Verifica byte de consistência do arquivo
                if (fgetc(input_fp) == '0') {
                    printf("Falha no processamento do arquivo.");
                    exit(0);
                }

                fseek(input_fp, PAGE_SIZE, SEEK_SET);
                read_bytes += PAGE_SIZE;

                // Varre todos os registros
                reg = read_reg_from_bin(input_fp, &read_bytes);
                while(reg) {
                    // Verifica se o registro atual é positivo para a chave/valor informada
                    if (match_reg(reg, field_name, key_to_search)){
                        
                        // Se sim, imprime na saída padrão
                        print_reg_to_std(reg);
                        found++;
                    }

                    // Busca por nroInscricao deve ser interrompida no primeiro resultado positivo,
                    // já que o valor nunca se repete
                    if ((strcmp(field_name, "nroInscricao") == 0) && found)
                        break;
                    // Outras buscas vão até o fim do arquivo
                    else
                        reg = read_reg_from_bin(input_fp, &read_bytes);
                    
                }
                // Mensagem caso nenhum registro tenha sido encontrado
                if (found == 0) {
                    printf("Registro inexistente.\n");
                }

                // Se encontrou algum registro, também imprime o num de páginas percorridas
                else {

                    pag_de_disco = read_bytes / PAGE_SIZE;

                    // Contabiliza páginas parcialmente percorridas
                    if (read_bytes % PAGE_SIZE) {
                        pag_de_disco++;
                    }
                    printf("Número de páginas de disco acessadas: %d", pag_de_disco);
                }
            }

            // Se o houve falha ao abrir o arquivo para leitura, imprime e encerra
            else {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }
        }
        break;

         // Operação '4': Busca por RRN
        case '4':
        {
            
            read_bytes = 0;                             // Contabiliza bytes percorridos no arquivo

            filename = argv[1];                         // argv[1] é o nome do arquivo de entrada
            rrn = atoi(argv[2]);                        // rrn procurado

            // Abre arquivo binário para leitura
            input_fp = fopen(argv[1], "rb");
            
            // Garante condição do ponteiro
            if (input_fp != NULL) {
            
                // Verifica byte de consistência do arquivo
                if (fgetc(input_fp) == '0') {
                    printf("Falha no processamento do arquivo.");
                    exit(0);
                }

                fseek(input_fp, PAGE_SIZE, SEEK_SET);
                read_bytes += PAGE_SIZE;

                // Avança, a partir da posição inicial, o número de registros até alcançar o RRN fornecido
                fseek(input_fp, rrn * REG_SIZE, SEEK_CUR);

                // Lê o registro correspondente
                reg = read_reg_from_bin(input_fp, &read_bytes);
                
                // Mensagem caso o registro não exista
                if (reg == NULL) {
                    printf("Registro inexistente.\n");
                }
                
                // Se encontrou algum registro, imprime o registro e também imprime o num de páginas percorridas
                else {
                    
                    print_reg_to_std(reg);
                    pag_de_disco = read_bytes / PAGE_SIZE;

                    // Contabiliza páginas parcialmente percorridas
                    if (read_bytes % PAGE_SIZE) {
                        pag_de_disco++;
                    }
                    printf("Número de páginas de disco acessadas: %d", pag_de_disco);
                }
            }\

            // Se o houve falha ao abrir o arquivo para leitura, imprime e encerra
            else {
                printf("Falha no carregamento do arquivo.");
                exit(0);
            }

            if (reg)
                free(reg);
            fclose(input_fp);
        }
        break;

        default:
            break;
    } 

    // Libera vetor de argumentos, que foi alocado na chamada da função "get_args"
    if (argv)
        free(argv);

    return 0;
}

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada binário.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *bin_file_pointer:   Ponteiro válido para um arquivo binário. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 *
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
                reg->indTamanhoCidade = indicador_temporario - 1;
                reg->cidade = (char*) malloc(reg->indTamanhoCidade * sizeof(char));
                read_bytes += sizeof(char) * fread(reg->cidade, sizeof(char), reg->indTamanhoCidade, bin_file_pointer);

            }
            else if (tag == '5') {
                reg->indTamanhoEscola = indicador_temporario - 1;
                reg->nomeEscola = (char*) malloc(reg->indTamanhoEscola * sizeof(char));
                read_bytes += sizeof(char) * fread(reg->nomeEscola, sizeof(char), reg->indTamanhoEscola, bin_file_pointer);

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
                reg->indTamanhoCidade = indicador_temporario - 1;
                reg->cidade = (char*) malloc(reg->indTamanhoCidade * sizeof(char));
                read_bytes += sizeof(char) * fread(reg->cidade, sizeof(char), reg->indTamanhoCidade, bin_file_pointer);

            }
            else if (tag == '5') {
                reg->indTamanhoEscola = indicador_temporario - 1;
                reg->nomeEscola = (char*) malloc(reg->indTamanhoEscola * sizeof(char));
                read_bytes += sizeof(char) * fread(reg->nomeEscola, sizeof(char), reg->indTamanhoEscola, bin_file_pointer);
            }

        }    
    
        int deslocamento = 80 - read_bytes;
        read_bytes += deslocamento;

        fseek(bin_file_pointer, deslocamento, SEEK_CUR);
        *size_read += read_bytes;

        // Verifica se o registro é um registro removido
        if (reg->removido == '*') {
            safely_free_reg(reg);
            return NULL;
        }

        return reg;
    }

    return NULL;
}



/*
 * Aloca e preenche o cabeçalho de arquivo. 
 * 
 * @return fileHeader: ponteiro para o cabeçalho criado. 
 */
fileHeader * create_initialized_header() {

    int fixed_len = 55;                                         // Tamanho dos campos fixos
    size_t string_len;                                          // Contador de tamanho das strings variáveis

    fileHeader *new_header;                                     // Declaração do ponteiro
    new_header = (fileHeader*) malloc(sizeof(fileHeader));      // Alocação do ponteiro


    new_header->status = '0';
    new_header->topoPilha = -1;

    /* Campo 1 */ 
    // Tag 
    new_header->tagCampo1 = '1';

    // Escreve descrição
    strcpy(new_header->desCampo1, "numero de inscricao do participante do ENEM\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo1) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo1 + string_len), EMPTY_CHAR, (fixed_len - string_len));


    /* Campo 2 */ 
    // Tag 
    new_header->tagCampo2 = '2';

    // Escreve descrição
    strcpy(new_header->desCampo2, "nota do participante do ENEM na prova de matematica\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo2) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo2 + string_len), EMPTY_CHAR, (fixed_len - string_len));

    /* Campo 3 */ 
    // Tag 
    new_header->tagCampo3 = '3';

    // Escreve descrição
    strcpy(new_header->desCampo3, "data\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo3) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo3 + string_len), EMPTY_CHAR, (fixed_len - string_len));


    /* Campo 4 */
    // Tag 
    new_header->tagCampo4 = '4';

    // Escreve descrição
    strcpy(new_header->desCampo4, "cidade na qual o participante do ENEM mora\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo4) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo4 + string_len), EMPTY_CHAR, (fixed_len - string_len));


    /* Campo 5 */ 
    // Tag 
    new_header->tagCampo4 = '5';

    // Escreve descrição
    strcpy(new_header->desCampo5, "nome da escola de ensino medio\0");
   
    // Calcula espaço preenchido
    string_len = strlen(new_header->desCampo5) + 1;
   
    // Ocupa restante com caracter vazio
    memset((new_header->desCampo5 + string_len), EMPTY_CHAR, (fixed_len - string_len));

    return new_header;
}


/*
 * Escreve um cabeçalho pré alocado e preenchido num arquivo binário.
 * 
 * @param fileHeader *header:       Ponteiro para um cabeçalho previamente inicializado
 * @param FILE *bin_file_pointer:   Ponteiro para um arquivo aberto em modo escrita em binário
 * @param int *file_size_in_bytes:  Ponteiro para um inteiro que registra o tamanho em bytes escrito no arquivo
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
 * Escreve um registro no arquivo binário fornecido
 * 
 * @param dataReg *reg:             Ponteiro para um registro previamente inicializado
 * @param FILE *bin_file_pointer:   Ponteiro para um arquivo aberto em modo escrita em binário
 * @param int *file_size_in_bytes:  Ponteiro para um inteiro que registra o tamanho em bytes escrito no arquivo
 */ 
void write_reg_to_bin(dataReg *to_write, FILE *bin_file_pointer, int *file_size_in_bytes) {

    // Inicializa contagem dos bytes escritos na chamada atual
    size_t size_written; 
    size_written = 0;

    // Escreve campos de tamanho fixo
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
        size_written += sizeof(char) * fwrite(to_write->cidade, sizeof(char), to_write->indTamanhoCidade, bin_file_pointer);
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

    // Atualiza contador de bytes do arquivo com o valor de bytes escritos n 
    *file_size_in_bytes += (int) size_written; 
}

/*
 * Imprime na saída padrão os campos de um registro conforme especificação do projeto
 * 
 * @param dataReg *reg:          Ponteiro para um registro previamente inicializado
 */ 
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
        printf("%d ", reg->indTamanhoCidade - 1);
        printf("%s ", reg->cidade);    
    }
    
    // Se comprimento maior que zero, imprime número de caracteres e string
    if (reg->indTamanhoEscola > 0) {
        printf("%d ", reg->indTamanhoEscola - 1);
        printf("%s", reg->nomeEscola);   
    }
    
    printf("\n");

}

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * 
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

/*
 * Dado um ponteiro para registro, libera seus campos alocados e, depois, o próprio ponteiro
 * 
 * @param dataReg *reg:   Ponteiro para um registro
 */
void safely_free_reg(dataReg *reg) {

    // Os if's evitam free em memória não alocada    
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

/*
 * Recebe argumentos da entrada padrão, divide as entradas nos campos que serão utilizados pelo programa e retorna.
 * 
 * @param int *argc:            Ponteiro para inteiro que conta o número de parâmetros lidos
 * 
 * @return char **argv:         Ponteiro para um vetor de strings, cada posição contendo um argumento recebido da entrada padrão
 */
char ** get_args(int *argc) {

    char token[32];             // Armazena temporariamente cada argumento da linha
 
    // Inicializa o buffers
    memset(token, '\0', sizeof(token));

    // Cria vetor de strings para retornar
    char **argv = (char **) malloc(sizeof(char*) * 4);
    
    *argc = 0;

    // Inicializa cada posição com NULL
    for (int i = 0; i < 4; ++i) {
        argv[i] = NULL;
    }


    // Lê operação
    scanf("%s", token);
    argv[*argc] = (char*) malloc(sizeof(char) * strlen(token));
    memcpy(argv[*argc], token, strlen(token));
    (*argc)++;
    memset(token, '\0', sizeof(token));

    // Lê arquivo de entrada
    scanf("%s", token);
    argv[1] = (char*) malloc(sizeof(char) * strlen(token));
    memcpy(argv[*argc], token, strlen(token));
    (*argc)++;
    memset(token, '\0', sizeof(token));


    // Se a operação é "3" ou "4", lê os parâmetros adicionais

    // Caso de busca por campo/valor
    if (strcmp(argv[0], "3") == 0) {

        // Lê o campo
        scanf("%s", token);
        argv[*argc] = (char*) malloc(sizeof(char) * strlen(token));
        memcpy(argv[*argc], token, strlen(token));
        (*argc)++;
        memset(token, '\0', sizeof(token));
        
        // Lê a chave de busca (pode conter espaços)
        scanf("%[^\n]", token);
        argv[*argc] = (char*) malloc(sizeof(char) * strlen(token));
        // token + 1 para ignorar o primeiro caracter, que será um espaço
        memcpy(argv[*argc], token + 1, strlen(token));
        (*argc)++;
        memset(token, '\0', sizeof(token));
    }

    // Caso de busca por RRN
    if (strcmp(argv[0], "4") == 0) {
        scanf("%s", token);
        argv[*argc] = (char*) malloc(sizeof(char) * strlen(token));
        memcpy(argv[*argc], token, strlen(token));
        (*argc)++;
        memset(token, '\0', sizeof(token));
    }

    // Retorna vetor lido
    return argv;
}

/*
 * Verifica se o registro fornecido contém o valor informado para campo informado
 *  
 * @param dataReg *reg:                 Ponteiro para um registro
 * @param char* field_name:             Nome do campo no qual será efetuada a comparação
 * @param char *key_to_search:          Valor que deve ser encontrado para confirmar a comparação
 * 
 * @return int match:                   1 caso o registro tenha o valor fornecido para o campo escolhido. 0 caso contrário. 
 */
int match_reg(dataReg *reg, char *field_name, char *key_to_search) {

    // Atua como bool
    int match = 0;

    // Os if's externos verificam o nome do campo que deve ser comparado.

    // Os if's aninhados internamente tem como objetivo garantir que o campo existe, e, caso afirmativo,
    // comparar o valor fornecido com o existente no registro.

    // nroInscricao
    if (strcmp(field_name, "nroInscricao") == 0) {
        
        if (reg->nroInscricao == atoi(key_to_search)) {
            match = 1;
        }
    
    }

    // data
    if (strcmp(field_name, "data") == 0) {

        if (strlen(reg->data) == 10) {
            if (strcmp(reg->data, key_to_search) == 0) {
                match = 1;
            }
        }
    
    }

    // nota
    if (strcmp(field_name, "nota") == 0) {
        
        if (reg->nota > 0.0) {
            if (reg->nota == atof(key_to_search)) {
                match = 1;
            }
        }
    
    }

    // cidade
    if (strcmp(field_name, "cidade") == 0) {
        if (reg->cidade != NULL) {
            if (strcmp(reg->cidade, key_to_search) == 0) {
                match = 1;
            }
        }    
    
    }

    // nomeEscola
    if (strcmp(field_name, "nomeEscola") == 0) {
        if(reg->nomeEscola != NULL) {
            if (strcmp(reg->nomeEscola, key_to_search) == 0) {
                match = 1;
            }
        }
    
    }

    return match;
}