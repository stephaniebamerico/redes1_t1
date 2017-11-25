#ifndef __MENSAGEM__
#define __MENSAGEM__

#define TAM_MSG 64

#include <stdlib.h>
#include <iostream>
#include <stdio.h>

using namespace std;

typedef struct {
    char inicio;
    int tamanho : 5;
    int sequencia : 6;
    int tipo : 5;
    char *dados;
    char paridade;
} mensagem_t;

void imprimeMensagem(mensagem_t msg);
char* msg_to_cstr(mensagem_t msg);
mensagem_t* cstr_to_msg(char *cstr);

#endif