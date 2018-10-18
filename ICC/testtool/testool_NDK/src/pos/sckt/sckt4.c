/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt4.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_TcpConnect接口功能
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
#define 	TESTAPI		"NDK_TcpConnect"
#if defined ME68 || defined ME50NX //ME68采用GPRS方式时，SOCKET_STATUS_DISCONNECT状态大概要80s才能被获取到
#define		MAXWAITTIME 90//接收等待时间
#else
#define		MAXWAITTIME 60	//接收等待时间
#endif
#define		WUCHASEC	0.03 //这个值待定
#if K21_ENABLE
#define		MAX_SIZE	2047//低端无线支持2047
#else
#define		MAX_SIZE	5678
#endif
#define		MAXTIMEOUT	3//超时测试时间

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
void sckt4(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1, j = 0;
	uint unSckt = 0, sendLen = 0, recvLen = 0;
	ushort usPort = 3400+rand()%256;	
	char szRemoteIp[16] = "192.168.4.155", sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	float tmp = 0.0,connecttime=0;
	LinkType tmplinktype = getlinktype();
	uint unSckt1 = 0, sendLen1 = 0, recvLen1 = 0;
	char sendBuf1[MAX_SIZE] = {0}, recvBuf1[MAX_SIZE] = {0}, porttmp[6] = {0}, ip2[16] = {0};
	ushort port2;
	int len = 0;
#if K21_ENABLE
	int status=0;
	char str[32] = {0};
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//测试前置(下层连接要先建立)
	if(NetUp()!=SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		return;
	}
	
	//case1:设备未打开，进行连接操作应该失败
	if((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR_LINUX_TCP_NOT_OPEN/*NDK_ERR_TCP_NETWORK*/)//细化返回值
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//打开并绑定TCP句柄
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
	
	//case2:传入非法参数,空的Sock句柄
	if(((ret=NDK_TcpConnect(0, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR_LINUX_TCP_NOT_OPEN/*NDK_ERR_TCP_NETWORK*/)
		||((ret1=NDK_TcpConnect(-1/*0*/, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) == NDK_OK /*!= NDK_ERR,NDK_ERR_TCP_NETWORK*/)//20160325兼容中低端返回值修改成只要不成功都合理 linwl		
		||((ret2=NDK_TcpConnect(unSckt, NULL, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR_PARA)){
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		GOTOERR;
	}

	//case3:远程端口错误情况测试
	//ret = NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, 123, MAXWAITTIME);//以太网返回值跟其他通讯方式不一样
	//if(((getlinktype()==ETH && ret!=NDK_ERR_LINUX_TCP_REFUSE)) || (getlinktype()!=ETH && ret!=NDK_ERR_LINUX_TCP_TIMEOUT))
#if K21_ENABLE    
	if((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, 123, MAXWAITTIME)) != NDK_OK)//低端上已修改成非阻塞式的 20160323
#else
	if((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, 123, MAXWAITTIME)) == NDK_OK)//!= NDK_ERR_LINUX_TCP_TIMEOUT/*NDK_ERR_TCP_NETWORK*/)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE   //端口错误连接不上返回SOCKET_STATUS_DISCONNECT 状态
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_DISCONNECT/*SOCKET_STATUS_CONNECTERR*/)
	{
		send_result("line %d:%sTCP获取连接状态出错(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//新增case8:连接不上后进行写会返回网络错误NDK_ERR_TCP_NETWORK
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if ((ret=NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_ERR_TCP_NETWORK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	
	if((ret=NDK_TcpClose(unSckt)) != NDK_OK)//这里为什么要做挂断？
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_TcpWait(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(auto_flag==0||auto_flag==2)//需手动测试
	{
	//case4:超时测试，包括超时时间准确性
	if((ret=NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	usPort=3656+rand()%256;//由于关闭TCP通道现已改成非阻塞式,如果重新打开绑定相同句柄、相同IP和相同端口号会未彻底释放 20160325
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(tmplinktype!=ETH)//非以太网设备:无线连接内网IP会连接不上而超时，WIFI需要拔掉路由器上的网线构造连接超时 20140815 linwl
	{
		cls_show_msg("如果是WIFI,请拔掉路由器上WLAN口的网线后按任意键继续");
		lib_StartStopwatch();
#if K21_ENABLE  
		if(((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXTIMEOUT)) != NDK_OK))//低端上已修改成非阻塞式的 20160323
#else
		if(((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXTIMEOUT)) != NDK_ERR_TCP_TIMEOUT)||((tmp = lib_ReadStopwatch()-MAXTIMEOUT)>WUCHASEC))
#endif			
		{
			send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
			GOTOERR;
		}
#if K21_ENABLE   //超时连接不上返回SOCKET_STATUS_DISCONNECT
		if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_DISCONNECT/*SOCKET_STATUS_CONNECTERR*/)//获取TCP状态应返回连接失败
		{
			send_result("line %d:%sTCP获取连接状态出错(%d))", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
		cls_show_msg("如果是WIFI,请插上路由器上的网线后按任意键继续");
	}
	else//以太网:以太网应该要能够判断到未接以太网线，返回NDK_ERR_ETH_PULLOUT，并快速退出函数，暂时要求在1秒时间内退出 20140815 linwl
	{
		cls_show_msg("如果是以太网,请拔掉网线后按任意键继续");
		sleep(2);//增加两秒时间给系统去判断网线情况，避免未判断到插线情况就去连接返回超时 20140815
		lib_StartStopwatch();
#if K21_ENABLE
		if(((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXTIMEOUT)) != NDK_OK))//低端上已修改成非阻塞式的 20160323
#else
		if(((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXTIMEOUT)) != NDK_ERR_ETH_PULLOUT)||((tmp = lib_ReadStopwatch())>1))
#endif
		{
			send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
			GOTOERR;
		}
#if K21_ENABLE //如果是以太网?低端以太网还未支持先修改
		//ME50C以太网未插线情况下会返回-523，与modeom共用同一个参数  20180615  sull modify
		if((ret=NDK_TcpCheck(unSckt, &status, MAXWAITTIME)) != NDK_ERR_MODEM_NOLINE)
		//if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_DISCONNECT/*SOCKET_STATUS_CONNECTERR*/)//获取TCP状态应返回连接失败
		{
			send_result("line %d:%sTCP获取连接状态出错(%d))", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
		cls_show_msg("如果是以太网,请插上网线后按任意键继续");
		sleep(2);
	}
	
	if((ret=NDK_TcpClose(unSckt)) != NDK_OK)//这里为什么要做挂断？
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_TcpWait(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	}
	else
		send_result("line %d:%s需结合手动测试验证", __LINE__, TESTAPI);
	
	//case5:正常使用测试，进行一次正常的通讯
	if((ret=NDK_TcpOpen(&unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	usPort=3656+rand()%256;//由于关闭TCP通道现已改成非阻塞式,如果重新打开绑定相同句柄、相同IP和相同端口号会未彻底释放 20160325
	if((ret=NDK_TcpBind(unSckt, netsetting.LocalIP.Str, usPort)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	lib_StartStopwatch();
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
	connecttime=lib_ReadStopwatch();
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

#if 1
	//case6:连接已连接的socket句柄
#if K21_ENABLE 
	if((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR_TCP_NETWORK) //低端不支持连接已连接上的句柄
#else
	if((ret=NDK_TcpConnect(unSckt, szRemoteIp, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE //&&!(defined ME50NX||defined ME68) //夏新平台不支持SOCKET_STATUS_SEND_OVER状态  sull 20180416 modify
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_SEND_OVER)//连接已连接句柄不管成功还是失败都应该不影响实际状态，case5发完了数据，所以这里返回SOCKET_STATUS_SEND_OVER
	{
		send_result("line %d:%sTCP获取连接失败(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#elif 0//fined ME50NX||defined ME68)  //夏新平台又支持了该状态  20180716 modify
	if((ret=lib_tcp_checkstatus(unSckt, MAXWAITTIME))!=SOCKET_STATUS_CONNECT)
	{
		send_result("line %d:%sTCP获取连接失败(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#endif

	//case6:关闭socket，连接已断开的socket
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
#if K21_ENABLE
	if ((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR_LINUX_TCP_NOT_OPEN)
#else
	if ((ret=NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	if(auto_flag==0||auto_flag==2)//需手动测试
	{
		//case7:新增双端口测试，看开两个端口，两者之间是否会互相影响 20180503 sull add
		cls_printf("第二个服务器IP:\n");
		strcpy(ip2, DEFAULT_SVR_IP_STR);
		if ((ret=lib_kbgetinput(ip2, 0, sizeof(ip2)-1, &len, INPUTDISP_OTHER, 30, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(ip2, DEFAULT_SVR_IP_STR);	
		//itoa(netsetting.SvrPort, tmp, 10);                                         
		cls_printf("第二个服务器TCP端口:\n");
		port2 = 3460;
		sprintf(porttmp, "%d", port2);
		if ((ret=lib_kbgetinput(porttmp, 0, sizeof(porttmp)-1, &len, INPUTDISP_OTHER, 30, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			port2 = 3460;	                     
		else                                                                         
			port2 = atoi(porttmp);
		cls_printf("进行双端口测试...");
		if((ret = NDK_TcpOpen(&unSckt)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		if((ret = NDK_TcpOpen(&unSckt1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret = NDK_TcpConnect(unSckt, netsetting.SvrIP.Str, netsetting.SvrTcpPort, MAXWAITTIME)) != NDK_OK)
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
	if((ret = NDK_TcpConnect(unSckt1, ip2, port2, MAXWAITTIME)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
#if K21_ENABLE
	if((ret=lib_tcp_checkstatus(unSckt1, MAXWAITTIME))!=SOCKET_STATUS_CONNECT)
	{
		send_result("line %d:%sTCP获取连接失败(%d))", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	memset(sendBuf, 0,sizeof(sendBuf));
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret = NDK_TcpWrite(unSckt, sendBuf, sizeof(sendBuf), MAXWAITTIME, &sendLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(3);
	for (j=0; j<sizeof(sendBuf1); j++)
		sendBuf1[j]=rand()%256;
	if((ret = NDK_TcpWrite(unSckt1, sendBuf1, sizeof(sendBuf1), MAXWAITTIME, &sendLen1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf1,0,sizeof(recvBuf1));
	if ((ret=NDK_TcpRead(unSckt1, recvBuf1, sizeof(sendBuf1), MAXWAITTIME, &recvLen1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(recvLen1 != sendLen1 || memcmp(sendBuf1, recvBuf1, recvLen1))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen1, sendLen1);
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
	if((ret = NDK_TcpClose(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}
	if((ret = NDK_TcpWait(unSckt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}
	if((ret = NDK_TcpClose(unSckt1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}	
	if((ret = NDK_TcpWait(unSckt1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, recvLen, sendLen);
		GOTOERR;
	}	
	}
	else
		send_result("line %d:%s需结合手动测试验证", __LINE__, TESTAPI);

	//测试结束
#if K21_ENABLE
	memset(str,0,sizeof(str));
	ftos(connecttime,str);
	send_result("%s测试通过(TCP 连接时间:%s)", TESTAPI, str); 
#else
	send_result("%s测试通过(TCP 连接时间:%f)", TESTAPI, connecttime); 
#endif
ERR:
	NDK_TcpClose(unSckt);
	NDK_TcpWait(unSckt);
	NetDown();
	return;
}

