#include "raw_socket.h"
#include "operations.h"
#include <fcntl.h>

using namespace std;

void changeDir (char * dir)
{
    /*if (strlen(dir) == 0) {
        dir = (char *) malloc (2);
        dir[0] = ".";
        dir[1]='\0';

    }*/

    
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


void libera_vetor_de_mensagens (mensagem_t **msgs, int tam)
{
    int pos = tam/32;
    int resto = tam%32;
    for (int i = 0; i < pos; ++i)
    {
        
        free(msgs[i]->dados);
        free(msgs[i]);
    }
    if (resto > 0) 
    {
        free(msgs[pos]);
        free(msgs[pos]->dados);
        
    }
    free (msgs);

}


void copiaString (char * dest, char * src,int tam)
{
    for (int i = 0; i < tam; ++i)
        dest[i] = src[i];
}

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

void printMsgData(int tam, char * str)
{
    for (int i = 0; i < tam; ++i)
    {
        printf("%c",str[i] );
    }
}


void monta_msgs_com_arq (int socket, string name)
{
    setvbuf(stdout, NULL, _IOLBF,0);
    mensagem_t **mensagens = NULL;
    char* nome;
    aloca_str(&nome, name.size());
    strcpy (nome, name.c_str());

    /*int filedesc = open(nome, O_RDONLY);
    char *buffer;
    aloca_str(&buffer, 40);
    if(filedesc < 0)
    {
        printf ("Erro ao abrir o arquivo!\n");
        return;
    }*/
    struct stat fileStat;    
    stat(nome,&fileStat); 
    int tam = fileStat.st_size;
    int posicoes = tam/31;
    int resto = tam%31;
    string buf;
    char * buffer = aloca_str(posicoes*31);

    FILE *fp;
    fp = fopen (nome, "r");

    fread(buffer, 31, posicoes, fp);

    mensagens = (mensagem_t **) malloc((posicoes)*sizeof(mensagem_t**));
     /*for (int i = 0; i < posicoes; ++i)
    {
        aloca_mensagem(&mensagens[i]);
        aloca_str(&((mensagens[i]))->dados, 32);

    }
    if (resto > 0) 
    {

        aloca_mensagem(&mensagens[posicoes]);
        aloca_str(&((mensagens[posicoes]))->dados, resto);
    }*/

    int posZero = 0;
    for (int i = 0; i < posicoes; ++i)
    {
        read(filedesc,buffer,31);
        buffer[31]='\0';
        buf = (string)(buffer);
        mensagens[i] = monta_mensagem_2(31,GET, i%64, buffer);
    }
    if (resto)
    {
        read(filedesc,buffer,resto);
        buffer[resto] = '\0';
        buf= (string)(buffer);
        mensagens[posicoes] = monta_mensagem_2(resto,GET, posicoes%64, buffer);

    }
    for (int i = 0; i < posicoes; ++i)
    {
        printMsgData(mensagens[i]->tamanho,mensagens[i]->dados);
    }
    if (resto)
    {
        cout <<mensagens[posicoes]->dados;

    }
    cout << endl << endl;
    //libera_vetor_de_mensagens(mensagens, tam);

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