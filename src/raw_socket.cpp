#include "raw_socket.h"
#include "mensagem.h"
#include "operations.h"

struct pollfd ufds; // usado para timeout em recv

void inicia_socket(int socket) {
    ufds.fd = socket;
    ufds.events = POLLIN;
}

int openRawSocket(char *device) {
  int soquete;
  struct ifreq ir;
  struct sockaddr_ll endereco;
  struct packet_mreq mr;

  soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));      /*cria socket*/
  if (soquete == -1) {
    printf("Erro no Socket\n");
    exit(-1);
  }

  memset(&ir, 0, sizeof(struct ifreq));     /*dispositivo eth0*/
  int tmp = sizeof(device);
  memcpy(ir.ifr_name, device, tmp);
  if (ioctl(soquete, SIOCGIFINDEX, &ir) == -1) {
    printf("Erro no ioctl\n");
    exit(-1);
  }
    
  memset(&endereco, 0, sizeof(endereco));   /*IP do dispositivo*/
  endereco.sll_family = AF_PACKET;
  endereco.sll_protocol = htons(ETH_P_ALL);
  endereco.sll_ifindex = ir.ifr_ifindex;
  if (bind(soquete, (struct sockaddr *)&endereco, sizeof(endereco)) == -1) {
    printf("Erro no bind\n");
    exit(-1);
  }


  memset(&mr, 0, sizeof(mr));          /*Modo Promiscuo*/
  mr.mr_ifindex = ir.ifr_ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)    {
    printf("Erro ao fazer setsockopt\n");
    exit(-1);
  }

  return soquete;
}

int recebe_conteudo(int socket, mensagem_t ***msg) {
    mensagem_t *msg_tam = NULL;
    aloca_mensagem(&msg_tam);
    msg_tam->tipo = NACK;

    // Descobre o tamanho total que deve receber 
    while(msg_tam->tipo != TAMANHO && msg_tam->tipo != ERRO)
        recebe_mensagem(socket, msg_tam);

    // Envia ACK para resposta da requisicao
    envia_confirmacao(socket, msg_tam->sequencia, ACK);
        
    if(msg_tam->tipo == ERRO) {
        cout << "[recebe_conteudo] Erro ao receber mensagem com tamanho do conteúdo" << endl;
        return -1;
    }

    int tam = atoi(msg_tam->dados);

    // Aloca espaço para o conteúdo
    *msg = (mensagem_t **) malloc(sizeof(mensagem_t *)*tam);
    mensagem_t *mensagem_recebida = NULL;
    aloca_mensagem(&mensagem_recebida);
    bool recebida[3];

    recebida[0] = 0; recebida[1] = 0; recebida[2] = 0;

    // Começa a receber todo conteúdo
    time_t ultimo_envio = time(NULL);
    int inicio = 0, seq, i;
    while(inicio < tam) {
        if(recebe_mensagem(socket, mensagem_recebida)
            && (mensagem_recebida->tipo == IMPRIMA
                || mensagem_recebida->tipo == FIM)) {
            seq = mensagem_recebida->sequencia;
            if(seq == inicio%TAM_SEQUENCIA || 
                seq == (inicio+1)%TAM_SEQUENCIA || 
                seq == (inicio+2)%TAM_SEQUENCIA) {
                // recebeu mensagem dentro da janela esperada
                for (i = 0; i <= 2 && seq != (inicio+i)%TAM_SEQUENCIA; ++i);

                //conferir paridade e enviar NACK
                if(calcula_paridade(*mensagem_recebida) == mensagem_recebida->paridade) {                   
                    recebida[i] = 1;

                    copia_mensagem(mensagem_recebida, &((*msg)[inicio+i]));
                    
                    if(recebida[0] && recebida[1] && recebida[2]) {
                        // recebeu a janela toda
                        envia_confirmacao(socket, (inicio+2)%TAM_SEQUENCIA, ACK);
                        
                        // janela desliza 3
                        inicio += 3;
                        recebida[0] = 0; recebida[1] = 0; recebida[2] = 0;
                    }
                }
                else {
                    envia_confirmacao(socket, seq, NACK);
                }

                ultimo_envio = time(NULL);
            }
            else {
                // timeout para ACK: reseta janela
                for (i = 0; inicio-i > 0 && (inicio-i)%TAM_SEQUENCIA != seq; ++i);
                inicio = inicio-i;
                recebida[0] = 0; recebida[1] = 0; recebida[2] = 0;
            }
        }
        else if(2*(time(NULL)-ultimo_envio) > TIMEOUT) {
            // timeout para receber a janela
            for (i = 0; i <= 2 && recebida[i]; ++i);
            if(i > 0) {
                --i;
                envia_confirmacao(socket, (inicio+i)%TAM_SEQUENCIA, ACK);
                if(i == 0) {
                    // janela desliza 1
                    recebida[0] = recebida[1]; recebida[1] = recebida[2]; recebida[2] = 0;
                }
                else {
                    // janela desliza 2
                    recebida[0] = recebida[2]; recebida[1] = 0; recebida[2] = 0;   
                }

                inicio += i+1;
            }
        }
    }

    // Recebe mensagem FIM
    mensagem_t *msg_fim = NULL;
    aloca_mensagem(&msg_fim);
    msg_fim->tipo = NACK;
    while(msg_fim->tipo != FIM)
        recebe_mensagem(socket, msg_fim);
    // Envia ACK para resposta da requisicao
    envia_confirmacao(socket, msg_fim->sequencia, ACK);

    return tam;
}

