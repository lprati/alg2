// ======================================================================== //
//  Disciplina: SCC0203 - Algoritmos e Estruturas de Dados II               //
//  Projeto Prático 1                                                       //
//                                                                          //
//  Elaborado por: Leonardo Giovanni Prati              #USP: 8300079       //
//  Data: 30-04-2019                                                        //
// ======================================================================== //

// Include guard
#ifndef programa_trab_1_header      
#define programa_trab_1_header      


#define PAGE_SIZE 16000             // Número de bytes da página de disco
#define REG_SIZE 80                 // Tamanho fixo do registro
#define EMPTY_CHAR '@'              // Caractere usado para indicar conteúdo vazio no arquivo binário
#define BUFFER_LEN 256              // Define comprimento padrão para buffers de leitura 

struct header {

    char status;                    // Indica o estado de consistência do arquivo. 1 = consistente, 0 = inconsistente
    int topoPilha;                  // Aponta para o topo da pilha
    char tagCampo1;                 // Tag do campo
    char desCampo1[55];             // Descrição do campo
    char tagCampo2;                 
    char desCampo2[55];
    char tagCampo3;
    char desCampo3[55];
    char tagCampo4;
    char desCampo4[55];
    char tagCampo5;
    char desCampo5[55];

};

struct data {

    char removido;                  // '*' se o registro foi removido, '-' caso contrário
    int encadeamento;               // Aponta para o próximo registro no encadeamento
    int nroInscricao;               // Campo 1
    double nota;                    // Campo 2
    char data[10];                  // Campo 3
    int indTamanhoCidade;           // Indicador de tamanho do campo 4
    char *cidade;                   // Campo 4
    int indTamanhoEscola;           // Indicador de tamanho do campo 5
    char *nomeEscola;               // Campo 5

};

typedef struct header fileHeader;
typedef struct data dataReg;

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * 
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_reg_from_csv(FILE *csv_file_pointer); // Review

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada binário.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *bin_file_pointer:   Ponteiro válido para um arquivo binário. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 *
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_reg_from_bin(FILE *bin_file_pointer, int *size_read); 


/*
 * Escreve um registro no arquivo binário fornecido
 * 
 * @param dataReg *reg:             Ponteiro para um registro previamente inicializado
 * @param FILE *bin_file_pointer:   Ponteiro para um arquivo aberto em modo escrita em binário
 * @param int *file_size_in_bytes:  Ponteiro para um inteiro que registra o tamanho em bytes escrito no arquivo
 */ 
void write_reg_to_bin(dataReg *to_write, FILE *bin_file_pointer, int *file_size_in_bytes); // Review


/*
 * Aloca e preenche o cabeçalho de arquivo. 
 * 
 * @return fileHeader: ponteiro para o cabeçalho criado. 
 */
fileHeader * create_initialized_header(); 

/*
 * Escreve um cabeçalho pré alocado e preenchido num arquivo binário.
 * 
 * @param fileHeader *header:       Ponteiro para um cabeçalho previamente inicializado
 * @param FILE *bin_file_pointer:   Ponteiro para um arquivo aberto em modo escrita em binário
 * @param int *file_size_in_bytes:  Ponteiro para um inteiro que registra o tamanho em bytes escrito no arquivo
 */
void write_header_to_bin(fileHeader *header, FILE *bin_file_pointer, int *file_size_in_bytes); // Review

/*
 * Imprime na saída padrão os campos de um registro conforme especificação do projeto
 * 
 * @param dataReg *reg:          Ponteiro para um registro previamente inicializado
 */ 
void print_reg_to_std(dataReg *to_write);

/*
 * Dado um ponteiro para registro, libera seus campos alocados e, depois, o próprio ponteiro
 * 
 * @param dataReg *reg:   Ponteiro para um registro
 */
void safely_free_reg(dataReg *reg);


/*
 * Recebe argumentos da entrada padrão, divide as entradas nos campos que serão utilizados pelo programa e retorna.
 * 
 * @param int *argc:            Ponteiro para inteiro que conta o número de parâmetros lidos
 * 
 * @return char **argv:         Ponteiro para um vetor de strings, cada posição contendo um argumento recebido da entrada padrão
 */
char ** get_args(int *argc);

/*
 * Verifica se o registro fornecido contém o valor informado para campo informado
 *  
 * @param dataReg *reg:                 Ponteiro para um registro
 * @param char* field_name:             Nome do campo no qual será efetuada a comparação
 * @param char *key_to_search:          Valor que deve ser encontrado para confirmar a comparação
 * 
 * @return int match:                   1 caso o registro tenha o valor fornecido para o campo escolhido. 0 caso contrário. 
 */
int match_reg(dataReg *reg, char *field_name, char *key_to_search);

#endif