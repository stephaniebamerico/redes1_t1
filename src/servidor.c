#include "raw_socket.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char const *argv[]) {
    char *msg = (char *) malloc(sizeof(char)*10);
    int socket;

    if((socket = openRawSocket("enp7s0")) < 0) {
        fprintf(stderr, "Erro ao abrir Raw Socket.\n");
        return 1;
    }

    printf("Aguardando mensagens.\n");

    while (1) {
        memset(msg, 0, 10);

        if(recv(socket, msg, 10, 0) < 0) {
            fprintf(stderr, "Erro ao receber mensagem.\n");
        }
        
        printf("\nMensagem recebida: %s\n", msg);
    }

    return 0;
}