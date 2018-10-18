/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest32.c
* Author 			: chensj
* version			: 
* DATE			: 20130624
* directory 	: 
* description	: 	USB串口综合测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	MAXWAITTIME 	30	//30
#define	TESTITEM		"USB串口综合测试"
#define DEFAULT_CNT_VLE	2048//3000
#define	DEFAULT_CNT_STR	"2048"//"3000"

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
* history 				: author			date			remarks
*					
*****************************************************************/
//PC--->POS
static void usb_pctopos(void)
{
	/*private & local definition*/
	int ret = -1,  datalen = 0, cnt = 0,succconut = 0, i = 0;
	char recvBuf[BUFSIZE_SERIAL+1] = {0}, CntStr[8] = DEFAULT_CNT_STR, sendBuf[BUFSIZE_SERIAL+1] = {0}; 
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");	
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//测试前置：
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");	
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg("line %d:打开USB失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
	NDK_PortClrBuf(PORT_NUM_USB);//避免其他用例影响清一下缓冲 20131114 zhengdc提出
	//生产测试数据包并发送给后台
	cls_show_msg("请把AccessPort工具打开串口,任意键继续");
	cls_show_msg1(2,"发送数据中...");
	for(i=0;i<BUFSIZE_SERIAL;i++)
		sendBuf[i] = rand()%256;
	if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
	if(cls_show_msg("请将AccessPort接收到的数据复制到发送框并开启自动发送,确认键继续")!=ENTER)
	{
		NDK_PortClose(PORT_NUM_USB);
		return;	
	}
	//接收压力测试
	i=0;
	while(1)
	{
		cls_printf("USB串口接收压力中\n总共:%d次\n已进行:%d次\n",cnt,i);
		if(i++>=cnt)
			break;
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB, BUFSIZE_SERIAL, recvBuf, MAXWAITTIME*1000, &datalen)) != NDK_OK)
		{
			NDK_PortClrBuf(PORT_NUM_USB);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, datalen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(datalen != BUFSIZE_SERIAL)
		{
			NDK_PortClrBuf(PORT_NUM_USB);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, datalen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(MemCmp(sendBuf, recvBuf,  BUFSIZE_SERIAL))
		{
			NDK_PortClrBuf(PORT_NUM_USB);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败,", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
	}

	cls_show_msg("测试完毕,请断开AccessPort连接并停止发送,按任意键继续");
	cls_show_msg_record(FILENAME,FUNCNAME,"USB串口接收压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_USB);
	return;
}

//PC<--->POS
static void usb_comm_pc(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, succconut = 0,cnt=0,nFirstRun=0;
	char recvBuf[BUFSIZE_SERIAL+1] = {0};
	char CntStr[8] = DEFAULT_CNT_STR,sendBuf[BUFSIZE_SERIAL+1]={0};
	uint unLen=0;
	
	/*process body*/	
	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);

	//测试前置：
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");	
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg("line %d:打开USB失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return ;
	}
	//生产测试数据包并发送给后台
	cls_show_msg("请把AccessPort工具打开连接上,任意键继续");
	for(i=0;i<BUFSIZE_SERIAL;i++)
		sendBuf[i] = rand()%256;
	
	//读写压力测试
	i=0;
	while(1)
	{
		cls_printf("USB串口读写压力中\n总共:%d次\n已进行:%d次\n",cnt,i);
		if(i++>=cnt)
			break;
		if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
		{				
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}

		if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		if(!nFirstRun)
		{
			nFirstRun++;
			if(cls_show_msg("请将AccessPort接收到的数据复制到发送框并开启自动发送,确认键继续")!=ENTER)
			{
				NDK_PortClose(PORT_NUM_USB);
				return;	
			}
		}
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(recvLen != BUFSIZE_SERIAL)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(MemCmp(sendBuf, recvBuf,  BUFSIZE_SERIAL))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败(recvlen=%d)", __LINE__, i, recvLen)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
	}

	cls_show_msg("测试完毕,请断开AccessPort连接并停止发送,按任意键继续");
	cls_show_msg_record(FILENAME,FUNCNAME,"USB串口读写压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_USB);
	return;
}

void usb_openclosepress()
{
	int ret = 0, i = 0, cnt = 0, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	if(cls_show_msg("请确保POS和PC已通过USB线连接,确认键继续...")!=ENTER)
		return;	

	NDK_PortClose(PORT_NUM_USB);
	
	while(1)
	{
		cls_printf("USB串口开关压力测试中\n总共:%d次,已执行:%d次,已成功:%d次",cnt,i,succ);
		if(i++>=cnt)//达到测试次数后退出
			break;	
		if((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开USB串口失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
#if K21_ENABLE
		sleep(3);
#endif
		if((ret=NDK_PortClose(PORT_NUM_USB))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭USB串口失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"总共进行%d次开关USB压力测试,已成功%d次",i-1,succ);
	NDK_PortClose(PORT_NUM_USB);
	return;
}

void usb_abnormal(void)
{	
	/*process body*/	
	int ret = 0,i = 0,recvLen = 0;
	char sendBuf[BUFSIZE_SERIAL+1]={0},recvBuf[BUFSIZE_SERIAL+1]={0};
	time_t oldtime = 0;

	NDK_PortClose(PORT_NUM_USB);
	//初始化发送缓冲
	for(i=0; i<sizeof(sendBuf)-1; i++)
		sendBuf[i]=rand()%256;
	
	//case1:在打开之后拔插USB线应该不影响后面的通讯,不出现死机等异常
	cls_show_msg("请将POS和PC通过USB线连接,并开启PC端的accessport工具,任意键继续");
	if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
	{
		cls_show_msg("line %d测试失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
#if !(defined ME50S ||defined ME50NS)	
	cls_show_msg("请先拔插一下USB线,再打开PC工具串口,按任意键继续");
#else
	cls_show_msg("请先拔插一下USB线,按任意键继续");
	if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
	{
		cls_show_msg("line %d测试失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
	cls_show_msg("打开PC工具串口,按任意键继续");
#endif	
	if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)",__LINE__,ret);
		goto END;
	}
	if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送数据失败(%d)",__LINE__,ret);
		goto END;
	}
	cls_show_msg("请将AccessPort接收到的数据复制到发送框并发送,任意键继续");
	memset(recvBuf, 0, sizeof(recvBuf));
	if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
	{
		cls_show_msg("line %d:接收数据失败(%d)",__LINE__,ret);
		goto END;
	}
	if(recvLen != BUFSIZE_SERIAL)
	{
		cls_show_msg("line %d:数据收发长度不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_SERIAL);
		goto END;
	}
	if(MemCmp(sendBuf, recvBuf,  BUFSIZE_SERIAL))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		goto END;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("1号子用例测试通过,任意键继续");
	
	//case2:测试连续发送过程中拔插USB线
	cls_show_msg("请将POS和PC通过USB线连接,并开启PC端的accessport工具,任意键继续");
	if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	cls_show_msg("PC打开串口,按任意POS输出数据3s后拔出USB线");//可以多次拔插
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>MAXWAITTIME)
			break;
		ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf);
		switch(ret)
		{
			case -1://低端可能返回-1
			case -2:
			case NDK_ERR_USB_LINE_UNCONNECT://拔线之后应该能够获取到-100状态,再插上线后重新操作串口应该能够正常工作不死机
			case NDK_ERR_TIMEOUT://由于拔线操作的时间误差,也可以能返回超时
			case -12:     //ME50S和ME50NS拔线后返回此返回值
				//cls_show_msg("连接USB线,任意键继续");
				NDK_PortClose(PORT_NUM_USB);
				cls_show_msg("关闭PC端串口后，连接USB线");
				if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
				{
			    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
			    	goto END;
				}
				cls_show_msg("打开PC端串口,任意键继续");
				break;	
			case NDK_OK:
				cls_printf("发送成功");
				break;
			default:
				cls_show_msg("line %d:未知的返回值(ret=%d)", __LINE__, ret);
				goto END;
		}
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("2号子用例测试通过,任意键继续");

#if !(defined ME50S ||defined ME50NS)   //ME50S和ME50NS 由于USB机制不同,read拔掉线不能检测故不测
	//case3:测试连续接收过程中拔插USB线
	cls_show_msg("请将POS和PC通过USB线连接,并重启PC端的accessport工具,任意键继续");
	if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	cls_show_msg("PC打开串口,POS按任意键将输出%dB数据到PC", BUFSIZE_SERIAL);
	cls_printf("POS--->PC(%dB)\n", sizeof(sendBuf)-1);
	if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
		goto END;		
	}
	show_msg1(3, "完毕!");
	//cls_show_msg("请将AccessPort接收到的数据复制到发送框并开启自动发送,按任意键继续");
	//cls_show_msg("请在PC发送数据3秒后开始拔插USB线操作,按任意键继续");
	cls_show_msg("请复制数据开始自动发送后拔USB,按任意键继续");//拔usb后等时间到提示信息出来后再插上usb
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>MAXWAITTIME)
			break;
		memset(recvBuf, 0, sizeof(recvBuf));
		ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, MAXWAITTIME*1000, &recvLen);
		switch(ret)
		{
			/*case :
			case :
			case :
			case :
			case :
				cls_show_msg("line %d:%s测试失败(ret=%d)", __LINE__, ret);
				goto END;*/
			case -1://低端可能返回-1
			case -2:
			case NDK_ERR_USB_LINE_UNCONNECT://拔线之后应该能够获取到-100状态,再插上线后重新操作串口应该能够正常工作不死机
			case NDK_ERR_TIMEOUT://由于拔线操作的时间误差,也可以能返回超时
				//cls_show_msg("连接USB线,任意键继续");
				NDK_PortClose(PORT_NUM_USB);
				cls_show_msg("停止发送,关闭串口后连接USB");
				if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
				{
			    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
			    	goto END;
				}
				cls_show_msg("打开串口,自动发送,任意键继续");
				break;	
			case NDK_OK:
				cls_printf("接收成功");
				break;
			default:
				cls_show_msg("line %d:未知的返回值(ret=%d)", __LINE__, ret);
				goto END;
		}
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("3号子用例测试通过,任意键继续");
#endif

	//case4:测试USB串口打开之后开关PC或者重启PC,POS不应该出现死机等异常
	if((ret = NDK_PortOpen(PORT_NUM_USB,"115200,8,N,1"))!=NDK_OK)
	{
		cls_show_msg("line %d测试失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
#if K21_ENABLE
	sleep(3);
#endif
	cls_show_msg("请把pc关机再开机或重启后,按任意键继续");
	cls_show_msg("测试通过");
END:
	NDK_PortClose(PORT_NUM_USB);
	return;
}
//PC<--->POS
static void usb_open_comm_close(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, succconut = 0,cnt=0,nFirstRun=0;
	char recvBuf[BUFSIZE_SERIAL+1] = {0};
	char CntStr[8] = DEFAULT_CNT_STR,sendBuf[BUFSIZE_SERIAL+1]={0};
	uint unLen=0;
	
	/*process body*/	
	cls_printf("默认测试次数:\n");
	//为了兼容无键盘低端产品ME11 ME15使用封装函数 20141031 jiangym modify
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);

	//测试前置：
	NDK_PortClose(PORT_NUM_USB);
	
	//生产测试数据包并发送给后台
	for(i=0;i<BUFSIZE_SERIAL;i++)
		sendBuf[i] = rand()%256;

	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");	
	
	//读写压力测试
	i=0;
	while(1)
	{
		cls_printf("USB串口读写压力中\n总共:%d次\n已进行:%d次\n",cnt,i);
		if(i++>=cnt)
			break;
		
		if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:打开USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB);
			return;
		}
		if(nFirstRun==0)
			cls_show_msg("请把AccessPort工具打开连接串口,任意键继续");
		else
			cls_show_msg("请把AccessPort打开串口并发送,任意键继续");
		if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
		{				
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		
		if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		if(!nFirstRun)
		{
			nFirstRun++;
			if(cls_show_msg("请将AccessPort接收到的数据复制到发送框并开启自动发送,确认键继续")!=ENTER)
			{
				NDK_PortClose(PORT_NUM_USB);
				return;	
			}
		}
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(recvLen != BUFSIZE_SERIAL)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, BUFSIZE_SERIAL)==ESC)
				break;
			else
				continue;
		}
		if(MemCmp(sendBuf, recvBuf,  BUFSIZE_SERIAL))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
		NDK_PortClose(PORT_NUM_USB);
		cls_show_msg("关闭AccessPort串口,任意键继续");
	}

	cls_show_msg("测试完毕,请断开AccessPort连接并停止发送,按任意键继续");
	cls_show_msg_record(FILENAME,FUNCNAME,"USB串口读写压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_USB);
	return;
}

