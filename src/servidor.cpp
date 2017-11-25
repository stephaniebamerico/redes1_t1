#include "raw_socket.h"
#include "mensagem.h"

int main(int argc, char const *argv[]) {
    char *msg = (char *) malloc(sizeof(char)*TAM_MSG);
    int socket = openRawSocket((char *) argv[1]);

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    cout << "Aguardando mensagens." << endl;
    mensagem_t *msg_recebida = (mensagem_t *) malloc(sizeof(mensagem_t));
    while (1) {
        memset(msg, 0, TAM_MSG);
        
        if(recebe_mensagem(socket, msg_recebida)) {
            msg_recebida = cstr_to_msg(msg, msg_recebida);
            cout << endl << "Mensagem recebida: " << endl;
            imprime_mensagem(*msg_recebida);
                
            envia_confirmacao(socket, ACK);

            mensagem_t *msg = monta_mensagem(OK, 0, "");
                
            cout << endl << "Mensagem montada: " << endl;
            imprime_mensagem(*msg);
            envia_mensagem(socket, msg);

            cout << "Enviou mensagem montada" << endl;
        }
    }

    return 0;
}