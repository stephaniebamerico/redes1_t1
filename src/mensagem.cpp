#include "mensagem.h"

char* msg_to_cstr(mensagem_t msg) {
    char *cstr = (char *) malloc (sizeof(char) * (msg.tamanho + 4));
    
    if(!cstr) {
        cerr << "[msg_to_cstr] Erro ao alocar cstr." << endl;
        return NULL;
    }

    cstr[0] = msg.inicio;
    cstr[1] = msg.tamanho; //000TTTTT
    cstr[1] = (cstr[1] << 3) | (msg.sequencia >> 3); //TTTTTSSS
    cstr[2] = (msg.sequencia << 3); //00SSS000
    cstr[2] = (cstr[2] << 2) | msg.tipo; //SSSTTTTT
    for (int i = 0; i < msg.tamanho; ++i) {
        cstr[3+i] = msg.dados[i];
    }
    cstr[3+msg.tamanho] = msg.paridade;

    return cstr;
}

mensagem_t* cstr_to_msg(char *cstr) {
    if(!cstr) {
        cerr << "[cstr_to_msg] Erro ao converter: cstr nulo." << endl;
        return NULL;
    }

    mensagem_t *msg = (mensagem_t *) malloc(sizeof(mensagem_t));
    
    msg->inicio = cstr[0];
    msg->tamanho = (cstr[1] >> 3); //000TTTTT
    msg->sequencia = ((cstr[1] << 3) & 0x0038) | (cstr[2] >> 5); //00SSSSSS
    msg->tipo = (cstr[2] & 0x001F); //000TTTTT
    msg->paridade = cstr[3+(msg->tamanho)];

    msg->dados = (char *) malloc (sizeof(char) * (msg->tamanho + 4));
    if(!(msg->dados)) {
        cerr << "[cstr_to_msg] Erro ao alocar mensagem_t." << endl;
        return NULL;
    }
    
    for (int i = 0; i < msg->tamanho; ++i) {
        (msg->dados)[i] = cstr[3+i];
    }

    return msg;
}

void imprimeMensagem(mensagem_t msg) {
    printf("inicio: %c\n", msg.inicio);
    printf("tam: %d\n", msg.tamanho);
    printf("seq: %d\n", msg.sequencia);
    printf("tipo: %d\n", msg.tipo);
    printf("dados: %s\n", msg.dados);
    printf("par: %d\n", (int) msg.paridade);
}