//POS->PC
static void usb_send_data(void)
{
	int ret = -1, i = 0, cnt=0;
	char sendBuf[1024+1]={0},CntStr[8] = DEFAULT_CNT_STR;
	uint unLen=0;

	/*process body*/	
	cls_printf("默认POS发送数据到PC次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//测试前置：
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");	
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
	//生产测试数据包并发送给后台
	cls_show_msg("请把AccessPort工具打开连接上,任意键继续");
	memset(sendBuf, 0, sizeof(sendBuf));
	for(i=0;i<sizeof(sendBuf)-1;i++)
	{
		sendBuf[i] = rand()%256;
	}
	
	//写数据
	if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg1(g_keeptime,"line %d:清缓冲区失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return; 
	}
	for(i=0;i<cnt;i++)
	{
		if((ret=NDK_PortWrite(PORT_NUM_USB, 1024, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i+1, ret);
			NDK_PortClose(PORT_NUM_USB);
			return; 
		}
	}
	if(cls_show_msg("PC端接收到的数据是否为%d?是按[ENTER],否为[其它]",cnt*1024)!=ENTER)
	{
		cls_show_msg1(g_keeptime, "line %d:连续发送数据测试失败", __LINE__);
		NDK_PortClose(PORT_NUM_USB);
		return; 
	}
	cls_show_msg("测试完毕,请断开AccessPort连接,按任意键继续");
	NDK_PortClose(PORT_NUM_USB);
	return;

}
//USB从模式压力测试
void usb_press()
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	while(1)
    {
        keyin = cls_show_msg("1.USB开关压力\n"
						"2.POS<->PC 3.PC->POS\n"
						"4.异常 5.流程\n"
						"6.POS->PC");
		switch(keyin) 
		{
		case '1':
			usb_openclosepress();
			break;
		case '2':
			usb_comm_pc();
			break;
		case '3':
			usb_pctopos();
			break;
		case '4':
			usb_abnormal();
			break;
		case '5':
			usb_open_comm_close();
			break;
		case '6':
			usb_send_data();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

#if USBHOST_ENABLE
void usb_host_openclosepress()
{
	int ret = 0, i = 0, cnt = 0, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	if(cls_show_msg("请确保主POS和从POS已通过USB线连接,确认键继续")!=ENTER)
		return;	

	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg1(2,"请打开从模式串口");
	while(1)
	{
		cls_printf("主模式USB串口开关压力测试中\n总共:%d次,已执行:%d次,已成功:%d次",cnt,i,succ);
		if(i++>=cnt)//达到测试次数后退出
			break;	
		if((ret=NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开USB串口失败%d", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
#if K21_ENABLE
		sleep(3);
#endif
		if((ret=NDK_PortClose(PORT_NUM_USB_HOST))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭USB串口失败%d", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"总共进行%d次主模式开关USB压力测试,已成功%d次",i-1,succ);
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg1(2,"请关闭从模式串口");
	return;
}

void usb_host_comm_usb()
{
	int ret = -1, i = 0, recvLen = 0, succconut = 0,cnt=0,nFirstRun=0;
	char recvBuf[BUFSIZE_USBHOST+1] = {0};
	char CntStr[8] = DEFAULT_CNT_STR,sendBuf[BUFSIZE_USBHOST+1]={0};
	uint unLen=0;
	
	/*process body*/	
	cls_printf("默认测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);

	//测试前置：
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");	
	cls_printf("等待约3秒后从模式POS打开串口");
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
	{
		cls_show_msg("line %d:打开USB失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return ;
	}
	//测试数据
	for(i=0;i<BUFSIZE_USBHOST;i++)
		sendBuf[i] = rand()%256;
	//读写压力测试
	if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return;	
	}
	i=0;
	while(1)
	{
		if(!nFirstRun)
		{
			nFirstRun++;
			if(cls_show_msg("开启从模式POS的收发数据后,确认键继续")!=ENTER)
			{
				NDK_PortClose(PORT_NUM_USB_HOST);
				return;	
			}	
		}
		cls_printf("主模式USB串口读写压力中\n总共:%d次\n已进行:%d次\n",cnt,i);
		if(i++>=cnt)
			break;
		if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB_HOST,BUFSIZE_USBHOST, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, BUFSIZE_USBHOST)==ESC)
				break;
			else
				continue;
		}
		if(recvLen != BUFSIZE_USBHOST)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, BUFSIZE_USBHOST)==ESC)
				break;
			else
				continue;
		}
		if(MemCmp(sendBuf, recvBuf,  BUFSIZE_USBHOST))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
	}

	cls_show_msg("测试完毕,请关闭从模式POS串口USB,按任意键继续");
	cls_show_msg_record(FILENAME,FUNCNAME,"主模式USB串口读写压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_USB_HOST);
	return;
}

void usb_host_recv_data()//usb_postohost
{
	int ret = -1,  datalen = 0, cnt = 0,succconut = 0, i = 0, flag = 1;
	char recvBuf[BUFSIZE_USBHOST+1] = {0}, CntStr[8] = DEFAULT_CNT_STR, sendBuf[BUFSIZE_USBHOST+1] = {0}; 
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//测试前置：
	NDK_PortClose(PORT_NUM_USB_HOST);
	if(cls_show_msg("测试的从设备是pos?,是[Enter],否[其他]")==ENTER)
		flag =  1;
	else 
		flag = 0;
	if(flag)
	{
		cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
		cls_printf("等待约3秒后从模式POS打开串口");
	}
	else
		cls_show_msg("请将扫描枪(或者其他从设备)连接POS的USB串口上,任意键继续");
	//主POS打开串口
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
	{
		cls_show_msg("line %d:打开USB失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return ;
	}
	NDK_PortClrBuf(PORT_NUM_USB_HOST);
	memset(sendBuf, 0x38, sizeof(sendBuf));
	//主POS 接收压力测试
	if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return;	
	}
	i=0;
	if(flag)
		cls_show_msg("从模式POS按连续发送数据后任意键继续");
	while(1)
	{
		if(cls_show_msg1(1,"USB串口接收压力中\n总共:%d次\n已进行:%d次\nESC退出",cnt,i)==ESC)
			break;
		if(i++>=cnt)
			break;
		if(flag==0)
			cls_printf("从设备读入数据成功后等待(扫描枪扫条形码(听到滴声就扫成功))");
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, recvBuf, 15*1000, &datalen)) != NDK_OK)/*MAXWAITTIME改成15:read希望读BUFSIZE_USBHOST长度数据,实际上其它设备可能没有那么长的数据,要等待到超时返回等待时间太久故修改时间为15秒*/
		{
			NDK_PortClrBuf(PORT_NUM_USB_HOST);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, datalen, BUFSIZE_USBHOST)==ESC)
				break;
			else
				continue;
		}
		if(flag)
		{
			if(datalen != BUFSIZE_USBHOST)
			{
				NDK_PortClrBuf(PORT_NUM_USB);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
				if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, datalen, BUFSIZE_USBHOST)==ESC)
					break;
				else
					continue;
			}
			if(MemCmp(sendBuf, recvBuf,  BUFSIZE_USBHOST))
			{
				NDK_PortClrBuf(PORT_NUM_USB_HOST);//失败情况下为避免影响下一次测试，清一下接收缓冲 linwl 20140115
				if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i)==ESC)
					break;
				else
					continue;
			}
		}
		else  //上送结果到PC
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "POS收到数据:%s", recvBuf);
		succconut++;//通讯成功次数增加	
	}

	cls_show_msg("测试完毕,关闭从模式POS串口,按任意键继续");
	cls_show_msg_record(FILENAME,FUNCNAME,"主模式USB串口接收压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_USB_HOST);
	return;
}

