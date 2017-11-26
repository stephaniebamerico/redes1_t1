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
#include <fcntl.h>


using namespace std;


int main()
{
    struct stat fileStat;    
    stat("testfile.txt",&fileStat); 
    int filedesc = open("testfile.txt", O_RDONLY);
    char  buffer[32];
    string buf;
    if(filedesc < 0)
    {
        printf("sadasd\n");
        return 1;
    }
 
    (read(filedesc,buffer,30)); 
    buf = buffer;
    cout << buf;
    (read(filedesc,buffer,31)); 
    buf = buffer;
    cout << buf;
    return 0;
}