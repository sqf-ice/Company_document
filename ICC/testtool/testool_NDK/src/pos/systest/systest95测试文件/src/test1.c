/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: test1.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 取硬件信息/取电量线程交叉
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "NDK.h"
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include <sys/syscall.h>

pid_t gettid(void)   //获取线程id
{
	return syscall(SYS_gettid);
}

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
int thread_run_flag1 = 0;
static pthread_mutex_t mutex;
static int pthread_retval = 0;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  						sull        20180411   created
*****************************************************************/
static void *pthread_info(void *arg)
{
	int  ret = -1;
	char sBuf[128]={0};
	uint unLen=0;

	//thread_run_flag 置1时 线程退出
	fprintf(stderr,"%s,%d devmgr devpid: %d\n",__FUNCTION__,__LINE__,gettid());
	while(thread_run_flag1 == 0)
	{	
		//为了确认确实有线程在跑
		//fprintf(stderr,"line:[%d]thread_info is runing\n",__LINE__);
		memset(sBuf,0,sizeof(sBuf));
		if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BIOS_VER, &unLen, sBuf)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]测试失败\n",__LINE__);
			goto ERR1;
		}
		NDK_SysMsDelay(10);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag1 = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

static void *pthread_vol(void *arg)
{
	int  ret = -1;
	uint unVol = 0;

	//thread_run_flag 置1时 线程退出
	fprintf(stderr,"%s,%d libpms volpid: %d\n",__FUNCTION__,__LINE__,gettid());
	while(thread_run_flag1 == 0)
	{	
		//为了确认确实有线程在跑
		//fprintf(stderr,"line:[%d]thread_vol is runing\n",__LINE__);
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK)
		{
			fprintf(stderr,"line:[%d]测试失败\n",__LINE__);
			goto ERR1;
		}
		NDK_SysMsDelay(10);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag1 = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

void main(void)
{	
	/*private & local definition*/
	pthread_t tidp,tidp1;
    int error=-1, error1=-1;
    int cnt = 100;
	//pthread_attr_t attr;

	/*process body*/
	NDK_ScrInitGui();

	fprintf(stderr,"line:[%d]子应用运行中...",__LINE__);
	//NDK_ScrClrs();
	//NDK_ScrPrintf("子应用运行中...");
	//NDK_ScrRefresh();

	thread_run_flag1 = 0;
	pthread_mutex_init(&mutex, NULL);
	if((error=NDK_PthreadCreate(&tidp,NULL,pthread_vol,NULL))!=NDK_OK)
	{
		fprintf(stderr,"line:[%d]创建线程失败",__LINE__);
		NDK_FsDel("/root/flag");
		return;
	}
	NDK_SysMsDelay(10);
	if((error1=NDK_PthreadCreate(&tidp1,NULL,pthread_info,NULL))!=NDK_OK)
	{
		fprintf(stderr,"line:[%d]创建线程失败",__LINE__);
		NDK_FsDel("/root/flag");
		return;
	}
	NDK_SysMsDelay(5*1000);
	thread_run_flag1=1;
	NDK_PthreadJoin(tidp,NULL);
	NDK_PthreadJoin(tidp1,NULL);
	pthread_mutex_destroy(&mutex);
	NDK_FsDel("/root/flag");
	fprintf(stderr,"line:[%d]子应用执行成功",__LINE__);
	return;
}