void usb_host_abnormal()
{
	int ret = 0, recvLen = 0, i=0;
	char sendBuf[BUFSIZE_USBHOST+1]={0},recvBuf[BUFSIZE_USBHOST+1]={0};
	time_t oldtime = 0;

	NDK_PortClose(PORT_NUM_USB_HOST);
	//初始化发送缓冲
	for(i=0; i<sizeof(sendBuf)-1; i++)
		sendBuf[i]=rand()%256;
	
	//case1:在打开之后拔插USB线再进行串口读写,不出现死机等异常
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
	cls_printf("等待约3秒后从模式POS打开串口");
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
	{
		cls_show_msg("line %d测试失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return;
	}
	cls_show_msg("请先拔插下主模式USB线,按任意键继续");
	//预期写失败，但不会死机
	if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf))!=NDK_ERR_WRITE)
	{
		cls_show_msg("line %d:测试失败(%d)",__LINE__,ret);
		goto END;
	}
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_printf("主从POS重新打开串口读写应正常");
	//若重新打开主模式串口进行通讯会正常
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
	{
		cls_show_msg("line %d测试失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return;
	}
	if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)",__LINE__,ret);
		goto END;
	}
	cls_show_msg("从模式POS开启收发数据后,任意键继续");
	if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送数据失败(%d)",__LINE__,ret);
		goto END;
	}
	memset(recvBuf, 0, sizeof(recvBuf));
	if((ret=NDK_PortRead(PORT_NUM_USB_HOST,BUFSIZE_USBHOST, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
	{
		cls_show_msg("line %d:接收数据失败(%d)",__LINE__,ret);
		goto END;
	}
	if(recvLen != BUFSIZE_USBHOST)
	{
		cls_show_msg("line %d:数据收发长度不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_USBHOST);
		goto END;
	}	
	if(MemCmp(sendBuf, recvBuf,  BUFSIZE_USBHOST))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		goto END;
	}
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg("1号子用例测试通过,关闭从模式串口后,任意键继续");
	
	//case2:测试连续发送过程中拔插USB线
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
	cls_printf("等待约3秒后从模式打开串口");
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	cls_show_msg("按任意键后主POS输出数据3s后拔出USB线");
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			if(cls_show_msg("拔插线正常按[Enter],长时间卡住未退出按[其他]")==ENTER)
			{
				break;
			}
			else
			{
				cls_show_msg("line %d:测试失败,超时退出,任意键继续", __LINE__);
				goto END;
			}
		}
		ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf);
		switch(ret)
		{
			case -1:
			case -2:
			case NDK_ERR_WRITE://主设备拔线之后获取不到-100状态只能返回写失败,再插上线后重新操作串口应该能够正常工作不死机
			case NDK_ERR_TIMEOUT://由于拔线操作的时间误差,也可以能返回超时
				NDK_PortClose(PORT_NUM_USB_HOST);
				cls_show_msg("关闭从POS串口后,连接USB线后按键继续");
				cls_printf("打开从POS串口");
				if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
				{
			    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
			    	goto END;
				}
				break;	
			case NDK_OK:
				cls_printf("发送成功");
				break;
			default:
				cls_show_msg("line %d:未知的返回值(ret=%d)", __LINE__, ret);
				goto END;
		}
	}
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg("2号子用例测试通过,关闭从模式串口后,任意键继续");
	
	//case3:测试连续接收过程中拔插USB线
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
	cls_printf("从模式打开串口");
	if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	cls_show_msg("从POS开启连续发送,任意键继续后3s主POS拔USB线");
	//拔usb后等时间到提示信息出来后再插上usb
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			if(cls_show_msg("拔插线正常按[Enter],长时间卡住未退出按[其他]")==ENTER)
			{
				break;
			}
			else
			{
				cls_show_msg("line %d:测试失败,超时退出,任意键继续", __LINE__);
				goto END;
			}
		}
		memset(recvBuf, 0, sizeof(recvBuf));
		ret=NDK_PortRead(PORT_NUM_USB_HOST,BUFSIZE_USBHOST, recvBuf, 2*1000, &recvLen);
		switch(ret)
		{
			case -1:
			case -2:
			case NDK_ERR_READ://主设备拔线之后获取不到-100状态只能返回读失败,再插上线后重新操作串口应该能够正常工作不死机
			case NDK_ERR_TIMEOUT://由于拔线操作的时间误差,也可以能返回超时
				NDK_PortClose(PORT_NUM_USB_HOST);
				cls_show_msg("ESC停止发送,关闭串口后连接USB");
				cls_printf("打开从POS串口");
				if((ret = NDK_PortOpen(PORT_NUM_USB_HOST,"115200,8,N,1,20"))!=NDK_OK)
				{
			    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
			    	goto END;
				}
				cls_show_msg("从模式POS开启连续发送,任意键继续");
				break;	
			case NDK_OK:
				cls_printf("接收成功");
				break;
			default:
				cls_show_msg("line %d:未知的返回值(ret=%d)", __LINE__, ret);
				goto END;
		}
	}
	NDK_PortClose(PORT_NUM_USB_HOST);
	cls_show_msg("3号子用例测试通过,关闭从模式串口后,任意键继续");
	
