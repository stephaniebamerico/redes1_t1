#ifndef __OPERATIONS__
#define __OPERATIONS__


#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "mensagem.h"

void changeDir (char * dir);

int* testOptions (char * options);

string list(int a, int l);

void arq_to_msg (int socket, string name);

void char_to_msg (int socket, char* buffer, int tam);

void msg_to_arq (mensagem_t **mensagens, string name, int tam);


#endif

/*
get (nome: maximo de 31 caracteres)
- nome do arquivo dentro da area de dados
- Primeiro manda o tamanho do arquivo em bytes
- Erro:
- Sem espaço em disco
Ao terminar de mandar envia A
Responde A mandando nack ou ack
Sobrescreve arquivos com mesmo nome
*/

