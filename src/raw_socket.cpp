#include "raw_socket.h"
#include "mensagem.h"

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

bool recebe_mensagem(int socket, mensagem_t *msg) {
    char *m = NULL;//(char *) malloc(TAM_MSG); //NULL;
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
    time_t ultimo_envio = time(NULL); // para sempre enviar na primeira vez
    int inicio = 0, n = 0;
    while(inicio < tam) {
        for (int i = 0; i < 3 && i < tam-inicio; ++i) {
            if(!enviada[i]) {
                m = msg_to_cstr(msg[(inicio+i)%TAM_SEQUENCIA], m);
                if(send(socket, m, TAM_MSG, 0) < 0) {
                    cerr << "[envia_mensagem] Erro ao enviar mensagem para o socket." << endl;
                    exit(-1);
                }
                enviada[i] = 1;
                ultimo_envio = time(NULL);
                printf("inicio: %d i: %d\n", inicio, i);
            }
        }

        recebe_mensagem(socket, resposta);

        if(time(NULL)-ultimo_envio > TIMEOUT) { 
            // se ja deu timeout
            enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;
            printf("TIMEOUT\n");
        }
        else if(resposta->tipo == ACK) {
            n = (resposta->dados)[0]-'0';
            if(n == inicio%TAM_SEQUENCIA) { 
                // janela desliza 1
                enviada[0] = enviada[1]; enviada[1] = enviada[2]; enviada[2] = 0;
            }
            else if(n == (inicio+1)%TAM_SEQUENCIA) { 
                // janela desliza 2
                enviada[0] = enviada[2]; enviada[1] = 0; enviada[2] = 0;
            }
            else { // janela desliza 3
                enviada[0] = 0; enviada[1] = 0; enviada[2] = 0;
            }
            inicio = n+1;
            printf("ACK %d\n", n);
        }
        else if(resposta->tipo == NACK) {
            n = (resposta->dados)[0]-'0';
            if(n == inicio%TAM_SEQUENCIA) { 
                // reenvia janela[0]
                enviada[0] = 0;
            }
            else if(n == (inicio+1)%TAM_SEQUENCIA) { 
                // reenviae janela[1] e janela desliza 1
                enviada[0] = enviada[1]; enviada[1] = enviada[2]; enviada[2] = 0;
            }
            else {
                // reenviae janela[2] e janela desliza 2
                enviada[0] = enviada[2]; enviada[1] = 0; enviada[2] = 0;
            }
            inicio = n;
            printf("NACK %d\n", n);
        }
    }

    //free(m);
    //free(r);
    //free(resposta);
}

void envia_confirmacao(int socket, int seq, int tipo) {
    mensagem_t *msg = monta_mensagem(tipo, 0, to_string(seq)); 
    char *m = NULL;
    aloca_str(&m, msg->tamanho+4);
    m = msg_to_cstr(msg, m);
    
    if(send(socket, m, TAM_MSG, 0) < 0) {
        cerr << "[enviaConfirmacao] Erro ao enviar mensagem para o socket." << endl;
        exit(-1);
    }

    printf("ACK %d enviado\n", seq);

    //free(m);
    //free(msg);
}