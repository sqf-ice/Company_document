#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>

#define COPYMODE     0644

int ndk_file_touch(char *filename)
{
    char error[1024] = {0};
    int fd;
    if(filename!=NULL) {
        fd=creat(filename,COPYMODE);
        if(-1 == fd) {
            if(errno != EISDIR) {
                sprintf(error,"create file : %s error",filename);
                perror(error);
                return -1;
            }
        } else {
            close(fd);
        }
    }
    return 0;
}

