#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <error.h>
#include <errno.h>
#include <utime.h>


#define CMD_BACKUP            0x00000001
#define CMD_RECUR             0x00000002

void do_cp(char *source,char *des,int flags);
char *getname(char *path);
int ndk_file_cp(char *sourceFile,char *desFile)
{
	int flags = 0,files = 0;
    int fd;
	char *filearray[1024];
	if(sourceFile!=NULL)
		filearray[files++] =(char *)sourceFile;
	if(desFile!=NULL)
		filearray[files] =(char *)desFile;

	char error[1024];
	if(files==1){
        fd=creat(filearray[files],S_IRWXU);
		if(-1 == fd)
		{
			if(errno != EISDIR)
			{
				sprintf(error,"create file : %s error",filearray[files]);
				perror(error);
				return -1;
			}
		}
		else	{	
                close(fd);
			do_cp(filearray[0],filearray[1],flags);	
          }
	}
	return 0;
}

char *getname(char *path)
{
	char *name=++path;
	while(*path)
	{
		if(*(path++) == '/')
			name = path;
	}
	return name;
}

void do_cp(char *source ,char *des,int flags)
{
	struct stat st;
	char error[1024];
	if(-1 == stat(source,&st))
	{
		sprintf(error,"file stat : %s error ",source);
		perror(error);
		return ;
	}
	if(S_ISDIR(st.st_mode))
	{
		if(!(flags & CMD_RECUR))
		{
			printf("file %s is a directory file\n",source);
			return ;
		}
		
		if(-1 == mkdir(des,S_IRWXU))
		{
			sprintf(error,"create dir %s error",des);
			perror(error);
			return ;
		}
		DIR *dir;
		struct dirent *direntp;
		int des_size = strlen(des);
		int source_size = strlen(source);

		if((dir = opendir(source))==NULL)
		{
			sprintf(error,"open dir %s error : ",source);
			perror(error);
			return;
		}
		
		des[des_size++] = '/';
		source[source_size++] = '/';

		while((direntp = readdir(dir))!=NULL)
		{
			if((!strcmp( direntp->d_name,"."))||(!strcmp(direntp->d_name,"..")))
				continue;
			printf("file %s \n",direntp->d_name);
			strcpy(des + des_size,direntp->d_name);
			strcpy(source+ source_size,direntp->d_name);
			do_cp(source,des,flags);
		}
		des[--des_size] = 0;
		source[--source_size] = 0;
	}
	else
	{
		int fdsource,fddes;
		int readbytes;
		if((fdsource = open(source,O_RDONLY)) == -1)
		{
			sprintf(error,"open file : %s error ",source);
			perror(error);
			return;
		}
		if((fddes = creat(des,O_CREAT|O_WRONLY|O_TRUNC))== -1)
		{
			sprintf(error,"open file : %s error",des);
			perror(error);
			close(fdsource);
			return;
		}
		char buf[1024];
		int size = 1024;
		while(((readbytes = read(fdsource,buf,size)) != -1) && readbytes)
		{
			if(-1 ==  write(fddes,buf,readbytes) )
			{
				perror("error");
				break;
			}
		}
		close(fdsource);
		close(fddes);
	}
	chmod(des,st.st_mode);
	if(flags & CMD_BACKUP)
	{
		struct utimbuf time_buf;
		time_buf.actime = st.st_atime;
		time_buf.modtime = st.st_mtime;
		if(-1 == utime(des,&time_buf))
			perror("set time error");
	}
}
