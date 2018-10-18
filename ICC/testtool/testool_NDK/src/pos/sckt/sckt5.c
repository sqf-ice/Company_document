/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt5.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_TcpWrite接口功能
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
#define 	TESTAPI		"NDK_TcpWrite"
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
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: 
*****************************************************************/
void sckt5(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, j = 0;
	uint unSckt = 0, sendLen = 0, recvLen = 0, sendLen1 = 0, sendLen2 = 0;
	ushort usPort = 3400+rand()%256;	
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//测试前置(下层连接要先建立)
	if(NetUp()!=SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		return;
	}
	
	//打开socket并连接
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
	
	//case1:网络未链接，进行写操作应该失败，低端返回NDK_ERR_TCP_NETWORK
	for(j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
#if K21_ENABLE
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen))  != NDK_ERR_TCP_NETWORK)
#else
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_ERR_TCP_SEND)
#endif
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
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_CONNECT)
	{
		send_result("line %d:%sTCP获取连接失败(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case2:传入非法参数，所有参数的非法测试都要考虑
	if(((ret=NDK_TcpWrite(-1/*0*/, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) ==NDK_OK/*!= NDK_ERR_TCP_SEND*/)//中端返回NDK_ERR_TCP_SEND,低端返回NDK_ERR_LINUX_TCP_NOT_OPEN,故放宽判断条件
		||((ret1=NDK_TcpWrite(unSckt, NULL, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_ERR_PARA)
		//||((ret2=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, NULL)) != NDK_OK)//修改成case6 20160325 zhengry
	   )
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	
	//case6:发送长度传NULL时,发送数据应该能成功
#if !(defined ME50NX||defined ME68) //ME50Nx最后一个参数传NULL时返回NDK_ERR_PARA 故此case不测 20171124 sull
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sendLen=sizeof(sendBuf);
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
#endif

	//case3:正常使用测试(大数据)
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
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

	//case5:补充用例测试分次(两次)发送数据，一次接收linwl20130617
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf)/2, MAXWAITTIME, &sendLen1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(sendLen1 != sizeof(sendBuf)/2)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, sendLen1, sizeof(sendBuf)/2);
		GOTOERR;
	}
	if((ret=NDK_TcpWrite(unSckt, sendBuf+sizeof(sendBuf)/2, sizeof(sendBuf)/2, MAXWAITTIME, &sendLen2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(sendLen2 != sizeof(sendBuf)/2)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, sendLen2, sizeof(sendBuf)/2);
		GOTOERR;
	}
	sendLen=sendLen1+sendLen2;
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
#if ETH_ENABLE||WLAN_ENABLE//无线模块无法构造拔线动作,不测试
	if(auto_flag==0||auto_flag==2)
	{
		//case7:断开物理连接线路,仍可以进行发送数据
		cls_show_msg1(MAXWAITTIME, "请断开以太网物理连接,任意键继续...");
		if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(MAXWAITTIME, "请接上以太网物理连接,任意键继续...");
		//低端WiFi模块需要读走数据,否则会影响6号用例,其他产品也加上应该也不影响 20180705 add
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_TcpRead(unSckt,recvBuf,sendLen,MAXWAITTIME,&recvLen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
		send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
#endif	
	//case4:关闭socket，写已关闭的socket应失败
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
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_ERR_LINUX_TCP_NOT_OPEN)
#else
	if((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_ERR_TCP_SEND)
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

