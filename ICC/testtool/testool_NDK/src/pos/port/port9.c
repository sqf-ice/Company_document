/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port9.c
* Author 			: 
* version			: 
* DATE				:
* directory 	: 
* description	: 测试HID串口
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include<stdio.h>

/*---------------constants/macro definition---------------------*/
#define		TESTAPI		"串口上送扫码"
#define		MAXTIME 30//接收等待时间

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
*			  			    sull              20180910		created
*****************************************************************/
/*
* 这边测试需要使用rs232串口(com1)上送来测试
*/
void port9(void)
{
	//以下为局部变量
	int ret = 0, cnt = 20, bak = 0, plen = 0, succ = 0;
	char wbuf[500] = {0}, str[31] = {0};
	uint  a = 0, a1 = 0;
	float ScanTime0 = 0, ScanTime = 0;
	char pvalue[500] = {0};
	time_t oldtime = 0;
	
	//以下为函数体
	cls_printf("%s测试中...", TESTAPI);
	if(cls_show_msg1(MAXTIME, "按确认键进入HID串口扫码测试,否则按其它键")==ENTER)
	{

		//测试前置:打开串口		
		if((ret = NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case1:进行连续20次扫码，并用HID串口上送到txt上
		cls_show_msg1(5,"请打开一txt，并把鼠标移到该位置，5s后开始扫码，扫码结果将上送到txt");
		bak = cnt;
		while(cnt--)
		{		
			plen=0;
			ScanTime0 = 0;
			memset(pvalue,0,sizeof(pvalue));
			NDK_SysReadWatch(&a1);
			if((ret=NDK_ScanStart()) != NDK_OK )
			{
				NDK_ScanStop();
				cls_show_msg1(3, "line %d:第%d次摄像头开启扫描失败(%d)", __LINE__, bak-cnt, ret);
				continue;
			}   
			oldtime = time(NULL);
			while(1)
			{	
				if((ret=NDK_ScanState(pvalue, &plen)) == NDK_OK)
				{
					NDK_SysReadWatch(&a);
					ScanTime0 = (a - a1)/(1000*1.0);
					ScanTime += ScanTime0;
					succ++;
					memset(str,0,sizeof(str));
					ftos(ScanTime0,str);
					memset(wbuf, 0, sizeof(wbuf));
					sprintf(wbuf, "%d:data:%s;   len:%d;    time:%ss\r\n", bak-cnt, pvalue, plen, str);
					NDK_PortWrite(PORT_NUM_HID, sizeof(wbuf), wbuf);
					break;
				}
				NDK_SysMsDelay(10);
				if((time(NULL) - oldtime)>MAXTIME)
				{
					NDK_ScanStop();
					cls_show_msg1(3,"line %d:扫描超时", __LINE__);
					break;
				}
			}
			if((ret = NDK_ScanStop())!=NDK_OK)
			{
				cls_show_msg1(3,"line %d:关闭摄像头失败(%d)", __LINE__, ret);
				continue;
			}
		}
		send_result("通讯总次数%d,成功次数%d\n", bak, succ);
		memset(str,0,sizeof(str));
		ftos(ScanTime/succ,str);
		send_result("扫码性能:%ss/次\n", str);
		NDK_PortClose(PORT_NUM_HID);
	}

	if(cls_show_msg1(MAXTIME, "按确认键进入USB串口扫码测试,否则按其它键")==ENTER)
	{

		//测试前置:打开串口	
		NDK_PortClose(PORT_NUM_USB);
		if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cnt = 20;
		succ = 0;
		ScanTime = 0;

		//case2:进行连续20次扫码，并用USB 串口上送到电脑上
		cls_show_msg("请打开accessport并连接,按任意键开始扫描，扫码结果将上送到accessport");
		bak = cnt;
		while(cnt--)
		{		
			plen=0;
			ScanTime0 = 0;
			memset(pvalue,0,sizeof(pvalue));
			NDK_SysReadWatch(&a1);
			if((ret=NDK_ScanStart()) != NDK_OK )
			{
				NDK_ScanStop();
				cls_show_msg1(3, "line %d:第%d次摄像头开启扫描失败(%d)", __LINE__, bak-cnt, ret);
				continue;
			}   
			oldtime = time(NULL);
			while(1)
			{	
				if((ret=NDK_ScanState(pvalue, &plen)) == NDK_OK)
				{
					NDK_SysReadWatch(&a);
					ScanTime0 = (a - a1)/(1000*1.0);
					ScanTime += ScanTime0;
					succ++;
					memset(str,0,sizeof(str));
					ftos(ScanTime0,str);
					memset(wbuf, 0, sizeof(wbuf));
					sprintf(wbuf, "%d:data:%s;   len:%d;    time:%ss\r\n", bak-cnt, pvalue, plen, str);
					NDK_PortWrite(PORT_NUM_USB, sizeof(wbuf), wbuf);
					break;
				}
				NDK_SysMsDelay(10);
				if((time(NULL) - oldtime)>MAXTIME)
				{
					NDK_ScanStop();
					cls_show_msg1(3,"line %d:扫描超时", __LINE__);
					break;
				}
			}
			if((ret = NDK_ScanStop())!=NDK_OK)
			{
				cls_show_msg1(3,"line %d:关闭摄像头失败(%d)", __LINE__, ret);
				continue;
			}
		}
		send_result("通讯总次数%d,成功次数%d\n", bak, succ);
		memset(str,0,sizeof(str));
		ftos(ScanTime/succ,str);
		send_result("扫码性能:%ss/次\n", str);
		NDK_PortClose(PORT_NUM_USB);
	}

	if(cls_show_msg1(MAXTIME, "按确认键进入COM1串口扫码测试,否则按其它键")==ENTER)
	{

		//测试前置:打开串口
		NDK_PortClose(PORT_NUM_COM1);
		if((ret = NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1")) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cnt = 20;
		succ = 0;
		ScanTime = 0;

		//case3:进行连续20次扫码，并用COM1串口上送到电脑上
		cls_show_msg("请打开SecuCRT并连接,按任意键开始扫描，扫码结果将上送到SecureCRT");
		bak = cnt;
		while(cnt--)
		{		
			plen=0;
			ScanTime0 = 0;
			memset(pvalue,0,sizeof(pvalue));
			NDK_SysReadWatch(&a1);
			if((ret=NDK_ScanStart()) != NDK_OK )
			{
				NDK_ScanStop();
				cls_show_msg1(3, "line %d:第%d次摄像头开启扫描失败(%d)", __LINE__, bak-cnt, ret);
				continue;
			}   
			oldtime = time(NULL);
			while(1)
			{	
				if((ret=NDK_ScanState(pvalue, &plen)) == NDK_OK)
				{
					NDK_SysReadWatch(&a);
					ScanTime0 = (a - a1)/(1000*1.0);
					ScanTime += ScanTime0;
					succ++;
					memset(str,0,sizeof(str));
					ftos(ScanTime0,str);
					memset(wbuf, 0, sizeof(wbuf));
					sprintf(wbuf, "%d:data:%s;   len:%d;    time:%ss\r\n", bak-cnt, pvalue, plen, str);
					NDK_PortWrite(PORT_NUM_COM1, sizeof(wbuf), wbuf);
					break;
				}
				NDK_SysMsDelay(10);
				if((time(NULL) - oldtime)>MAXTIME)
				{
					NDK_ScanStop();
					cls_show_msg1(3,"line %d:扫描超时", __LINE__);
					break;
				}
			}
			if((ret = NDK_ScanStop())!=NDK_OK)
			{
				cls_show_msg1(3,"line %d:关闭摄像头失败(%d)", __LINE__, ret);
				continue;
			}
		}
		send_result("通讯总次数%d,成功次数%d\n", bak, succ);
		memset(str,0,sizeof(str));
		ftos(ScanTime/succ,str);
		send_result("扫码性能:%ss/次\n", str);
		NDK_PortClose(PORT_NUM_COM1);
	}

	send_result("%s测试通过", TESTAPI);
	return;
}


