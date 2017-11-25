#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <dirent.h>

int * blah ()
{
    int* a =(int*) malloc (sizeof(int)*2);
    a[0]=10;
    a[1]=-1;
    return NULL;
}

using namespace std;
int main(int argc, char const *argv[])
{
    system ("ls > ls.txt");

    char *s = (char*)"..";
    chdir (s);
    system("ls");
    char *s2="include";
    chdir (s2);
    system("ls > ls.txt 2> error.txt");
    string str = "mundo";
    str = str + s;


    struct stat st;
    stat("error.txt", &st);
    int size = st.st_size;
    printf("%d\n",size );
    remove ("error.txt");
    remove ("ls.txt");
    int *a = blah();
    //printf("%d\n",a[0] );
    return 0;
}