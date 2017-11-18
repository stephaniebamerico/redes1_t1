#include "raw_socket.h"
#include "mensagem.h"
#include <iostream>

using namespace std;

void imprimeMenu();

int main(int argc, char const *argv[]) {
    char *buffer;
    int socket;

    if(!(buffer = (char *) malloc(sizeof(char)*TAM_MSG))) {
        cerr << "Erro ao alocar buffer." << endl;
        return 1;
    }

    if((socket = openRawSocket((char *) "enp7s0")) < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    string operacao;

    while(1) {
        imprimeMenu();
        cout << "Entre com a operacao desejada: ";
        getline(cin, operacao);

        if(operacao.size() > TAM_MSG) {
            cerr << "Mensagem excede o tamanho maximo." << endl;
            return 1;
        }
        printf("%s\n", operacao.c_str());
        strcpy(buffer, operacao.c_str());
        if(send(socket, buffer, TAM_MSG, 0) < 0) {
            cerr << "Erro ao enviar mensagem." << endl;
            return 1;
        }

        cout << endl << "Mensagem enviada com sucesso." << endl;
    }

    return 0;
}

void imprimeMenu() {
    printf("Operacoes disponiveis:\n");
    
    printf("* cd [endereco local]\n");
    printf("\tAltera diretorio corrente do cliente para [endereco local].\n\n");

    printf("* cdr [endereco remoto]\n");
    printf("\tAltera diretorio corrente do servidor para [endereco remoto].\n\n");    
    
    printf("* ls [opcao] [endereco local]\n");
    printf("\tLista conteudo do diretorio [enredeco local] do cliente (diretorio corrente por padrao).\n");
    printf("\t-a: nao ignora arquivos ocultos\n\t-l: usa formato de listagem longa \n\n");

    printf("* lsr [opcao] [endereco remoto]\n");
    printf("\tLista conteudo do diretorio [enredeco remoto] do servidor (diretorio corrente por padrao).\n");
    printf("\t-a: nao ignora arquivos ocultos\n\t-l: usa formato de listagem longa \n\n");

    printf("* get [endereco remoto]\n");
    printf("\tCopia o arquivo [endereco remoto] do servidor para o cliente.\n\n");

    printf("* put [endereco local]\n");
    printf("\tCopia o arquivo [endereco local] do cliente para o servidor.\n\n");
}