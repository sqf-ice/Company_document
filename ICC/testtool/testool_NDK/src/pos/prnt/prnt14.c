/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt14.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetPrintedLen接口功能
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
#define TESTAPI	"NDK_PrnGetPrintedLen"	
#define MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt14(void)
* functional description 	:获取打印长度
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt14(void)
{
	//以下为局部变量
	int nRet = 0;
	uint unOldLen=0,unNewLen=0;
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
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

			//case1:测试开机开始统计长度？
			if (ENTER==cls_show_msg("本用例是否为机具开机后运行的第一个用例?\n[确认]是[其它]否"))
			{
				if(NDK_PrnGetPrintedLen(&unOldLen)!=NDK_OK || unOldLen!=0)
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;
				}
			}
			
			//case2:测试获取的长度是否正确
			if(NDK_PrnGetPrintedLen(&unOldLen)!=NDK_OK)
			{
				send_result("line %d:%s测试失败",__LINE__,TESTAPI);
				goto prnt_error;
			}
			NDK_PrnStr("以下打印128*64建行LOGO\n");
			NDK_PrnImage(128, 64, 0, CCB_BMP);
			NDK_PrnStart();
			if((nRet = NDK_PrnGetPrintedLen(&unNewLen))!=NDK_OK || (unNewLen-unOldLen)!=64)
			{
				send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI, nRet, unNewLen-unOldLen);
				goto prnt_error;
			}

			//case3:测试只送数不打印两次获取长度应不变
			lib_initprn(0);
			if(NDK_PrnGetPrintedLen(&unOldLen)!=NDK_OK)
			{
				send_result("line %d:%s测试失败",__LINE__,TESTAPI);
				goto prnt_error;
			}
			NDK_PrnStr("如果看到这句话测试失败\n");
			if((nRet = NDK_PrnGetPrintedLen(&unNewLen))!=NDK_OK || (unNewLen-unOldLen)!=0)
			{
				send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI, nRet, unNewLen-unOldLen);
				goto prnt_error;
			}

			//case4:参数异常
			if((nRet = NDK_PrnGetPrintedLen(NULL))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI, nRet);
				goto prnt_error;
			}			
			
			send_result("%s测试通过", TESTAPI);
			goto prnt_error;
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
				goto prnt_error;
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
			goto prnt_error;
			break;
		}
	
	}
	
	//测试结束
prnt_error:
	NDK_PrnInit(g_PrnSwitch);	
	return;
}

