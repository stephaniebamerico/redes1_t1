#include "raw_socket.h"
#include "mensagem.h"

int main(int argc, char const *argv[]) {
    char *msg = (char *) malloc(sizeof(char)*TAM_MSG);
    int socket = openRawSocket((char *) "enp7s0");

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    cout << "Aguardando mensagens." << endl;

    while (1) {
        memset(msg, 0, TAM_MSG);

        if(recv(socket, msg, TAM_MSG, 0) < 0) {
            cerr << "Erro ao receber mensagem." << endl;
        }
        
        if(msg[0] == 0x007E) {
            mensagem_t *msg_recebida = (mensagem_t *) malloc(sizeof(mensagem_t));
            msg_recebida = cstr_to_msg(msg, msg_recebida);

            cout << endl << "Mensagem recebida: " << endl;
            imprime_mensagem(*msg_recebida);
            
            envia_confirmacao(socket, ACK);
            mensagem_t *msg = monta_mensagem(OK, 0, "");
            envia_mensagem(socket, msg);
        }
    }

    return 0;
}