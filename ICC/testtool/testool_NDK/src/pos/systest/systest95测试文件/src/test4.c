/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: test4.c
* Author 			: 
* version			: 
* DATE				: 取硬件信息/无线线程交叉
* directory 			: 
* description			: 
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

pid_t gettid(void)
{

	return syscall(SYS_gettid);

}

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
int thread_run_flag1 = 0;
static pthread_mutex_t mutex;
static int pthread_retval = 0;
static char WlanEssid[] = "test";

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
	//while(thread_run_flag1 == 0)
	fprintf(stderr,"%s,%d devmgr devpid: %d\n",__FUNCTION__,__LINE__,gettid());
	while(thread_run_flag1 == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_info is runing\n",__LINE__);
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

static void *pthread_wlm(void *arg)
{
	EM_WLM_STATUS emStatus = -1;
	int err = 0, ret = -1;
	EM_PPP_STATUS actual = PPP_STATUS_DISCONNECT;
	uint rlen = 0, slen = 0;
	uint time = 0;
	ulong LocalIp = 0;
	uchar ip[16] = {0};
	uint sckt = 0;
	ushort port = 0;
	char sbuf[] = "111111111111111111112222222222222222222222222222222";
	char rbuf[1024] = {0};

	if ((ret = NDK_WlInit(2000,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
	{
		fprintf(stderr,"line:[%d]无线初始化失败",__LINE__);
		NDK_PppHangup(1);
		goto ERR1;
	}
	if((ret=NDK_PppDial("card", "card"))!=NDK_OK)
	{
		fprintf(stderr,"line:[%d]打开PPP通道失败!",__LINE__);
		NDK_PppHangup(1);
		goto ERR1;
	}
	NDK_SysStartWatch();
	while(1)
	{
		if((ret=NDK_PppCheck(&actual, &err))!=NDK_OK)
		{
			fprintf(stderr,"line:[%d]获取ppp状态失败!",__LINE__);
			NDK_PppHangup(1);
			goto ERR1;
		}
		if(actual==PPP_STATUS_CONNECTED)
		{
				break;
		}
		NDK_SysStopWatch(&time);
		if(time/1000>10)
		{
			fprintf(stderr,"line:[%d]获取ppp状态超时!",__LINE__);
			NDK_PppHangup(1);
			goto ERR1;
		}
	}
	//thread_run_flag 置1时 线程退出
	fprintf(stderr,"%s,%d devpid: %d\n",__FUNCTION__,__LINE__,gettid());
	while(thread_run_flag1 == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_wlm is runing\n",__LINE__);
		if((ret = NDK_TcpOpen(&sckt)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]打开TCP通道失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
		if((ret = NDK_TcpConnect(sckt, "218.66.48.230", 3459, 5)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]TCP连接失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
		if((ret = NDK_TcpWrite(sckt, sbuf, sizeof(rbuf), 5, &slen)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]数据发送失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
		memset(rbuf, 0, sizeof(rbuf));
		if((ret = NDK_TcpRead(sckt, rbuf, sizeof(rbuf), 5, &rlen)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]数据接收失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
		if((ret = NDK_TcpClose(sckt)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]断开TCP失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
		if((ret = NDK_TcpWait(sckt)) != NDK_OK)
		{
			fprintf(stderr,"line:[%d]断开TCP失败!",__LINE__);
			NDK_PppHangup(1);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			goto ERR1;
		}
	}
	if((ret = NDK_PppHangup(1)) != NDK_OK)
	{
		fprintf(stderr,"line:[%d]断开PPP失败!",__LINE__);
		NDK_PppHangup(1);
		NDK_TcpClose(sckt);
		NDK_TcpWait(sckt);
		goto ERR1;
	}
	NDK_SysStartWatch();
	while(1)
	{
		if((ret=NDK_PppCheck(&actual, &err))!=NDK_OK)
		{
			fprintf(stderr,"line:[%d]获取ppp状态失败!",__LINE__);
			NDK_PppHangup(1);
			goto ERR1;
		}
		if(actual==PPP_STATUS_DISCONNECT)
		{
				break;
		}
		NDK_SysStopWatch(&time);
		if(time/1000>5)
		{
			fprintf(stderr,"line:[%d]获取ppp状态超时!",__LINE__);
			NDK_PppHangup(1);
			goto ERR1;
		}
	}

	NDK_SysMsDelay(10);
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

	/*process body*/
	NDK_ScrInitGui();

	fprintf(stderr,"line:[%d]子应用运行中...",__LINE__);
	//NDK_ScrClrs();
	//NDK_ScrPrintf("子应用运行中...");
	//NDK_ScrRefresh();

	thread_run_flag1 = 0;
	pthread_mutex_init(&mutex, NULL);
	if((error=NDK_PthreadCreate(&tidp,NULL,pthread_wlm,NULL))!=NDK_OK)
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
	NDK_SysMsDelay(30*1000);
	thread_run_flag1=1;
	NDK_PthreadJoin(tidp,NULL);
	NDK_PthreadJoin(tidp1,NULL);
	pthread_mutex_destroy(&mutex);
	NDK_FsDel("/root/flag");
	fprintf(stderr,"line:[%d]子应用执行成功",__LINE__);
	return;
}


