
#define TAM_MSG 64

typedef struct {
    char inicio;
    int tamanho : 5;
    int sequencia : 6;
    int tipo : 5;
    char *dados;
    char paridade;
} mensagem_t;

char* msg_to_cstr(mensagem_t msg);

mensagem_t cstr_to_msg(mensagem_t cstr);
