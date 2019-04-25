#ifndef programa_trab_1_header
#define programa_trab_1_header

#define PAGE_SIZE 16000             // Número de bytes da página de disco
#define REG_SIZE 80                 // Tamanho fixo do registro
#define EMPTY_CHAR '@'              // Caractere usado para indicar conteúdo vazio no arquivo binário
#define BUFFER_LEN 256              // Define comprimento padrão para buffers de leitura 

struct header {

    char status;
    int topoPilha;
    char tagCampo1;
    char desCampo1[55];
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

    char removido;
    int encadeamento;
    int nroInscricao;
    double nota;
    char data[10];
    int indTamanhoCidade;
    char *cidade;
    int indTamanhoEscola;
    char *nomeEscola;    

};

typedef struct header fileHeader;
typedef struct data dataReg;

/* 
 * Função para recuperar um registro de dados a partir do arquivo de entrada csv.
 * Chamadas subsequentes para o mesmo arquivo recuperam as linhas sequencialmente.
 * 
 * @param FILE *csv_file_pointer:   Ponteiro válido para um arquivo csv. Assume-se que o arquivo está na formatação pré estabelecida para o projeto.
 * @return dataReg* reg:            Ponteiro para uma estrutura dataReg contendo os dados lidos. Caso não tenha sido possível recuperar nenhum dado, retorna NULL.
 */ 
dataReg * read_reg_from_csv(FILE *csv_file_pointer); // Review

/* 
 *
 * 
 */ 
int read_reg_from_bin(FILE *bin_file_pointer); 

/* 
 *
 * 
 */ 
void write_reg_to_bin(dataReg *to_write, FILE *bin_file_pointer, int *file_size_in_bytes); // Review

/* 
 *
 * 
 */
void fill_up_page(FILE *bin_file_pointer, int *file_size_in_bytes);

/* 
 *
 * 
 */
fileHeader * create_initialized_header(); 

/* 
 *
 * 
 */
void write_header_to_bin(fileHeader *header, FILE *bin_file_pointer, int *file_size_in_bytes); // Review

/* 
 *
 * 
 */
void print_reg_to_std(dataReg *to_write);

void safely_free_reg(dataReg *reg);

#endif