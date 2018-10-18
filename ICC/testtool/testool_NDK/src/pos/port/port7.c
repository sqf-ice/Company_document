/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: port模块
* file name		      : port7.c
* Author 			: 
* version			: 
* DATE				: 20140220
* directory 			: 
* description			: 测试USB串口功能
* related document	: SDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"USB高速串口"
#define MAXWAITTIME	10//30
#define SNDCNT			(71)
#define SNDCNT1			(128)

/****************************************************************
* function name 	 	: void port7(void)
* functional description 	:USB串口功能
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	 	jiangym 		20140221   created
*****************************************************************/
void host_usb_test(void)
{
	int ret = 0;
	char sendbuf[BUFSIZE_BT]={0}, recbuf[BUFSIZE_BT]={0};
	int len = 0;
	//time_t oldtime=0;
	
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:关闭Usb主串口
	if((ret=NDK_PortClose(PORT_NUM_USB_HOST))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:主pos与从POS连接时,主POS未打开串口就进行发送，应该返回NDK_ERR_OPEN_DEV
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, sizeof(sendbuf),sendbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:主pos与从POS连接时,主POS未打开串口就进行接收，应该返回NDK_ERR_OPEN_DEV
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf, MAXWAITTIME*1000,&len))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"子用例测试通过");
	
	//case3:测试主模式打开后,从模式未能在超时时间内连接成功,应该要返回超时
	cls_show_msg("请将主POS与从POS断开USB连接,任意键继续");//不是第一测试会加载驱动，因此会成功，需要拔掉线
	if((ret=NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,2")) != NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:主pos与从POS连接,打开串口,进行正常收发4K数据
	cls_show_msg("请将主POS和从POS通过USB线连接,打开从POS串口,任意键继续");
	if((ret=NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("开启从POS的收发数据后,主POS按任意键将输出%dkB数据到从POS", sizeof(sendbuf)/1024);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, sizeof(sendbuf), sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	cls_show_msg1(2,"主pos将接收到从POS的数据");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST, sizeof(recbuf), recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"接收完毕");
	if((len!=sizeof(sendbuf))||memcmp(sendbuf, recbuf, len))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, sizeof(sendbuf));
		GOTOERR;
	}
	
	//case5:主pos与从POS连接,打开串口,进行正常收发71字节数据
	cls_show_msg("主POS按任意键将输出%dB数据到从POS", SNDCNT);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, SNDCNT, sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}	
	cls_show_msg1(2,"主pos将接收到从POS的数据");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_USB_HOST, SNDCNT, recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
   	}
	cls_show_msg1(2,"接收完毕");
	if((len!=SNDCNT)||memcmp(sendbuf, recbuf, SNDCNT))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, SNDCNT);
		GOTOERR;
	}
	
	//case6:主pos与从POS连接,打开串口,进行正常收发128字节数据
	cls_show_msg("主POS按任意键将输出%dB数据到从POS", SNDCNT1);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, SNDCNT1, sendbuf)) != NDK_OK)	/*往串口发送测试数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	cls_show_msg1(2,"主pos将接收到从POS的数据");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_USB_HOST, SNDCNT1, recbuf, MAXWAITTIME*1000, &len)) != NDK_OK)	/*读取串口数据*/
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"接收完毕");
	if((len!=SNDCNT1)||memcmp(sendbuf, recbuf, SNDCNT1))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, len, SNDCNT1);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);	
		
ERR:
	NDK_PortClose(PORT_NUM_USB_HOST);
	return;
}

void slave_usb()
{
	/*private & local definition*/
	int keyin = 0, ret = 0;
	char recbuf[BUFSIZE_BT]={0} ;
	int len = 0, reclen=0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTAPI);	
	
	while(1)
    {
        keyin = cls_show_msg("1.打开USB串口\n"
							"2.收发数据\n"
							"3.关闭USB串口");
		switch(keyin) 
		{
		case '1':
			if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
				NDK_PortClose(PORT_NUM_USB);
				return;
			}
			break;
		case '2':	
			memset(recbuf, 0, sizeof(recbuf));
			if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	/*清接收缓冲*/
			{				
				cls_show_msg1(5,"line %d:清缓冲区失败(%d)",__LINE__,ret);
				NDK_PortClose(PORT_NUM_USB);
				return;
			}
			while(1)
			{	
				NDK_PortReadLen(PORT_NUM_USB,&reclen);	/*取缓冲区字节数*/
				if(reclen>0)
				{	
					//收数据
					cls_printf("接收%d字节数据中",reclen);
					if((ret = NDK_PortRead(PORT_NUM_USB,reclen,recbuf,20000,&len)) != NDK_OK)
					{
						send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
						NDK_PortClose(PORT_NUM_USB);
						return;
					}
					
					//发数据
					cls_printf("发送%d字节数据中",len);
					if((ret=NDK_PortWrite(PORT_NUM_USB, len, recbuf)) != NDK_OK)	/*往串口发送测试数据*/
					{
						send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
						NDK_PortClose(PORT_NUM_USB);
						return;		
					}
				}
				if(cls_show_msg1(2,"等待数据收发中,ESC退出")==ESC)
					break;
			}
			break;
		case '3':
			if((ret=NDK_PortClose(PORT_NUM_USB))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;
			}
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

void port7(void)
{	
	/*private & local definition*/
	int keyin = 0;
	//EM_PORT_NUM usbport = PORT_NUM_USB;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTAPI);	
	while(1)
    {
        keyin = cls_show_msg("1.USB主模式测试\n"
							"2.USB从模式操作");
		switch(keyin) 
		{
		case '1':
			host_usb_test();
			break;
		case '2':
			slave_usb();
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


