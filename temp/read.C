#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

string list(int a, int l)
{
    string list ;
    if (l)
    {
        struct passwd *pw;
        struct group *gp;
        DIR *mydir;
        char *c;
        int i;
        struct dirent *myfile;
        struct stat fileStat;
        mydir=opendir(".");
        stat(".",&fileStat); 
        while((myfile=readdir(mydir))!=NULL)
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
        closedir(mydir);  
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
            perror ("");
        }
    }
    return list;
}

int main(int argc, char const *argv[])
{
    int a,b;
    cin >> a;
    cin >> b;
    string aaaa = list (a,b);
    cout <<aaaa;
    return 0;
}