#include "raw_socket.h"
#include "operations.h"
#include <fcntl.h>

using namespace std;

void changeDir (char * dir)
{
       
    struct stat fileStat;
    stat(dir,&fileStat);
    errno = 0;
    
    //muda para o diretório dir
    if ((fileStat.st_mode & S_IRGRP) ) chdir (dir);
    else errno = EACCES;
  
}

int* testOptions (char * options)
{
    //opt 1: -l, op2: -a
    int* opt = (int*) malloc (sizeof(int)*2);
    opt[0]=0;
    opt[1]=0;
    char space[2] = " ";
    char *token;
    /* get the first token */
    token = strtok(options, space);
    //código de erro é 0
    errno = 0;

    /* walk through other tokens */
    while( token != NULL ) {
        //se é opção -l, opt[0] é 1
        if (strcmp (token, "-l") == 0)
            opt[0]=1;
        //se é opção -a, opt[0] é 1
        else if (strcmp (token, "-a") == 0)
            opt[1]=1;
        else { 
            //argumento inválido, altera variável errno e renorta com ponteiro nulo
            errno = EINVAL;
            return NULL;
        }

        //corta próxima parte da string 
        token = strtok(NULL, space);
    }
    //retorna opt (com 0,0 se não tem argumentos ou 1 na posição do argumento que tiver)
    return opt;
}

string list(int a, int l)
{
    //código de erro é 0
    errno = 0;
    string list ;
    if (l)
    {
        struct passwd *pw;
        struct group *gp;
        DIR *dir;
        char *c;
        int i;
        struct dirent *myfile;
        struct stat fileStat;
        if ((dir=opendir(".")) != NULL){
            stat(".",&fileStat); 
            while((myfile=readdir(dir))!=NULL)
            {
                //se tem opção -a, imprime tudo. se não, só imprime os que não tem "." antes do nome
                if (a || (myfile->d_name[0] != '.')) {
                    //pega informações sobe o arquivo
                    stat(myfile->d_name,&fileStat);  
                    //confere de é diretório
                    list = list +( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    //confere rwx usuário, grupo e outros
                    list = list +( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    list = list +( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    list = list +( (fileStat.st_mode & S_IROTH) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXOTH) ? "x" : "-"); 
                    list = list +(" ");
                    //quantidade de links para o arquivo/ diretório
                   
                    list = list + to_string( fileStat.st_nlink);
                    list = list + " ";

                    pw=getpwuid(fileStat.st_uid);
                    list = list +(pw->pw_name);
                    list = list + " ";

                    gp=getgrgid(fileStat.st_gid);
                    list = list +(gp->gr_name);
                    list = list + " ";
                    //tamanho do arquivo
                    list = list + to_string(fileStat.st_size);
                    list = list + " ";
                    //data da criação do arquivo
                    c=ctime(&fileStat.st_mtime);
                    for(i=4;i<=15;i++)
                    list = list + (c[i]);
                    list = list + " ";
                    list = list +(myfile->d_name);
                    list = list + '\n';
                }
            }
            closedir(dir);  
        }
        else
            errno = EACCES;
    }
    else
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (".")) != NULL) {
        /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                //se tem opção -a, imprime tudo. se não, só imprime os que não tem "." antes do nome
                if (a || ent->d_name[0] != '.')
                {
                    //nome do arquivo
                    list = list + (ent->d_name);
                    list = list + " ";
                }
            }
            list = list + '\n';
            closedir (dir);
        } else {
        /* could not open directory */
            errno = EACCES;
            perror ("");
        }
    }
    return list;
}


/*
get (nome: maximo de 31 caracteres)
- nome do arquivo dentro da area de dados
- Primeiro manda o tamanho do arquivo em bytes
- Erro:
- Sem espaço em disco
Ao terminar de mandar envia A
Responde A mandando nack ou ack
Sobrescreve arquivos com mesmo nome
Copia o arquivo [endereco remoto] do servidor para o cliente.

*/


//  tam/32



void copiaString (char * dest, char * src,int tam)
{
    for (int i = 0; i < tam; ++i)
        dest[i] = src[i];
}

