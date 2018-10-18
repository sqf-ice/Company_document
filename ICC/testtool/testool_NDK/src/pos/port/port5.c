/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port5.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_PortReadLen接口功能
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
#define 	TESTAPI		"NDK_PortReadLen"
#define		MAX_SIZE	BUFSIZE_SERIAL
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
void port5(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, port = -1, len = 0,len1 = 0,readlen = 0, j = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	char *pszAttr;
	int bpsid=0;
	
	//以下为函数体
	//测试前置:关闭串口
	//选择测试的串口(串口1或串口2),波特率
	cls_show_msg1(2, "%s测试中...", TESTAPI);
#if !K21_ENABLE  //跟开发确认低端平台无此接口
	NDK_SysSetSuspend(0);//禁止进入休眠
#endif
	port = select_serial();
	pszAttr = conf_serial(&bpsid);
	NDK_PortClose(port);
	
	//case1:对未开的串口进行取缓冲区字节数操作,应返回相应错误值
	if((ret = NDK_PortReadLen(port,&len)) != NDK_ERR_OPEN_DEV)	/*取缓冲区字节数*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	
	//case2:打开串口,传入非法参数NULL,非法串口
	cls_show_msg1(MAXWAITTIME, "短接所选串口的23脚,任意键继续...");
	if((ret=NDK_PortOpen(port, pszAttr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_PortReadLen(-1,&len)) != NDK_ERR_PARA)
		||((ret1=NDK_PortReadLen(port,NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
		
	//case3:正常使用测试
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
	NDK_SysDelay(MAXWAITTIME/(bpsid+1)*10);	/*数据在链路上需要时间,延迟一下,让部分数据进入接收缓冲区*/
	if((ret = NDK_PortReadLen(port,&len)) != NDK_OK)	/*取缓冲区字节数*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if(len != sizeof(sendBuf))//第一次读到的长度不为4096则多读几次
	{
		while(1)//经过开发和应用确认由于内核原因NDK_PortReadLen函数最大只能返回4095,所以这里必须循环的调用该函数通过多次调用的长度和来判断数据是否丢
		{
			if((ret = NDK_PortReadLen(port,&len1)) != NDK_OK)	/*取缓冲区字节数*/
			{				
				send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len1);
				goto ERR;
			}
			if(len1==0)
			{
				if(readlen!=sizeof(sendBuf))//多次调用后的长度和应该等于预期值
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, readlen, sizeof(sendBuf));
					goto ERR;
				}
				break;
			}
			readlen+=len1;
			memset(recvBuf,0,sizeof(recvBuf));
			if((ret = NDK_PortRead(port, len1, recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_OK)	/*读取串口数据*/
			{				
				send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
				goto ERR;
			}	
			if(len!=len1)//NDK_PortReadLen取到的长度必须和NDK_PortRead实际读到的一致
			{				
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, len1);
				goto ERR;
			}
		}
	}
	
	//case4:清缓冲后在调用
	if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortReadLen(port,&len)) != NDK_OK)	/*取缓冲区字节数*/
	{				
 		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
 		GOTOERR;
	}
	if(len != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
		GOTOERR;
	}
	
	//case5:串口数据接收后，接收缓冲区会清0么
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
	if((ret = NDK_PortReadLen(port,&len)) != NDK_OK)	/*取缓冲区字节数*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if(len != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
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
#if !K21_ENABLE  //跟开发确认低端平台无此接口
	NDK_SysSetSuspend(1);//允许进入休眠
#endif
	return;
}
