/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port1.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_PortOpen和NDK_PortClose接口功能
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
#define		TESTAPI		"NDK_PortOpen,NDK_PortClose"
#define		BPS_NUM		10
#define		DATABIT_NUM		4
#define		CHECKBIT_NUM	3
#define		STOPBIT_NUM		2
#define		MAX_SIZE		BUFSIZE_SERIAL
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
void port1(void)
{
	//以下为局部变量
	int port = 0, len = 0, i = 0, j = 0, k = 0, n = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0}, szTmp[20] = {0};
	char *pszAttr;
	int ret = -1, ret1 = -1, ret2 = -1, ret3 = -1, ret4 = -1, ret5 = -1;
	char *bps[BPS_NUM] ={"300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400"};
	char *dataBit[DATABIT_NUM] = {"8", "7", "6", "5"};
	char *checkBit[CHECKBIT_NUM] = {"N", "O", "E"};
	char *stopBit[STOPBIT_NUM] = {"1", "2"};	
	int bpsid=0;
	
	//以下为函数体
	//测试前置:关闭串口
	//选择测试的串口(串口1或串口2),波特率
	cls_show_msg1(2, "%s测试中...", TESTAPI);
	port = select_serial();
	pszAttr = conf_serial(&bpsid);
	NDK_PortClose(port);
#if !(ANDIROD_ENABLE)
	//case1:传入非法参数NULL,非法串口,非法长度
	if(((ret=NDK_PortOpen(-1, pszAttr)) != NDK_ERR_PARA)
		||((ret1=NDK_PortOpen(port, NULL)) != NDK_ERR_PARA)
		||((ret2=NDK_PortOpen(port, "115201,8,N,1")) != NDK_ERR_PARA)
		||((ret3=NDK_PortOpen(port, "115200,9,N,1")) != NDK_ERR_PARA)
		||((ret4=NDK_PortOpen(port, "115200,8,A,1")) != NDK_ERR_PARA)
		||((ret5=NDK_PortOpen(port, "115200,8,N,3")) != NDK_ERR_PARA)){
		send_result("line %d:%s测试失败(%d,%d,%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3, ret4, ret5);
		GOTOERR;
	}
#endif
	//case2:打开并短接串口1的23脚,进行自收发测试
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
	if((ret = NDK_PortClose(port)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:测试各波特率、数据位、停止位、校验位串口的初始化
	for(i=0; i<BPS_NUM; i++)
		for(j=0; j<DATABIT_NUM; j++)
			for(k=0; k<CHECKBIT_NUM; k++)
				for(n=0; n<STOPBIT_NUM; n++)
				{
					sprintf(szTmp,"%s,%s,%s,%s",bps[i],dataBit[j],checkBit[k],stopBit[n]);
					if((ret=NDK_PortOpen(port, szTmp)) != NDK_OK)
					{
						send_result("line %d:初始化串口(%d, %s)失败(ret=%d)", __LINE__, port, szTmp, ret);
						GOTOERR;
					}
					if((ret = NDK_PortClose(port)) != NDK_OK)
					{
						send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
						GOTOERR;
					}
					cls_printf("初始化串口(%d, %s)成功", port, szTmp);
				}

	//case4:关闭串口,读写都应返回失败
	if((ret = NDK_PortWrite(port, sizeof(sendBuf), sendBuf)) != NDK_ERR_OPEN_DEV)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortRead(port, sizeof(recvBuf), recvBuf, MAXWAITTIME/(bpsid+1)*1000, &len)) != NDK_ERR_OPEN_DEV)	/*读取串口数据*/
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
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

