/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name			: sckt9.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_TcpCheck获取tcp状态
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_TcpCheck"	
#define		MAXWAITTIME 60	//接收等待时间
#if K21_ENABLE
#define		MAX_SIZE	2047//低端无线支持2047
#else
#define		MAX_SIZE	5678
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 				: void wifi6(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			   zhengry      20160321    created 		 
*****************************************************************/
void sckt9(void)
{
	/*private & local definition*/
	int ret=0, errcode=0, j=0;
	char ErrIp[16] = "192.168.4.155";
	uint unSckt = 0, sendLen = 0, recvLen = 0;
	int tcp_status;
	ushort usPort = 3400+rand()%256;	
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//测试前置(下层连接要先建立)
	if(NetUp() != SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		return;
	}
	if((ret = NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:异常测试:未调用NDK_NetAddRouterTable,会返回网络接口（wifi 或者PPP）错误NDK_ERR_TCP_NETWORK.目前暂不能测
	
	//case1:测试未调用连接服务器函数,获取状态应返回未连接SOCKET_STATUS_DISCONNECT
	if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(tcp_status != SOCKET_STATUS_DISCONNECT)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//连接服务器
	if((ret = NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:参数异常测试,两个接收参数同时为NULL才返回NDK_ERR_PARA
	if((ret = NDK_TcpCheck(unSckt, NULL, NULL) ) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret); 
		GOTOERR;  
	}
	
	//case3:socket句柄错误,获取TCP状态应返回NDK_ERR_LINUX_TCP_NOT_OPEN
	if((ret = NDK_TcpCheck(unSckt+1, &tcp_status, &errcode) ) != NDK_ERR_LINUX_TCP_NOT_OPEN)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

 	//case4:正常测试,测试获取状态为已连接SOCKET_STATUS_CONNECT
	lib_StartStopwatch();
	while(1)
	{
		if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(tcp_status == SOCKET_STATUS_CONNECT)
			break;		
		if(tcp_status!=SOCKET_STATUS_CONNECTING)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
			goto ERR;
		}
		if(lib_ReadStopwatch()>MAXWAITTIME)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
			goto ERR;
		}
	}

	//case5:发送大数据,获取状态应返回正在发送SOCKET_STATUS_READY_TO_SEND,发送完成后返回SOCKET_STATUS_SEND_OVER
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0   //发送时间太快 捕捉不到SOCKET_STATUS_READY_TO_SEND这个状态,暂不测试
	lib_StartStopwatch();
	while(1)
	{
		if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(tcp_status == SOCKET_STATUS_READY_TO_SEND)
			break;	
		if(lib_ReadStopwatch()>MAXWAITTIME)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
			goto ERR; 
		}
	}
#endif
	if(sendLen != sizeof(sendBuf))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, sendLen, sizeof(sendBuf));
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret=NDK_TcpRead(unSckt, recvBuf, sendLen, MAXWAITTIME, &recvLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(recvLen != sendLen || memcmp(sendBuf, recvBuf, recvLen))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR; 
	}
	if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0//(defined ME50NX||defined ME68)&&getlinktype()!=GPRS  //夏新平台不支持SOCKET_STATUS_SEND_OVER状态  20180416 sull modify
	//夏新平台又支持了该状态  20180716 modify
	if(tcp_status != SOCKET_STATUS_CONNECT)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
		GOTOERR;
	}
#else
	if(tcp_status != SOCKET_STATUS_SEND_OVER)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
		GOTOERR;
	}
#endif
	if((ret=NDK_TcpClose(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8:关闭TCP句柄通道后,调用获取TCP状态应该返回NDK_ERR_LINUX_TCP_NOT_OPEN socket句柄错误
	if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_ERR_LINUX_TCP_NOT_OPEN)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}

	//case6:连接错误IP的服务器,由于TcpConnect是非阻塞式,应返回成功,获取TCP状态时先返回正在连接SOCKET_STATUS_CONNECTING,最后应该返回连接失败SOCKET_STATUS_DISCONNECT
	if((ret = NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	usPort=3656+rand()%256;//由于关闭TCP通道现已改成非阻塞式,如果重新打开绑定相同句柄、相同IP和相同端口号会未彻底释放 20160325
	if((ret = NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	if((ret = NDK_TcpConnect(unSckt, ErrIp, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)

	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
 	if(tcp_status != SOCKET_STATUS_CONNECTING)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
		GOTOERR;
	}
	lib_StartStopwatch();
	while(1)
	{
		if((ret = NDK_TcpCheck(unSckt, &tcp_status, &errcode) ) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(tcp_status == SOCKET_STATUS_DISCONNECT)
			break;		
		if( tcp_status != SOCKET_STATUS_CONNECTING)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
			goto ERR;
		}
		if(lib_ReadStopwatch()>MAXWAITTIME)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, tcp_status);
			goto ERR;
		}
	}
	if((ret=NDK_TcpClose(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case8:NDK_ERR_OPEN_DEV:socket驱动打开失败和NDK_ERR_IOCTL：调用驱动失败暂时无法测试
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_TcpClose(unSckt);
	NDK_TcpWait(unSckt);
	NetDown();
	return;
}

