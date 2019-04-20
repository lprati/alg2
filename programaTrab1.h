#ifndef programa_trab_1_header
#define programa_trab_1_header

#define PAGE_SIZE 16000             // Número de bytes da página de disco
#define REG_SIZE 80                 // Tamanho fixo do registro
#define EMPTY_CHAR '@'              // Caractere usado para indicar conteúdo vazio no arquivo binário
#define BUFFER_LEN 256              // Define comprimento padrão para buffers de leitura 

struct header {

    char status;
    int topoPilha;
    char tagCampo1[1];
    char desCampo1[55];
    char tagCampo2[1];
    char desCampo2[55];
    char tagCampo3[1];
    char desCampo3[55];
    char tagCampo4[1];
    char desCampo4[55];
    char tagCampo5[1];
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

dataReg * read_next_data_reg(FILE* csv_file_pointer);
void write_reg_to_file(dataReg *to_write, FILE* bin_file_pointer, int *file_size_in_bytes);

#endif