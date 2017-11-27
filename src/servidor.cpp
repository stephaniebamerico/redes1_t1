#include "raw_socket.h"
#include "mensagem.h"
#include "operations.h"

int main(int argc, char const *argv[]) {
    int socket = openRawSocket((char *) argv[1]);

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    inicia_socket(socket);
    int *lsArgs = (int*)malloc (3*sizeof(int));
    cout << "Aguardando mensagens." << endl;
    mensagem_t *msg_recebida = NULL;
    aloca_mensagem(&msg_recebida);
    while (1) {        
        if(recebe_mensagem(socket, msg_recebida)) {
            //DEBUG
            cout << endl << "Mensagem recebida: " << endl;
            imprime_mensagem(*msg_recebida);

            envia_confirmacao(socket, msg_recebida->sequencia, ACK);
/*==================================================================================================*/
            if (msg_recebida->tipo == CD) {
                errno = 0;
                chdir (msg_recebida->dados);
                system("pwd");
                mensagem_t *msg_resposta;
                if (errno == 0)
                    msg_resposta = monta_mensagem(OK, 0, "");
                else {
                    if (errno == EACCES)
                        msg_resposta = monta_mensagem(ERRO, 0, "2");
                    else 
                        msg_resposta = monta_mensagem(ERRO, 0, "1");
                }
                 envia_mensagem(socket, &msg_resposta, 1);

                //DEBUG
                cout << endl << "Mensagem enviada: " << endl;
                imprime_mensagem(*msg_resposta);
                
                //libera_mensagem(msg_ok);
                //libera_mensagem(msg_recebida);
                aloca_mensagem(&msg_recebida);
            }
/*==================================================================================================*/
            else if (msg_recebida->tipo == LS) {
                errno = 0;

                lsArgs = testOptions(msg_recebida->dados);
                
                mensagem_t *msg_resposta;
                if (errno == 0)
                {
                    string lsSaida = list(lsArgs[1],lsArgs[0]);
                    if (errno == 0)
                    {
                        char * ls;
                        aloca_str (&ls, lsSaida.size());
                        strcpy(ls, lsSaida.c_str());
                        char_to_msg(socket, ls,lsSaida.size() );
                    }
                    else if (errno == EACCES)
                    {
                        msg_resposta = monta_mensagem(ERRO, 0, "2");
                        envia_mensagem(socket, &msg_resposta, 1);
                    }
                    else 
                    {
                        msg_resposta = monta_mensagem(ERRO, 0, "1");
                        envia_mensagem(socket, &msg_resposta, 1);
                    }
                    
                }
                else
                {
                    msg_resposta = monta_mensagem(ERRO, 0, "1");
                        envia_mensagem(socket, &msg_resposta, 1);
                }

            }
            else {
                if(msg_recebida->tipo == 20) {
                    mensagem_t *conteudo;
                    recebe_conteudo(socket, &conteudo);
                }
                /*mensagem_t *msg_ok = monta_mensagem(OK, 0, "");
                
                envia_mensagem(socket, &msg_ok, 1);

                //DEBUG
                cout << endl << "Mensagem enviada: " << endl;
                imprime_mensagem(*msg_ok);
                
                //libera_mensagem(msg_ok);
                //libera_mensagem(msg_recebida);
                aloca_mensagem(&msg_recebida);*/
            }
        }
    }

    return 0;
}