
#define TAM_MSG 64

typedef struct {
    char inicio;
    int tamanho : 5;
    int seqencia : 6;
    int tipo : 5;
    char *dados;
    char paridade;
} mensagem;
