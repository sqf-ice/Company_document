/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: port模块
* file name		: port6.c
* Author 			: linrq
* version			: 
* DATE			: 20121022
* directory 		: 
* description		: 测试USB串口功能
* related document	: SDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"USB高速串口"
#define MAXWAITTIME	10//30
#define SNDCNT			(71)
#define SNDCNT1			(128)

#if !K21_ENABLE
#define SNDTIMES		(10)//5892 每次发送4K ，所以要发送10次
#else
#define SNDTIMES		(40)//K21 每次发送1K ，所以要发送40次
#endif

/****************************************************************
* function name 	 	: void port6(void)
* functional description 	:USB串口功能
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  linrq      20121022	created
*****************************************************************/
void port6(void)
{	
	int ret = 0;
	char sendbuf[BUFSIZE_SERIAL]={0}, recbuf[BUFSIZE_SERIAL]={0};
	int len = 0,j = 0;
	time_t oldtime=0;
	
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:关闭Usb串口
	if((ret=NDK_PortClose(PORT_NUM_USB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:pos与PC连接时,未打开就进行发送，应该返回NDK_ERR_OPEN_DEV
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf),sendbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:pos与PC连接时,未打开就进行接收，应该返回NDK_ERR_OPEN_DEV
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB,sizeof(recbuf),recbuf, MAXWAITTIME*1000,&len))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"子用例测试通过");

#if !K21_ENABLE//根据会议讨论结果 K21不支持open判断USB未插线
	//case3:pos与PC未连接时,打开U口应失败返回NDK_ERR_USB_LINE_UNCONNECT
	cls_show_msg("请拔USB线,任意键继续");
	if((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_ERR_USB_LINE_UNCONNECT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");
#endif
	
	//case4:pos与pc连接后接收超时NDK_ERR_TIMEOUT
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE
	sleep(3);
#endif
	if((ret = NDK_PortRead(PORT_NUM_USB, sizeof(recbuf), recbuf, 1*1000, &len))!=NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");

	//case5:拔掉USB线后发送数据时,应失败,返回NDK_ERR_USB_LINE_UNCONNECT
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请拔USB线,按任意键继续");
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB,sizeof(sendbuf),sendbuf))!=NDK_ERR_USB_LINE_UNCONNECT)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");

	//case6:拔掉USB线后接收数据时,应失败,返回NDK_ERR_USB_LINE_UNCONNECT
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请拔USB线,按任意键继续");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_ERR_USB_LINE_UNCONNECT)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");
	
	//case14:测试USB打开后，PC端工具未打开，发送数据应该会select失败返回NDK_ERR
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if K21_ENABLE
	sleep(3);//ME30 OPEN后PC需要3秒时间才能完成连接
#endif
	memset(sendbuf, 0x38, sizeof(sendbuf));
	for(j = 0;j<SNDTIMES;j++)//根据国锋的解释 USB连线后即使PC端工具没有打开，但PC端可能有USB串口缓冲，所以前几次写可能会成功，故修改用例多次写，只要其中一次写失败则表示用例通过
	{
		if((ret = NDK_PortWrite(PORT_NUM_USB,sizeof(sendbuf),sendbuf))==NDK_ERR)
			break;
	}
	if(j==SNDTIMES)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
/*	if((ret = NDK_PortWrite(PORT_NUM_USB,sizeof(sendbuf),sendbuf))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}*/
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");
	
	//case7:打开usb设备，在发送数据时拔下，应返回失败NDK_ERR_USB_LINE_UNCONNECT
	cls_show_msg("请将POS和PC通过USB线连接,并开启PC端的accessport工具,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
	    send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
	    GOTOERR;
	}
	cls_show_msg("PC打开串口,按任意键POS将输出数据3s后拔下USB线");
	memset(sendbuf, 0x38, sizeof(sendbuf));
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
		ret=NDK_PortWrite(PORT_NUM_USB,SNDCNT1,sendbuf);
		switch(ret)
		{
			case -1://低端可能返回-1 20150906
			case NDK_ERR_USB_LINE_UNCONNECT:
				break;	
			case NDK_OK:
				cls_printf("发送成功");
				break;
			default:
				send_result("line %d:%s未知的返回值(ret=%d)", __LINE__, TESTAPI, ret);
				goto ERR;
		}
		if( ret== NDK_ERR_USB_LINE_UNCONNECT||ret==-1)
			break;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");

	//case8:打开usb设备，在接收数据时拔下，应返回失败NDK_ERR_USB_LINE_UNCONNECT
	cls_show_msg("请将POS和PC通过USB线连接,关闭PC串口后重启PC端的accessport工具,任意键继续");//备注:K21需要先按键才能操作PC端
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("PC打开串口,按任意键继续");
	cls_printf("请拔USB线");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_ERR_USB_LINE_UNCONNECT)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");

