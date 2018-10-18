
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mount.h>
#include <assert.h>

#include "NDK.h"
#include "NDK_Net.h"

#define MUX_TMPLE_FILE 		"/tmp/gsm_mux"
#define CLOSE_WIRELESS_MUX_METHOD 0
#define ENALBE_WIRELESS_MUX_METHOD 1
#define INIT_MUX_PTS_METHOD   2

static int m_MuxAuxInitFlag=0;
static int getmuxinfo_flag=0;
static int EnableMux=0;

extern int g_AuxNo;

int RunGsmMux()
{
	//ShowSignalQuality(-1, -1);
	system("/usr/sbin/gsmMuxd -r  -p /dev/ttyS2 -w -s /dev/modem /dev/ptmx /dev/ptmx &");
	return 0;
}

//channel :3 AT通道
//channel :4 PPP通道
//channel :5 信号通道
/*
#define AUX4 3
#define AUX5 4
#define AUX6 5
*/

#define AT_CHANNEL 3
#define CSQ_CHANNEL 5

int ndk_closeWirelessAux()
{
	int ret;
#if 0
//	char str_command[256];
	if(access(MUX_TMPLE_FILE,F_OK)==0)
  	{
			NDK_PortClose(PORT_NUM_WIRELESS);
			NDK_PortClose(PORT_NUM_MUX1);
			NDK_PortClose(PORT_NUM_MUX2);
	  	wlm_methodcall_gsmmux(CLOSE_WIRELESS_MUX_METHOD,&ret);
  	}
#endif
	if(access(MUX_TMPLE_FILE,F_OK)==0)
		NDK_PortClose(PORT_NUM_MUX1);
	else
	    NDK_PortClose(PORT_NUM_WIRELESS);
	comm_methodcall_general(FUNC_WLM_RESET_BEFORE,0,&ret);
	return ret;
}

int ndk_InitWirelessAux()
{
	 int ret;
	 comm_methodcall_general(FUNC_WLM_RESET_AFTER,0,&ret);
	 return ret;
}

int ndk_GetMuxInfo(void)
{
	int i;
	int running_flag=0;
	if(access(MUX_TMPLE_FILE,F_OK)==0)
	{
		return 0;
	}
	if(access("/tmp/gsm_runnig",F_OK)==0)
	{
		running_flag=1;
	}
  if(running_flag==0)
  {
  	//pclose(fd);
  	return 0;
  }
	for(i=0;i<100;i++)
	{
		if(access(MUX_TMPLE_FILE,F_OK)==0)
		{
			return 0;
		}
		ndk_msdelay(100);
	}
	if(i==100)
	{
		fprintf(stderr,"start Mux error\n");
		return -1;
	}
	else 
	{
		return 0;
	}
	return 0;
}

int ndk_InitMuxPtsFile()
{
/*
	struct timeval tv;  
	struct timeval tz;  
	gettimeofday(&tv, &tz); 
	fprintf(stderr,"tv->sec=%d,tv-->usec=%d\n",tv.tv_sec,tv.tv_usec);
*/
	int ret;
	comm_methodcall_general(FUNC_WLM_GSMMUX_FILEINIT,0,&ret);
	return ret;
}