bool recebe_mensagem(int socket, mensagem_t *msg) {
    char *m = NULL;
    aloca_str(&m, TAM_MSG);
    m[0] = 0;

    // Confere se tem algo no socket para ser lido
    if(poll(&ufds, 1, WAIT) > 0) {
        if(recv(socket, m, TAM_MSG, 0) < 0) {
            cerr << "Erro ao receber mensagem do socket." << endl;
            exit(-1);
        }
        // Confere se chegou uma mensagem ou se é só lixo
        if(m[0] == 0x007E) {
            msg = cstr_to_msg(m, msg);
            //free(m);
            return true;
        }
    }

    //free(m);
    return false;
}

void envia_mensagem(int socket, mensagem_t **msg, int tam) {
    char *r = NULL, *m = NULL;
    mensagem_t *resposta = NULL;
    bool enviada[3];

    aloca_str(&r, TAM_MSG);
    aloca_str(&m, (*msg)->tamanho+4); 
    aloca_mensagem(&resposta);

    resposta->tipo = FIM; // só para não dar problema até ler a primeira resposta
    enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;

    // Tenta enviar mensagem
    time_t ultimo_envio = time(NULL);
    int inicio = 0, n = 0;
    while(inicio < tam) {
        for (int i = 0; i < 3 && i < tam-inicio; ++i) {
            if(!enviada[i]) {
                m = msg_to_cstr(msg[inicio+i], m);
                if(send(socket, m, TAM_MSG, 0) < 0) {
                    cerr << "[envia_mensagem] Erro ao enviar mensagem para o socket." << endl;
                    exit(-1);
                }

                enviada[i] = 1;
                ultimo_envio = time(NULL);
            }
        }

        recebe_mensagem(socket, resposta);

        if(time(NULL)-ultimo_envio > TIMEOUT) { 
            // se ja deu timeout
            enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;
            printf("TIMEOUT\n");
        }
        else if(resposta->tipo == ACK) {
            n = atoi(resposta->dados);
            if(n == inicio%TAM_SEQUENCIA) { 
                // janela desliza 1
                enviada[0] = enviada[1]; enviada[1] = enviada[2]; enviada[2] = 0;
                inicio += 1;
            }
            else if(n == (inicio+1)%TAM_SEQUENCIA) { 
                // janela desliza 2
                enviada[0] = enviada[2]; enviada[1] = 0; enviada[2] = 0;
                inicio += 2;
            }
            else if(n == (inicio+2)%TAM_SEQUENCIA) { 
                // janela desliza 3
                enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;
                inicio += 3;
            }
            resposta->tipo = TRATADO;
            printf("Recebeu ACK %d\n", n);
        }
        else if(resposta->tipo == NACK) {
            n = atoi(resposta->dados);
            if(n == inicio%TAM_SEQUENCIA) { 
                // reenvia janela[0]
                enviada[0] = 0;
                
                printf("Recebeu NACK %d\n", n);
            }
            else if(n == (inicio+1)%TAM_SEQUENCIA) { 
                // reenviae janela[1] e janela desliza 1
                enviada[0] = 0; enviada[1] = enviada[2]; enviada[2] = 0;
                inicio += 1;

                printf("Recebeu NACK %d\n", n);
            }
            else if(n == (inicio+2)%TAM_SEQUENCIA) {
                // reenviae janela[2] e janela desliza 2
                enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;
                inicio += 2;

                printf("Recebeu NACK %d\n", n);
            }
            resposta->tipo = TRATADO;
        }
    }
}

void envia_confirmacao(int socket, int seq, int tipo) {
    char *c_seq = NULL;
    string s_seq = to_string(seq);
    aloca_str(&c_seq, s_seq.size());
    strcpy(c_seq, s_seq.c_str());
    mensagem_t *msg = monta_mensagem(tipo, 0, c_seq); 
    
    char *m = NULL;
    aloca_str(&m, msg->tamanho+4);
    m = msg_to_cstr(msg, m);

    imprime_mensagem(*msg);
    int t = send(socket, m, TAM_MSG, 0);
    if(t < 0) {
        cerr << "[enviaConfirmacao] Erro ao enviar mensagem para o socket." << endl;
        exit(-1);
    }
    string op = (tipo == ACK) ? "ACK" : "NACK";
    cout << op << " " << seq << endl;
    //printf("Confirmação do tipo %d para sequencia %d enviada\n", tipo, seq);
}