#if !K21_ENABLE
	//case9:要求打开设备后，进入休眠再唤醒（只需要正确调用休眠接口，不管是否进行了实质休眠），应仍能正常地进行数据收发
	cls_show_msg("请将POS和PC通过USB线连接,并重启PC端的accessport工具,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysGoSuspend();//（只需要正确调用休眠接口，不管是否进行了实质休眠）,根据应用最新要求应该不会进入休眠
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [任意]键被后面的语句识别到
	cls_show_msg("PC打开串口,POS按任意键将输出%dB数据到PC", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf), sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	show_msg1(3, "完毕!");

	cls_show_msg("PC开启[发送],pos按任意键将在%ds内接收PC的数据", MAXWAITTIME);
	cls_printf("POS<---PC(%dB)\n", sizeof(sendbuf));
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB, sizeof(recbuf), recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	show_msg1(3, "完毕!");
	if((len!=sizeof(sendbuf))||memcmp(sendbuf, recbuf, len))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, sizeof(sendbuf));
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	cls_show_msg1(2,"子用例测试通过");
#endif

	//case10:pos与PC连接,打开U口,进行正常收发4K数据
	cls_show_msg("请将POS和PC通过USB线连接,并重启PC端的accessport工具,任意键继续");
	if((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("PC打开串口,POS按任意键将输出%dB数据到PC", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf), sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	show_msg1(3, "完毕!");
	
	cls_show_msg("PC开启[发送],pos按任意键将在%ds内接收PC的数据", MAXWAITTIME);
	cls_printf("POS<---PC(%dB)\n", sizeof(sendbuf));
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB, sizeof(recbuf), recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	show_msg1(3, "完毕!");
	if((len!=sizeof(sendbuf))||memcmp(sendbuf, recbuf, len))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, sizeof(sendbuf));
		GOTOERR;
	}
	
	//case11:正常收发非整8字节数据
	cls_show_msg("PC清空数据,POS按任意键将输出%dB数据到PC", SNDCNT);
	cls_printf("POS--->PC(%dB)\n", SNDCNT);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB, SNDCNT, sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	show_msg1(3, "完毕!");
	
	cls_show_msg("PC开启[发送],POS按任意键将在%ds内接收PC的数据", MAXWAITTIME);
	cls_printf("POS<---PC(%dB)\n", SNDCNT);
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_USB, SNDCNT, recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
   	}
	if((len!=SNDCNT)||memcmp(sendbuf, recbuf, SNDCNT))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, SNDCNT);
		GOTOERR;
	}

	//case12:发送64整数倍但又不超过4K的数据
	cls_show_msg("PC清空数据,POS按任意键将输出%dB数据到PC", SNDCNT1);
	cls_printf("POS--->PC(%dB)\n", SNDCNT1);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB, SNDCNT1, sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	show_msg1(3, "完毕!");
	
	cls_show_msg("PC开启[发送],POS按任意键将在%ds内接收PC的数据", MAXWAITTIME);
	cls_printf("POS<---PC(%dB)\n", SNDCNT1);
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_USB, SNDCNT1, recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	show_msg1(3, "完毕!");
	if((len!=SNDCNT1)||memcmp(sendbuf, recbuf, SNDCNT1))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, SNDCNT1);
		GOTOERR;
	}
	
	//case13:复位测试
	cls_show_msg("PC:确保发送框有数据并开启[发送]发送一次数据后[停止],按任意键继续");
	sleep(1);
	if((ret=NDK_PortClrBuf(PORT_NUM_USB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);				
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_USB, sizeof(recbuf)-1, recbuf, MAXWAITTIME*1000, &len)) != NDK_ERR_TIMEOUT)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((len!=0))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
		GOTOERR;
	}

	//case14:测试写完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret = NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf), sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortClose(PORT_NUM_USB)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case15:重新打开串口进行数据收发应该成功	
	cls_show_msg("请重启PC端的accessport工具,任意键继续");
	if((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("PC打开串口,POS按任意键将输出%dB数据到PC", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_PortWrite(PORT_NUM_USB, sizeof(sendbuf), sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	show_msg1(3, "完毕!");
	
	cls_show_msg("PC开启[发送],pos按任意键将在%ds内接收PC的数据", MAXWAITTIME);
	cls_printf("POS<---PC(%dB)\n", sizeof(sendbuf));
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB, sizeof(recbuf), recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	show_msg1(3, "完毕!");
	if((len!=sizeof(sendbuf))||memcmp(sendbuf, recbuf, len))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, sizeof(sendbuf));
		goto ERR;
	}
	//测试结束
	send_result("%s测试通过", TESTAPI);	
		
ERR:
	NDK_PortClose(PORT_NUM_USB);
	return;
}

