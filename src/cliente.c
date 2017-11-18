#include "raw_socket.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char const *argv[]) {
    char *buffer;
    int socket;

    if(!(buffer = (char *) malloc(sizeof(char)*10))) {
        fprintf(stderr, "Erro ao alocar buffer.\n");
        return 1;
    }

    if((socket = openRawSocket("enp7s0")) < 0) {
        fprintf(stderr, "Erro ao abrir Raw Socket.\n");
        return 1;
    }

    int t = sizeof(buffer);
    memset(buffer, 0, t);
    strcpy(buffer, "tudo bem?\n");

    if(send(socket, buffer, strlen(buffer), 0) < 0) {
        fprintf(stderr, "Erro ao enviar mensagem.\n");
        return 1;
    }

    printf("\nMensagem enviada com sucesso.\n");

    return 0;
}