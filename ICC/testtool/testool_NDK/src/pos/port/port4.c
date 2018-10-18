/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port4.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_PortClrBuf接口功能
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
#define 	TESTAPI		"NDK_PortClrBuf"
#define		MAX_SIZE	BUFSIZE_SERIAL
#define		MAXWAITTIME 150//接收等待时间
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
* history 		 		: author			date			remarks
*			  						chensl		   20130105	 		created
*****************************************************************/
void port4(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, port = -1, len = 0, j = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	char *pszAttr;
	int bpsid=0;
	
	//以下为函数体
	//测试前置:关闭串口
	//选择测试的串口(串口1或串口2),波特率
	cls_show_msg1(2, "%s测试中...", TESTAPI);
	port = select_serial();
	pszAttr = conf_serial(&bpsid);
	NDK_PortClose(port);
	
	//case1:对未开的串口进行清缓冲操作,应返回相应错误值
	if((ret = NDK_PortClrBuf(port)) != NDK_ERR_OPEN_DEV)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	
	//case2:打开串口,传入非法串口,应返回对应的错误值
	cls_show_msg1(MAXWAITTIME, "短接所选串口的23脚,任意键继续...");
	if((ret=NDK_PortOpen(port, pszAttr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_PortClrBuf(-1)) != NDK_ERR_PARA)
		||((ret1=NDK_PortClrBuf(100/*9*/)) != NDK_ERR_PARA))//linwl20131014 由于新增了一个扫描头PORT_NUM_SCAN 9 引起用例失败，故将用例测试值直接改大成100
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	
	//case3:清接收缓冲之后再去读数据,应读不到数据了
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(MAXWAITTIME/(bpsid+1)*10);	/*数据在链路上需要时间,延迟一下,让部分数据进入接收缓冲区*/
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, MAX_SIZE, recvBuf, MAXTIMEOUT*1000, &len)) != NDK_ERR_TIMEOUT)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}

	//case4:正常使用测试,清缓冲后校验读写的准确性
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
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
