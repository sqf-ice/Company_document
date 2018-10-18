/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port2.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_PortRead接口功能
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
#define		TESTAPI			"NDK_PortRead"
#define		MAXTIMEOUT	10//超时测试时间
#define		MAXWAITTIME 150//接收等待时间
#define		WUCHASEC	0.03 //这个值待定
#define		MAX_SIZE	BUFSIZE_SERIAL


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
* history 		 		: author			date			remarks
*			  						chensl		   20130105	 		created
*****************************************************************/
void port2(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1, ret3 = -1, ret4 = -1, ret5 = -1, port = -1, len = 0, j = 0;
	float tmp = 0.0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0}, *pszAttr;
#if K21_ENABLE
	char str[32] = {0};
#endif
	int bpsid=0;
	
	//以下为函数体
	//测试前置:关闭串口
	//选择测试的串口(串口1或串口2),波特率
	cls_show_msg1(2, "%s测试中...", TESTAPI);
	port = select_serial();
	pszAttr = conf_serial(&bpsid);
	NDK_PortClose(port);

	//case1:对未开的串口进行读操作,应返回相应错误值
	if((ret = NDK_PortRead(port, sizeof(recvBuf), recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_ERR_OPEN_DEV)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	
	//case2:打开串口,传入非法参数NULL,非法串口,非法长度,非法超时时间
	cls_show_msg1(MAXWAITTIME, "短接所选串口的23脚,任意键继续...");
	if((ret=NDK_PortOpen(port, pszAttr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if(((ret=NDK_PortRead(-1, sizeof(recvBuf), recvBuf, 0, &len)) != NDK_ERR_PARA)
		||((ret1=NDK_PortRead(port, sizeof(recvBuf), recvBuf, 0, NULL)) != NDK_ERR_PARA)
		||((ret2=NDK_PortRead(port, sizeof(recvBuf), NULL, 0, &len)) != NDK_ERR_PARA)
		||((ret3=NDK_PortRead(port, sizeof(recvBuf), recvBuf, -1, &len)) != NDK_ERR_PARA)
		||((ret4=NDK_PortRead(port, BUFSIZE_SERIAL+1/*4097*/, recvBuf, 0, &len)) != NDK_ERR_PARA)
		||((ret5=NDK_PortRead(port, 0, recvBuf, 0, &len)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3, ret4, ret5);
		GOTOERR;
	}
	
	//case3:大数据读取(4K)	
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, MAX_SIZE, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if((len != sizeof(sendBuf)) || memcmp(sendBuf, recvBuf, len))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, sizeof(sendBuf));
		GOTOERR;
	}

	//case7:增加用例测试unLen>pnReadLen情况下portread函数的返回值和实际收到的数据值 20150819linwl,此处中低端存在差异,低端返回-10中端返回0
	if((ret = NDK_PortWrite(port, 1024, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, 2048, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if((len != 1024) || memcmp(sendBuf, recvBuf, len))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
		GOTOERR;
	}
	
	//case4:读超时测试
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	lib_StartStopwatch();
	if( ((ret=NDK_PortRead(port, sizeof(recvBuf), recvBuf, MAXTIMEOUT*1000, &len))!=NDK_ERR_TIMEOUT) || (tmp=fabs(lib_ReadStopwatch()-MAXTIMEOUT))>WUCHASEC)	/*读取串口数据*/
	{
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%ss)", __LINE__, TESTAPI, ret, str);
#else
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
#endif
		GOTOERR;
	}

	//case6:串口接收限制长度为4K(4K限制,case2已测)
	//case5:测试读完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, MAX_SIZE, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if((ret = NDK_PortClose(port)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	if (g_com!=INVALID_COM)
	{
		if ((ret=NDK_PortOpen(g_com, "115200,8,N,1")) != NDK_OK)
		{
			//初始化失败了,也不必send_result("line %d: 初始化串口错", __LINE__);
			cls_show_msg("line %d: 初始化串口错", __LINE__);
			return;
		}
	}
	return;
}
