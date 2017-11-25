#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <iostream>
#include <string>

#include "mensagem.h"

#define 0 ACK
#define 2 TAMANHO
#define 3 OK
#define 6 CD
#define 7 LS
#define 8 GET
#define 9 PUT
#define A FIM
#define C MOSTRANATELA
#define E ERRO
#define F NACK


void operations (mensagem_t msg){
    if (msg.tipo == CD)
        changeDir(mensagem.dados);
    else if (msg.tipo == LS)
        list(mensagem.dados);

    
}

int changeDir (char *dir)
{
    int err = chdir (dir);
    if (err)
    {
        switch (errno) {
            case EACCES:
                return 2;

            case ENOENT:
                return 1;
                break;

            case ENOTDIR:
                return 1;

            default: 
                return 3;
        }
    }
        return 0;
}

int* testOptions (char *options)
{
     //opt 1: -l, op2: la
    int* opt = (int*) malloc (sizeof(int)*2);
    opt[0]=0;
    opt[1]=0;
    char spaca[2] = " ";
    char *token;
    /* get the first token */
    token = strtok(str, s);

    /* walk through other tokens */
    while( token != NULL ) {
        if (strcmp (token, "-l") == 0)
            opt[0]=1;
        else if (strcmp (token, "-a") == 0)
            opt[1]=1;
        else { 
            errno = EINVAL;
            return NULL;
        }


        token = strtok(NULL, s);
    }
}

string list(int a, int l)
{
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
        if (dir=opendir(".")!= NULL){
            stat(".",&fileStat); 
            while((myfile=readdir(dir))!=NULL)
            {
                if (a || (myfile->d_name[0] != '.')) {
                    stat(myfile->d_name,&fileStat);  
                    list = list + ( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    list = list + ( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    list = list +( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    list = list +( (fileStat.st_mode & S_IROTH) ? "r" : "-");
                    list = list +( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    list = list +( (fileStat.st_mode & S_IXOTH) ? "x" : "-"); 
                    list = list +(" ");

                    list = list + to_string( fileStat.st_nlink);
                    list = list + " ";

                    pw=getpwuid(fileStat.st_uid);
                    list = list +(pw->pw_name);
                    list = list + " ";
                    gp=getgrgid(fileStat.st_gid);
                    list = list +(gp->gr_name);
                    list = list + " ";

                    list = list + to_string(fileStat.st_size);
                    list = list + " ";

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
                if (a || ent->d_name[0] != '.')
                {
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


