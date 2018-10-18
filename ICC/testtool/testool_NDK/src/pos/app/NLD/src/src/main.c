#include <stdio.h>
#include "NDK.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "sys/types.h"
#include "fcntl.h"
#include "errno.h"
#include<sys/ioctl.h>
#include <string.h>
int main(int argc, char* argv[])
{
	int iret,nkey;
	char szBuff[256]={0};
	NDK_ScrInitGui();
	NDK_ScrClrs();
	if(argc>1)
	{
		for(iret=0;iret<argc;iret++)
		{
			strcat(szBuff,argv[iret]);
		}
		NDK_ScrPrintf("\n Recv param:%s\n",szBuff);
		NDK_SysDelay(5);
	}
	else
	{
		strcpy(szBuff,"\nRun success!\n");
		NDK_ScrPrintf(szBuff);
		NDK_SysDelay(5);
	}
	return 0;
}