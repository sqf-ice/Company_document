/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: port模块
* file name		: port10.c
* Author 			: 
* version			: 
* DATE			:
* directory 		: 
* description		: 测试HID串口功能
* related document	: SDK.chm
*
************************************************************************/
#include "mode.h"
#include<stdio.h>

#define TESTAPI	"HID串口"
#define MAXWAITTIME	10//30
#define SNDCNT			(71)
#define SNDCNT1			(128)
#define SNDTIMES		(40)//K21 每次发送1K ，所以要发送40次

/****************************************************************
* function name 	 	: void port6(void)
* functional description 	:USB串口功能
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	         sull        20180910    created
*****************************************************************/
/*
* 这边测试需要使用rs232串口(com1)上送来测试
*/
void port10(void)
{	
	int ret = 0;
	char sendbuf[BUFSIZE_SERIAL]={0}, recbuf[BUFSIZE_SERIAL]={0};
	int len = 0,j = 0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";
	
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:关闭HID串口
	if((ret=NDK_PortClose(PORT_NUM_HID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:pos与PC连接时,未打开就进行发送，应该返回失败
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_HID, sizeof(sendbuf),sendbuf))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:支持上传大写、小写、数字、符号
	if ((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(sendbuf,0,sizeof(sendbuf));
	sprintf(sendbuf,"abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/");
	cls_show_msg1(5,"请打开任一txt文档，将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC", sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_HID,sizeof(sendbuf),sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
	if(cls_show_msg("完毕!txt文档大小为%dB,且内容为%s?是[ENTER],否[其它]",sizeof(sendbuf),sendbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:要求打开设备后，进入休眠再唤醒（只需要正确调用休眠接口，不管是否进行了实质休眠），应仍能正常地进行数据收发
	if ((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysGoSuspend();//（只需要正确调用休眠接口，不管是否进行了实质休眠）,根据应用最新要求应该不会进入休眠
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [任意]键被后面的语句识别到
	cls_show_msg1(5,"清空txt数据，将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_HID, sizeof(sendbuf), sendbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	if(cls_show_msg("完毕!txt文档大小为%dB,且内容都为8?是[ENTER],否[其它]",sizeof(sendbuf))!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case4:HID串口只能写不能读，read应该返回不支持
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_HID, sizeof(recbuf), recbuf, MAXWAITTIME*1000, &len)) != NDK_ERR_NOT_SUPPORT)	
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_HID);
	cls_show_msg1(2,"子用例测试通过");

	//case5:pos与PC连接,打开U口,进行正常发送4K数据
	if((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(5,"清空文档数据,将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_HID, sizeof(sendbuf), sendbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	if(cls_show_msg("完毕!txt文档大小为%dB,且内容都为8?是[ENTER],否[其它]",sizeof(sendbuf))!= ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case6:正常发送非整8字节数据
	cls_show_msg1(5,"txt清空数据,将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC", SNDCNT);
	cls_printf("POS--->PC(%dB)\n", SNDCNT);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_HID, SNDCNT, sendbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	if(cls_show_msg("完毕!txt文档大小为%dB,且内容都为8?是[ENTER],否[其它]",SNDCNT)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	

	//case7:发送64整数倍但又不超过4K的数据
	cls_show_msg1(5,"txt清空数据,将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC", SNDCNT1);
	cls_printf("POS--->PC(%dB)\n", SNDCNT1);
	memset(sendbuf, 0x38, sizeof(sendbuf));
	if((ret=NDK_PortWrite(PORT_NUM_HID, SNDCNT1, sendbuf)) != NDK_OK)	
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	if(cls_show_msg("完毕!txt文档大小为%dB,且内容都为8?是[ENTER],否[其它]",SNDCNT1)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
		
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=NDK_PortRead(PORT_NUM_HID, SNDCNT1, recbuf, MAXWAITTIME*1000, &len)) != NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:复位测试:HID串口不支持NDK_PortClrBuf
	if((ret=NDK_PortClrBuf(PORT_NUM_HID))!=NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);				
		GOTOERR;
	}

	//case9:测试写完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret = NDK_PortWrite(PORT_NUM_HID, sizeof(sendbuf), sendbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortClose(PORT_NUM_HID)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case10:重新打开串口进行数据发送应该成功	
	if((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(5,"txt清空数据,将鼠标移动到该文档,测试过程中请不要移开鼠标,5秒后POS将输出%dB数据到PC和串口", sizeof(sendbuf));
	cls_printf("POS--->PC(%dB)\n", sizeof(sendbuf));
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j] = str1[rand()%(sizeof(str1)-1)];
	if((ret=NDK_PortWrite(PORT_NUM_HID, sizeof(sendbuf), sendbuf)) != NDK_OK)	
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	NDK_PortWrite(PORT_NUM_COM1, sizeof(sendbuf), sendbuf);
	if(cls_show_msg( "完毕!txt文档大小为%dB,且内容与发送到串口上的内容一致?是[ENTER],否[其它]",sizeof(sendbuf))!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);	
		
ERR:
	NDK_PortClose(PORT_NUM_HID);
	return;
}


