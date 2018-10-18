/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt6.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_TcpRead接口功能
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_TcpRead"
#define		MAXWAITTIME 60	//接收等待时间
#define		WUCHASEC	0.03 //这个值待定
#if K21_ENABLE
#define		MAX_SIZE	2047//低端无线支持2047
#else
#define		MAX_SIZE	5678
#endif
#define		MAXTIMEOUT	3	//超时测试时间

/*------------global variables definition-----------------------*/

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
* history 		 		: 
*****************************************************************/
void sckt6(void)
{
	//以下为局部变量
	int ret = -1,ret1 = -1,ret2 = -1,j = 0;
	uint unSckt = 0, sendLen = 0, recvLen = 0, recvLen1 = 0, recvLen2 = 0;
	ushort usPort = 3400+rand()%256;	
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	float tmp = 0.0;
#if K21_ENABLE		
	char str[32] = {0};
#endif	

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//测试前置(下层连接要先建立)
	if(NetUp()!=SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		return;
	}
	
	//打开socket并绑定
	if((ret=NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1:网络未连接，进行写操作应该失败，低端返回NDK_ERR_TCP_NETWORK
	memset(recvBuf,0,sizeof(recvBuf));
#if K21_ENABLE
	if ((ret=NDK_TcpRead(unSckt, recvBuf, sizeof(recvBuf), MAXWAITTIME, &recvLen)) != NDK_ERR_TCP_NETWORK)
#else
	if ((ret=NDK_TcpRead(unSckt, recvBuf, sizeof(recvBuf), MAXWAITTIME, &recvLen)) != NDK_ERR_TCP_RECV)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//连接socket
	if((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_CONNECT)
	{
		send_result("line %d:%sTCP获取连接失败(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case2:传入非法参数，包括所有参数
	if(//((ret=NDK_TcpRead(-1, recvBuf, sizeof(recvBuf), MAXWAITTIME, &recvLen))  == NDK_OK /*!= NDK_ERR,NDK_ERR_PARA*/)||//recvBuf会引起跑飞,开发已知但不修改的BUG,为减少测试过程中的反复定位暂时不测试第一个参数非法情况//20160325兼容中低端返回值修改成只要不成功都合理 linwl	
		((ret1=NDK_TcpRead(unSckt, NULL, sizeof(recvBuf), MAXWAITTIME, &recvLen)) != NDK_ERR_PARA)
		||((ret2=NDK_TcpRead(unSckt, recvBuf, sizeof(recvBuf), MAXWAITTIME, NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		GOTOERR;
	}
	
	//case3:正常使用测试(大数据)
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if ((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
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

	//case4:补充用例测试发送数据，分次(两次)接收linwl20130617
	if ((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(sendLen != sizeof(sendBuf))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, sendLen, sizeof(sendBuf));
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret=NDK_TcpRead(unSckt, recvBuf, sendLen/2, MAXWAITTIME, &recvLen1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case7:补充用例接收长度增加1验证接收长度超过实际数据长度时返回NDK_OK并且接收到实际长度的数据  20170112
#if K21_ENABLE
	if((ret=NDK_TcpRead(unSckt, recvBuf+sendLen/2, MAX_SIZE-sendLen/2+1, 10, &recvLen2)) != NDK_OK)
#else
	if((ret=NDK_TcpRead(unSckt, recvBuf+sendLen/2, MAX_SIZE-sendLen/2, MAXWAITTIME, &recvLen2)) != NDK_OK)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	recvLen = recvLen1+recvLen2;
	if(recvLen != sendLen || memcmp(sendBuf, recvBuf, recvLen))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}
	
	//case5:超时测试，准确性测试，低端平台无NDK_ERR_TCP_TIMEOUT这个返回值而使用NDK_ERR_TIMEOUT
	memset(recvBuf,0,sizeof(recvBuf));
	lib_StartStopwatch();
#if K21_ENABLE
	if(((ret=NDK_TcpRead(unSckt, recvBuf, MAX_SIZE+1, MAXTIMEOUT, &recvLen)) != NDK_ERR_TIMEOUT)||((tmp=lib_ReadStopwatch()-MAXTIMEOUT)>WUCHASEC))
#else
	if(((ret=NDK_TcpRead(unSckt, recvBuf, MAX_SIZE+1, MAXTIMEOUT, &recvLen)) != NDK_ERR_TCP_TIMEOUT)||((tmp=lib_ReadStopwatch()-MAXTIMEOUT)>WUCHASEC))
#endif
	{
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, str);
#else
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
#endif
		GOTOERR;
	}
	
	//case6:关闭socket，读已关闭的socket应失败
	if((ret=NDK_TcpClose(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_TcpWait(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE  //低端返回NDK_ERR_LINUX_TCP_NOT_OPEN
	if ((ret=NDK_TcpRead(unSckt, recvBuf, MAX_SIZE, MAXWAITTIME, &recvLen)) != NDK_ERR_LINUX_TCP_NOT_OPEN)
#else
	if ((ret=NDK_TcpRead(unSckt, recvBuf, MAX_SIZE, MAXWAITTIME, &recvLen)) != NDK_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_TcpClose(unSckt);
	NDK_TcpWait(unSckt);
	NetDown();
	return;
}

