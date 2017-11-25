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
        
        cout << endl << "Mensagem recebida: " << endl;
        mensagem_t *msg_recebida = cstr_to_msg(msg);
        if(msg_recebida->inicio == 0x007E)
            imprimeMensagem(*msg_recebida);
    }

    return 0;
}