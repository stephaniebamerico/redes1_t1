#include "mensagem.h"
#include "raw_socket.h"

void cstr_tam_seq_tipo_dados(mensagem_t msg, char *cstr, int pos_inicial) {
    cstr[pos_inicial] = msg.tamanho; //000TTTTT
    cstr[pos_inicial] = (cstr[pos_inicial] << 3) | (msg.sequencia >> 3); //TTTTTSSS
    cstr[pos_inicial+1] = (msg.sequencia << 3); //00SSS000
    cstr[pos_inicial+1] = (cstr[pos_inicial+1] << 2) | msg.tipo; //SSSTTTTT
    
    if(msg.tamanho > 0)
        for (int i = 0; i < msg.tamanho; ++i) {
            cstr[pos_inicial+2+i] = msg.dados[i];
        }
}

char* msg_to_cstr(mensagem_t *msg, char* cstr) {
    if(!cstr || !msg) {
        cerr << "[msg_to_cstr] Erro ao converter: parametro nulo." << endl;
        return NULL;
    }

    cstr[0] = msg->inicio;
    cstr_tam_seq_tipo_dados(*msg, cstr, 1);
    cstr[3+msg->tamanho] = msg->paridade;

    return cstr;
}

mensagem_t* cstr_to_msg(char *cstr, mensagem_t *msg) {
    if(!cstr || !msg) {
        cerr << "[cstr_to_msg] Erro ao converter: parametro nulo." << endl;
        return NULL;
    }
    
    msg->inicio = cstr[0];
    msg->tamanho = (cstr[1] >> 3); //000TTTTT
    msg->sequencia = ((cstr[1] << 3) & 0x0038) | (cstr[2] >> 5); //00SSSSSS
    msg->tipo = (cstr[2] & 0x001F); //000TTTTT
    msg->paridade = cstr[3+(msg->tamanho)];
    
    if(msg->tamanho > 0) {
        msg->dados = (char *) malloc (sizeof(char) * msg->tamanho);
        if(!(msg->dados)) {
            cerr << "[cstr_to_msg] Erro ao alocar dados." << endl;
            return NULL;
        }
        
        for (int i = 0; i < msg->tamanho; ++i) {
            (msg->dados)[i] = cstr[3+i];
        }
    }

    return msg;
}

mensagem_t* monta_mensagem(int tipo, int sequencia, string args) {
    mensagem_t *msg = (mensagem_t *) malloc(sizeof(mensagem_t));
    msg->inicio = 0x007E;
    msg->tamanho = args.size();
    msg->sequencia = sequencia;
    msg->tipo = tipo;
    if(msg->tamanho > 0) {
        msg->dados = (char *) malloc(sizeof(char) * msg->tamanho);
        strcpy(msg->dados, args.c_str());
    }
    msg->paridade = calcula_paridade(*msg);

    return msg;
}

char calcula_paridade(mensagem_t msg) {
    char *m = (char *) malloc(msg.tamanho + 2);

    cstr_tam_seq_tipo_dados(msg, m, 0);

    char paridade = 0;
    for (int i = 0; i < msg.tamanho; ++i) {
        paridade = paridade | m[i];
    }

    free(m);

    return paridade;
}

void imprime_mensagem(mensagem_t msg) {
    printf("inicio: %c\n", msg.inicio);
    printf("tam: %d\n", msg.tamanho);
    printf("seq: %d\n", msg.sequencia);
    printf("tipo: %d\n", msg.tipo);
    printf("dados: %s\n", msg.dados);
    printf("par: %d\n", (int) msg.paridade);
}

bool cd_remoto(int socket, string args) {
    // Cria mensagem
    mensagem_t *msg = monta_mensagem(6, 0, args);
    cout << "Mensagem montada: " << endl;
    imprime_mensagem(*msg);

    // Envia ao servidor
    if(envia_mensagem(socket, msg)) {
        // Recebe resposta para requisicao
        free(msg->dados);
        free(msg);
        msg = (mensagem_t *) malloc(sizeof(mensagem_t));

        while(msg->tipo != OK) {
            recebe_mensagem(socket, msg);
        }

        envia_confirmacao(socket, ACK);
        
        if(msg->tipo == OK) {
            cout << "Recebi OK" << endl;
            return true;
        }
        else if(msg->tipo == ERRO) {
            cout << "Erro ao executar comando cd " << args << endl;
        }
    }

    // Erro no envio da mensagem ou ERRO como resposta
    return false;
}