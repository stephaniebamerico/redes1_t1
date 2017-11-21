
#define TAM_MSG 64

typedef struct {
    char inicio;
    int tamanho : 5;
    int sequencia : 6;
    int tipo : 5;
    char *dados;
    char paridade;
} mensagem_t;

char* msg_to_cstr(mensagem_t msg) {
    char *cstr = (char *) malloc (sizeof(char) * (msg.tamanho + 4));
    
    if(!cstr) {
        cerr << "Erro ao alocar cstr." << endl;
        return NULL;
    }

    cstr[0] = msg.inicio;
    cstr[1] = (msg.tamanho << 3) + (msg.sequencia >> 3);
    cstr[2] = (msg.sequencia << 3) + msg.tipo;
    for (int i = 0; i < msg.tamanho; ++i) {
        cstr[3+i] = msg.dados[i];
    }
    cstr[3+msg.tamanho] = msg.paridade;

    return cstr;
}

mensagem_t cstr_to_msg(mensagem_t cstr) {
    if(!cstr) {
        cerr << "Erro ao converter: cstr nulo." << endl;
        return NULL;
    }

    mensagem_t msg;
    msg.inicio = cstr[0];
    msg.tamanho = (cstr[1] >> 3);
    msg.sequencia = (cstr[1] << 3) + (cstr[2] >> 3);
    msg.tipo = 
    
    for (int i = 0; i < msg.tamanho; ++i) {
        cstr[3+i] = msg.dados[i];
    }
    cstr[3+msg.tamanho] = msg.paridade;

    return cstr;
}
