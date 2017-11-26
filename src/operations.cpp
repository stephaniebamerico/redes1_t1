
#include <operations.h>

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
*/


