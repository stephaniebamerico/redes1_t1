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
    char *m = (char *) malloc (sizeof(char) * TAM_MSG);
    m[0] = 0;

    if(poll(&ufds, 1, 1000) > 0) {
        if(recv(socket, m, TAM_MSG, 0) < 0) {
            cerr << "Erro ao receber mensagem do socket." << endl;
            exit(-1);
        }

        if(m[0] == 0x007E) {
            msg = cstr_to_msg(m, msg);
            imprime_mensagem(*msg);

            free(m);
            return true;
        }
    }

    free(m);
    return false;
}

bool envia_mensagem(int socket, mensagem_t *msg) {
    char *r = (char *) malloc (sizeof(char) * TAM_MSG);
    char *m = (char *) malloc (sizeof(char) * (msg->tamanho + 4)); 
    mensagem_t *resposta = (mensagem_t *) malloc(sizeof(mensagem_t));
    if(!m || !r || !resposta) {
        cerr << "[envia_mensagem] Erro ao alocar variavel." << endl;
        return false;
    }

    m = msg_to_cstr(msg, m);
    resposta->tipo = NACK;

    // Tenta enviar mensagem
    int tentativas=0;
    while(resposta->tipo != ACK && tentativas < 2) {
        if(send(socket, m, TAM_MSG, 0) < 0) {
            cerr << "[envia_mensagem] Erro ao enviar mensagem para o socket." << endl;
            exit(-1);
        }

        cout << "Tentou enviar cd " << tentativas++ << " vezes" << endl;

        recebe_mensagem(socket, resposta);
    }

    free(m);
    free(r);
    free(resposta);
    
    // ACK
    return true;
}

void envia_confirmacao(int socket, int tipo) {
    mensagem_t *msg = monta_mensagem(tipo, 0, ""); 
    char *m = (char *) malloc (sizeof(char) * (msg->tamanho + 4));
    m = msg_to_cstr(msg, m);
    
    if(send(socket, m, TAM_MSG, 0) < 0) {
        cerr << "[enviaConfirmacao] Erro ao enviar mensagem para o socket." << endl;
        exit(-1);
    }

    cout << "Enviou ACK" << endl;

    free(m);
    free(msg);
}