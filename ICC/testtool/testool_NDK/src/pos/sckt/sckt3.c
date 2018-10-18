/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt3.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_TcpBind接口功能
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
#define 	TESTAPI		"NDK_TcpBind"
#define		MAXWAITTIME 60//30	//接收等待时间
#define		MAX_SIZE	5678

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
void sckt3(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, i = 0;
	uint unSckt = 0, unScktTCP = 0, sendLen = 0, recvLen = 0;
	ushort usPort = 3400+rand()%256;	
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:未NetUp,bind失败
	if((ret=NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_ERR_TCP_NETWORK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}		
	if(NetUp()!=SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		GOTOERR;
	}
	
	//case2:传入非法参数,空的Sock句柄
	if(((ret=NDK_TcpBind(-1/*0*/, netsetting.LocalIP.Str, usPort)) != NDK_ERR_TCP_PARAM)
		||((ret1=NDK_TcpBind(unSckt, NULL, usPort)) != NDK_ERR_PARA)){
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	
#if 1
	//case3:绑定的IP异常的测试
	if((ret=NDK_TcpBind(unSckt, "192.168.5.2552", usPort)) != NDK_ERR_TCP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case4:正常使用测试，正确绑定后能够 正常连接建立数据通讯
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME) )!=SOCKET_STATUS_CONNECT)
	{
		send_result("line %d:%sTCP获取连接状态出错(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	for(i=0;i<sizeof(sendBuf);i++)
		sendBuf[i]=rand()%255;
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
	if ((ret=NDK_TcpRead(unSckt, recvBuf, sizeof(sendBuf), MAXWAITTIME, &recvLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(recvLen != sendLen || memcmp(sendBuf, recvBuf, recvLen))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}
	
	//case5:再用同类型的socket绑定同一IP/PORT应该失败
	if((ret=NDK_TcpOpen(&unScktTCP)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_TcpBind(unScktTCP, netsetting.LocalIP.Str, usPort)) != NDK_ERR_TCP_INVADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6.绑定一个已连接的SOCKET应该失败
	usPort++;
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_ERR_TCP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:关闭socket
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
	if((ret=NDK_TcpClose(unScktTCP)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_TcpWait(unScktTCP)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:绑定已关闭的socket应失败
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_ERR_TCP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_TcpClose(unSckt);
	NDK_TcpWait(unSckt);
	NDK_TcpClose(unScktTCP);
	NDK_TcpWait(unScktTCP);
	NetDown();
	return;
}

