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
#include <unistd.h>
#include <mensagem.h>
#include <poll.h>

#define TIMEOUT 20

using namespace std;

int openRawSocket(char *device);

bool recebe_mensagem(int socket, mensagem_t *msg);

bool envia_mensagem(int socket, mensagem_t *msg);

void envia_confirmacao(int socket, int tipo);

#endif
