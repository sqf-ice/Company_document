/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port3.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_PortWrite接口功能
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
#define 	TESTAPI		"NDK_PortWrite"
#define		MAX_SIZE	5678
#define		MAXWAITTIME 150//接收等待时间


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
void port3(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1, port = -1, len1 = 0, len2 = 0, j = 0;
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
	
	//case1:对未开的串口进行写操作,应返回相应错误值
	if((ret = NDK_PortWrite(port, sizeof(sendBuf), sendBuf)) != NDK_ERR_OPEN_DEV)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:打开串口,传入非法参数NULL,非法串口,非法长度
	cls_show_msg1(MAXWAITTIME, "短接所选串口的23脚,任意键继续...");
	if((ret=NDK_PortOpen(port, pszAttr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_PortWrite(-1, sizeof(sendBuf), sendBuf)) != NDK_ERR_PARA)
		||((ret1=NDK_PortWrite(port, 0, sendBuf)) != NDK_ERR_PARA)		
		||((ret2=NDK_PortWrite(port, sizeof(sendBuf), NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		GOTOERR;
	}
	
	//case3:普通数据包发送接收要比较一致(大于4K)
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, 4096, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len1)) != NDK_OK)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len1);
		GOTOERR;
	}
	if((ret = NDK_PortRead(port, MAX_SIZE-4096, recvBuf+4096, MAXWAITTIME/(bpsid+1)*1000, &len2)) != NDK_OK)	/*分两次读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len2);
		GOTOERR;
	}
	if(((len1+len2) != sizeof(sendBuf)) || memcmp(sendBuf, recvBuf, sizeof(sendBuf)))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len1+len2, sizeof(sendBuf));
		GOTOERR;
	}

	//case4:测试写完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortClose(port)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:挂断之后重新打开串口应该能够正常数据收发，不被上次异常挂断影响
	if((ret=NDK_PortOpen(port, pszAttr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret = NDK_PortWrite(port, MAX_SIZE, sendBuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret = NDK_PortRead(port, 4096, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len1)) != NDK_OK)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len1);
		GOTOERR;
	}
	if((ret = NDK_PortRead(port, MAX_SIZE-4096, recvBuf+4096, MAXWAITTIME/(bpsid+1)*1000, &len2)) != NDK_OK)	/*分两次读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len2);
		GOTOERR;
	}
	if(((len1+len2) != sizeof(sendBuf)) || memcmp(sendBuf, recvBuf, sizeof(sendBuf)))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len1+len2, sizeof(sendBuf));
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
