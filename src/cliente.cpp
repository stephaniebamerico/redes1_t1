#include "raw_socket.h"
#include "mensagem.h"

void imprimeMenu();

int main(int argc, char const *argv[]) {
    int socket = openRawSocket((char *) argv[1]);

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    inicia_socket(socket);

    /* Socket aberto com sucesso, iniciar cliente: */

    string operacao, comando, args;
    int comando_pfim;
    
    imprimeMenu();

    while(1) {
        cout << endl << "Entre com a operacao desejada: ";
        getline(cin, operacao);

        /* Descobre a posicao em que acaba a substring com o tipo de operacao (comando) */
        comando_pfim = operacao.find_first_of(" ");

        if(comando_pfim < 0)
            comando_pfim = operacao.size();
        
        /* Descobre o comando e os argumentos */
        comando = operacao.substr(0, comando_pfim);
        args = (comando_pfim+1 < (int) operacao.size()) ? operacao.substr(comando_pfim+1, operacao.size()) : "";
        
        /* Executa a operacao, se listada */
        if(comando == "cd") {
            cout << "Operacao cd com argumentos: " << args << endl;
        }
        else if(comando == "cdr") {
            cout << "comecando crd" << endl;
            if(args.size() > 0 && args.size() < 248) {
                cd_remoto(socket, args);
            }
        }
        else if(comando == "ls") {
            cout << "Operacao ls com argumentos: " << args << endl;
        }
        else if(comando == "lsr") {
            cout << "Operacao lsr com argumentos: " << args << endl;
        }
        else if(comando == "get") {
            cout << "Operacao get com argumentos: " << args << endl;
        }
        else if(comando == "put") {
            cout << "Operacao put com argumentos: " << args << endl;
        }
        else {
            cout << "Operacao invalida" << endl;
        }
    }

    return 0;
}

void imprimeMenu() {
    cout << "Operacoes disponiveis:" << endl;
    
    cout << "* cd [endereco local]" << endl;
    cout << "\tAltera diretorio corrente do cliente para [endereco local]." << endl << endl;

    cout << "* cdr [endereco remoto]" << endl;
    cout << "\tAltera diretorio corrente do servidor para [endereco remoto]." << endl << endl;
    
    cout << "* ls [opcao] [endereco local]" << endl;
    cout << "\tLista conteudo do diretorio [enredeco local] do cliente (diretorio corrente por padrao)." << endl;
    cout << "\t-a: nao ignora arquivos ocultos" << endl;
    cout << "\t-l: usa formato de listagem longa" << endl << endl;

    cout << "* lsr [opcao] [endereco remoto]" << endl;
    cout << "\tLista conteudo do diretorio [enredeco remoto] do servidor (diretorio corrente por padrao)." << endl;
    cout << "\t-a: nao ignora arquivos ocultos" << endl;
    cout << "\t-l: usa formato de listagem longa" << endl << endl;

    cout << "* get [endereco remoto]" << endl;
    cout << "\tCopia o arquivo [endereco remoto] do servidor para o cliente." << endl << endl;

    cout << "* put [endereco local]" << endl;
    cout << "\tCopia o arquivo [endereco local] do cliente para o servidor." << endl << endl;
}