END:
	NDK_PortClose(PORT_NUM_USB_HOST);
	return;
}

//USB主模式压力测试
void usb_host_press()
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	while(1)
    {
        keyin = cls_show_msg("1.USB开关压力\n"
						"2.主POS<->从POS\n"
						"3.从设备->主POS\n"
						"4.异常 ");
		switch(keyin) 
		{
		case '1':
			usb_host_openclosepress();
			break;
		case '2':
			usb_host_comm_usb();
			break;
		case '3':
			usb_host_recv_data();
			break;
		case '4':
			usb_host_abnormal();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

void usb_host_U()
{
	int ret = -1, recvLen = 0, i=0;
	char recvBuf[BUFSIZE_USBHOST+1] = {0};
	char sendBuf[BUFSIZE_USBHOST+1] = {0};

	while(1)
	{
		if(ESC==cls_show_msg1(2, "正在进行第%d次主模式与U盘的切换测试,按ESC退出", i+1))
			break;
		if( i++ > 3 )
			break;
		//测试数据
		memset(sendBuf, 0x38, sizeof(sendBuf));
		//usb 主模式打开 读写 关闭 流程
		NDK_PortClose(PORT_NUM_USB_HOST);
		cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");	
		cls_printf("从模式POS打开串口");
		if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return ;
		}
		if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
		{				
			cls_show_msg1(g_keeptime,"line %d:清缓冲区失败(%d)",__LINE__,ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		cls_show_msg("从模式POS开启收发数据后,任意键继续");
		if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:写USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB_HOST,BUFSIZE_USBHOST, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:读USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		if(recvLen != BUFSIZE_USBHOST)
		{
			cls_show_msg1(g_keeptime,"line %d:数据收发长度不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_USBHOST);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		if(MemCmp(sendBuf, recvBuf, BUFSIZE_USBHOST))
		{
			cls_show_msg1(g_keeptime,"line %d:校验数据失败", __LINE__);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		NDK_PortClose(PORT_NUM_USB_HOST);
		
		//U盘的操作
		cls_show_msg("主模式测试完成，请拔掉主POS的USB线插上U盘，并关闭从模式POS串口后按任意键继续");
		//NDK_DiskClose(UDISK);
		if((ret=systest_SDCard(UDISK))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:U盘测试失败(%d)", __LINE__, ret);
			continue;
		}		
	}
}

void host_usb_slave()
{
	int ret = -1, recvLen = 0,  i=0;
	char recvBuf[BUFSIZE_USBHOST+1] = {0};
	char sendBuf[BUFSIZE_USBHOST+1] = {0};
	
	while(1)
	{
		if(ESC==cls_show_msg1(2, "正在进行第%d次主从模式切换测试,按ESC退出", i+1))
			break;
		if( i++ > 3 )
			break;
		//测试数据
		memset(sendBuf, 0x38, sizeof(sendBuf));
		//usb主模式的操作	
		NDK_PortClose(PORT_NUM_USB_HOST);
		cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
		cls_printf("等待约3秒后从模式POS打开串口");
		if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return ;
		}
		if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
		{				
			cls_show_msg1(g_keeptime,"line %d:清缓冲区失败(%d)",__LINE__,ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		cls_show_msg("从模式POS开启收发数据后,任意键继续");
		if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:写USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB_HOST, BUFSIZE_USBHOST, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:读USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		if(recvLen != BUFSIZE_USBHOST)
		{
			cls_show_msg1(g_keeptime,"line %d:数据收发长度不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_USBHOST);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		if(MemCmp(sendBuf, recvBuf, BUFSIZE_USBHOST))
		{
			cls_show_msg1(g_keeptime,"line %d:校验数据失败", __LINE__);
			NDK_PortClose(PORT_NUM_USB_HOST);
			return;
		}
		NDK_PortClose(PORT_NUM_USB_HOST);
		cls_show_msg("主模式测试完成，请关闭从模式POS串口后按任意键继续");
	
		//usb从模式的操作
		cls_show_msg("请将当主模式的POS和PC通过USB线连接,任意键继续");	
		NDK_PortClose(PORT_NUM_USB);
		if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB);
			return ;
		}
		cls_show_msg("打开PC端AccessPort工具,并打开串口,任意键继续");
		memset(sendBuf, 0x38, sizeof(sendBuf));
		if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_USBHOST, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:写USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB);
			return;
		}
		cls_show_msg("请将AccessPort接收到的数据复制到发送框并发送,任意键继续");	
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB, BUFSIZE_USBHOST, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:读USB失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_USB);
			return;
		}
		if(recvLen != BUFSIZE_USBHOST)
		{
			cls_show_msg1(g_keeptime,"line %d:数据收发长度不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_USBHOST);
			NDK_PortClose(PORT_NUM_USB);
			return;
		}
		if(MemCmp(sendBuf, recvBuf, BUFSIZE_USBHOST))
		{
			cls_show_msg1(g_keeptime,"line %d:校验数据失败", __LINE__);
			NDK_PortClose(PORT_NUM_USB);
			return;
		}
		NDK_PortClose(PORT_NUM_USB);
		cls_show_msg("从模式测试完成，请关闭PC工具串口后按任意键继续");	
	}
}

void usb_change_press()
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	while(1)
    {
        keyin = cls_show_msg("1.U盘和USB主\n"
						"2.USB主和USB从\n");
		switch(keyin) 
		{
		case '1':
			usb_host_U();
			break;
		case '2':
			host_usb_slave();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}
#else
void usb_host_openclosepress(){return;}
void usb_host_comm_usb(){return;}
void usb_host_recv_data(){return;}
void usb_host_abnormal(){return;}
void usb_host_press(){cls_show_msg1(5,"本设备不支持USB主模式,请人工判断是否支持");return;}
void usb_host_U(){return;}
void host_usb_slave(){return;}
void usb_change_press(){cls_show_msg1(5,"本设备不支持USB主模式,请人工判断是否支持");return;}
#endif

void usb_client()
{
	/*private & local definition*/
	int keyin = 0, ret = 0;
	char sendbuf[BUFSIZE_USBHOST]={0}, recbuf[BUFSIZE_USBHOST]={0} ;
	int len = 0;

	/*process body*/
	while(1)
    {
        keyin = cls_show_msg("1.打开USB串口\n"
						"2.收发数据\n"
						"3.连续发数据\n"
						"4.关闭USB串口");
		switch(keyin) 
		{
		case '1':
			if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(ret:%d)", __LINE__, TESTITEM, ret);
				NDK_PortClose(PORT_NUM_USB);
				return;
			}
			cls_show_msg1(1,"打开USB串口成功");
			break;
		case '2':	
			memset(recbuf, 0, sizeof(recbuf));
			if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
			{				
				cls_show_msg1(g_keeptime,"line %d:清缓冲区失败(%d)",__LINE__,ret);
				NDK_PortClose(PORT_NUM_USB);
				return;
			}
			cls_printf("数据收发中.");
			while(1)
			{			
				//收数据
				ret = NDK_PortRead(PORT_NUM_USB,sizeof(recbuf),recbuf,5000,&len);
				if(ret==NDK_ERR_TIMEOUT || cls_show_msg1(1,"数据收发中.ESC退出")==ESC)
					break;
				else if(ret==NDK_OK)
				{
					//发数据
					if((ret=NDK_PortWrite(PORT_NUM_USB, len, recbuf)) != NDK_OK)	/*往串口发送测试数据*/
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试失败(ret:%d)", __LINE__, TESTITEM, ret);
						NDK_PortClose(PORT_NUM_USB);
						return; 	
					}
				}
				else
				{
					if(ret!=-100)
						cls_show_msg1(g_keeptime,"line %d:%s测试失败(ret:%d)", __LINE__, TESTITEM, ret);
					NDK_PortClose(PORT_NUM_USB);
					return;
				}						
			}
			break;
		case '3':	
			memset(sendbuf, 0x38, sizeof(sendbuf));
			//连续发数据
			while(1)
			{
				if(cls_show_msg1(1,"连续发送数据中,ESC退出")==ESC)
					break;
				ret = NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf), sendbuf);/*往串口发送测试数据*/
				if(ret == NDK_ERR_USB_LINE_UNCONNECT)
				{
					cls_show_msg("USB线被拔下,任意键继续");
					break;
				}
				else if(ret== NDK_OK)	
					continue;
				else
				{
					cls_show_msg1(g_keeptime,"line %d:%s测试失败(ret:%d)", __LINE__, TESTITEM, ret);
					NDK_PortClose(PORT_NUM_USB);
					return;		
				}
			}
			break;
		case '4':
			if((ret=NDK_PortClose(PORT_NUM_USB))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				return;
			}
			cls_show_msg1(1,"关闭USB串口成功");
			break;
		case ESC:
			NDK_PortClose(PORT_NUM_USB);
			return;
			break;	
		default:
			continue;
			break;
		}
	}
	return;
}
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		chensj		   20130624	 		created
					jiangym 		 20140220			added
*****************************************************************/
void systest32(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	//EM_PORT_NUM usbport = PORT_NUM_USB;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);	
	while(1)
    {
        keyin = cls_show_msg("1.USB从模式测试\n"
							"2.USB主模式测试\n"
							"3.USB切换测试\n"
							"4.客户端操作");
		switch(keyin) 
		{
		case '1':
			usb_press();
			break;
		case '2':
			usb_host_press();
			break;
		case '3':
			usb_change_press();
			break;
		case '4':
			usb_client();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}


