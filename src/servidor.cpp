#include "raw_socket.h"
#include "mensagem.h"

int main(int argc, char const *argv[]) {
    char *msg = (char *) malloc(sizeof(char)*TAM_MSG);
    int socket;

    if((socket = openRawSocket((char *) "enp7s0")) < 0) {
        fprintf(stderr, "Erro ao abrir Raw Socket.\n");
        return 1;
    }

    printf("Aguardando mensagens.\n");

    while (1) {
        memset(msg, 0, TAM_MSG);

        if(recv(socket, msg, TAM_MSG, 0) < 0) {
            fprintf(stderr, "Erro ao receber mensagem.\n");
        }
        
        printf("\nMensagem recebida: %s\n", msg);
    }

    return 0;
}