//monta uma mensagem passando tam como parametro, e args é um char*
mensagem_t* monta_mensagem_2(int tam, int tipo, int sequencia, char* args) {
    mensagem_t *msg = NULL;
    aloca_mensagem(&msg);

    msg->inicio = 0x007E;
    msg->tamanho = tam;
    msg->sequencia = sequencia;
    msg->tipo = tipo;
    if(msg->tamanho > 0) {
        aloca_str(&(msg->dados), msg->tamanho);
        copiaString(msg->dados, args,tam);
    }
    msg->paridade = calcula_paridade(*msg);

    return msg;
}

void arq_to_msg(int socket, string name)
{
    //nome do arquivo a ser aberto
    char* nome;
    aloca_str(&nome, name.size());
    //passa a string name para um vetor de chars
    strcpy (nome, name.c_str());

    struct stat fileStat;    
    stat(nome,&fileStat); 
    //descobre tam do arquivo em bytes
    int tam = fileStat.st_size;

    //quantas posições o vetor de mensagens precisa
    int posicoes = tam/31;
    //se tiver resto, precisa de uma posição a mais
    int resto = tam%31;
    char * buffer;
    aloca_str(&buffer, posicoes*31+resto+1);
    char* bufferResto;
    if (resto)
        aloca_str(&bufferResto, resto+2);

    FILE *fp;
    fp = fopen (nome, "r");
    if (!fp)
    {
        printf("erro!\n");
        return;
    }
    //lê os dados
    fread(buffer, 31, posicoes, fp);
    
    fread(bufferResto, resto, 1, fp);
    fclose(fp);
    copiaString(buffer+31*posicoes,bufferResto, resto+1);
    char_to_msg(socket, buffer, tam);
}



void char_to_msg (int socket, char* buffer, int tam)
{
    //quantas posições o vetor de mensagens precisa
    int posicoes = tam/31;
    //se tiver resto, precisa de uma posição a mais
    int resto = tam%31;
    //vetor de mensagens
    mensagem_t **mensagens = NULL;

    mensagens = (mensagem_t **) malloc((posicoes + (resto > 0 ? 1 : 0))*sizeof(mensagem_t**));

    char *aux;
    aloca_str(&aux, 32);
    for (int i = 0; i < posicoes; ++i)
    {
        copiaString (aux, buffer+i*31,31);
        mensagens[i] = monta_mensagem_2(31,IMPRIMA, i%64, aux);
    }
    if (resto)
    {
        //copiaString (aux, bufferResto, resto);
        mensagens[posicoes] = monta_mensagem_2(resto+1,IMPRIMA, posicoes%64, buffer+31*posicoes);

    }
    
    mensagem_t *msg;
    msg = monta_mensagem(TAMANHO, 0, to_string(posicoes + (resto > 0 ? 1 : 0)) );
    cout << "Mensagem TAM:" << endl;
    imprime_mensagem(*msg);
    envia_mensagem(socket, &(msg), 1);
    
    envia_mensagem(socket, mensagens, posicoes + (resto > 0 ? 1 : 0));



}

void msg_to_arq (mensagem_t **mensagens, string name, int tam)
{
    int posicoes = tam/31;
    int resto = tam%31;
    char* saida;
    aloca_str(&saida, posicoes*31+resto);

    for (int i = 0; i < posicoes; ++i)
    {
        copiaString(saida+i*31, mensagens[i]->dados, mensagens[i]->tamanho);

    }
    if (resto)
    {
        copiaString(saida+posicoes*31,mensagens[posicoes]->dados,mensagens[posicoes]->tamanho);

    }
    FILE *fp;
    char* fileName;
    aloca_str(&fileName, name.size());
    strcpy(fileName, name.c_str());
    fp = fopen(fileName, "w");
    if (!fp)
    {
        printf("erro!\n");
        return;
    }

    fwrite(saida, 1, 31*(posicoes)+ resto,fp);
    fclose(fp);

    cout << endl << endl;
}


/*




 put (nome do arquivo)
- nome do arquivo no pedido
- Erros:
- Sem permissão
- Se receber ok manda o descritor (2) com
- tamanho do arquivo
- Erros:
- Espaço insuficiente
Ao terminar manda A, resposta ack ou nack

*/