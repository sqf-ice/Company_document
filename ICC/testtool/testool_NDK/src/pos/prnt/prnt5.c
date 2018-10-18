/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt5.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnStart接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_PrnStart"	
#define MAXWAITTIME 90

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt5(void)
* functional description 	: 开始打印
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt5(void)
{
	//以下为局部变量
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet = 0;
	EM_PRN_STATUS emStatus=PRN_STATUS_BUSY;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}

	//case3:缺纸情况下 直接调用NDK_PrnStart应该会返回缺纸状态
	cls_show_msg("确保缺纸情况下,任意键继续...");
	if((nRet = NDK_PrnStart()) != PRN_STATUS_NOPAPER)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		goto prnt_error;
	}
	
	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(nBusy==FALSE)
			{
				nOldtime = time(NULL);//从第一次收到忙状态开始计时
				nBusy = TRUE;
			}
			if((nDiff=time(NULL)-nOldtime)>MAXWAITTIME)
			{
				send_result("line %d:打印机处于忙状态时间过长", __LINE__);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, nDiff);
			continue;
			break;

		case PRN_STATUS_OK:
			nBusy=FALSE;

			//start与init结合的功能在init用例中已经测试,本用例仅测试start特性的东西
			
			//case1:能否开始启动打印，打印返回值为0？以及阻塞特性		
			if((nRet = lib_initprn(0)) != NDK_OK)/*if((nRet = NDK_PrnInit(0)) != NDK_OK)*/
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			if((nRet = NDK_PrnStr("开启打印\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n")) != NDK_OK )
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			if((nRet = NDK_PrnStart()) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			NDK_PrnGetStatus(&emStatus);
			if(emStatus!=PRN_STATUS_OK)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI, emStatus);
				goto prnt_error;//return;
			}
			if (cls_show_msg("打出\"开启打印\"及10行,按[确认]否则[其他]")!=ENTER)
			{
				send_result("%s测试失败", TESTAPI);
				goto prnt_error;//return;
			}
			cls_show_msg1(2, "子用例通过!");			
				
			//case2:未调用NDK_PrnStr、NDK_PrnImage直接调用NDK_PrnStart
			cls_show_msg("将不打印任何数据,任意键继续...");
			if((nRet = lib_initprn(1)) != NDK_OK)/*if((nRet = NDK_PrnInit(1)) != NDK_OK)*/
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			if((nRet = NDK_PrnStart()) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			if (cls_show_msg("若打出任何数据,按[确认]否则[其他]")==ENTER)
			{
				send_result("%s测试失败", TESTAPI);
				goto prnt_error;//return;
			}

			send_result("%s测试通过", TESTAPI);
			goto prnt_error;//return;
			break;
		case PRN_STATUS_NOPAPER:
			cls_printf("打印机缺纸...装纸后,任意键继续,ESC放弃");
			lib_kbflush(); //清KB BUF
			while ((nKeyin=lib_kbhit())==0)
			{
				NDK_SysBeep();
				//正常装纸
				NDK_PrnFeedByPixel(10);
				sleep(2);
			}
			if (nKeyin==ESC)
			{
				send_result("line %d:打印机缺纸,用户放弃", __LINE__);
				goto prnt_error;//return;
			}
			NDK_PrnStart();
			cls_printf("测试%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: 打印机过热", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: 打印机电源故障", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_DESTROYED:
			send_result("line %d: 打印机损坏", __LINE__);
			goto prnt_error;
			break;
		default:
			//未知状态，判为错
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto prnt_error;//return;
			break;
		}
	
	}
	
	//测试结束	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);	
	return;
}

