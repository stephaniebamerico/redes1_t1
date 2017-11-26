#ifndef __RAW_SOCKET__
#define __RAW_SOCKET__

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <mensagem.h>
#include <poll.h>

#define TIMEOUT 0.5
#define WAIT 100

using namespace std;

void inicia_socket(int socket);

int openRawSocket(char *device);

bool recebe_mensagem(int socket, mensagem_t *msg);

void envia_mensagem(int socket, mensagem_t **msg, int tam);

void envia_confirmacao(int socket, int seq, int tipo);

#endif
