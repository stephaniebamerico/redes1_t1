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

    cout << "Aguardando mensagens." << endl;
    mensagem_t *msg_recebida = NULL;
    aloca_mensagem(&msg_recebida);
    while (1) {        
        if(recebe_mensagem(socket, msg_recebida)) {

            //DEBUG
            cout << endl << "Mensagem recebida: " << endl;
            imprime_mensagem(*msg_recebida);




            envia_confirmacao(socket, ACK);
            if (msg_recebida->tipo == CD)
            {
                chdir (msg_recebida->dados);
                system("pwd");
                mensagem_t *msg_resposta;
                if (errno == 0)
                    msg_resposta = monta_mensagem(OK, 0, "");
                else
                {
                    if (errno == EACCES)
                        msg_resposta = monta_mensagem(ERRO, 0, "2");
                    else 
                        msg_resposta = monta_mensagem(ERRO, 0, "1");
                }
                 envia_mensagem(socket, msg_resposta);

                //DEBUG
                cout << endl << "Mensagem enviada: " << endl;
                imprime_mensagem(*msg_resposta);
                
                //libera_mensagem(msg_ok);
                //libera_mensagem(msg_recebida);
                aloca_mensagem(&msg_recebida);
            }

            else
            {
                mensagem_t *msg_ok = monta_mensagem(OK, 0, "");
                
                envia_mensagem(socket, msg_ok);

                //DEBUG
                cout << endl << "Mensagem enviada: " << endl;
                imprime_mensagem(*msg_ok);
                
                //libera_mensagem(msg_ok);
                //libera_mensagem(msg_recebida);
                aloca_mensagem(&msg_recebida);
            }
        }
    }

    return 0;
}