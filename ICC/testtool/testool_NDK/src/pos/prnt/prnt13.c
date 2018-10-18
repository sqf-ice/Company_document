/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt13.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetUnderLine接口功能
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
#define TESTAPI	"NDK_PrnSetUnderLine"	
#define MAXWAITTIME 90

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt13(void)
* functional description 	:打印是否开启下划线功能
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt13(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet = 0, nRet1 = 0;
	
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
			//开开关关  验证是否最后一次生效 20140603 added by jiangym
			//case1:PRN_UNDERLINE_STATUS_OPEN
			NDK_PrnSetUnderLine(PRN_UNDERLINE_STATUS_OPEN);
			NDK_PrnStr("1.下面这句话如果没有下划线测试失败\n");
			NDK_PrnStart();
			NDK_SysDelay(10);
			if (cls_show_msg("1.检查打印效果(是否有下划线),[确认]有,[其它]没有")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto prnt_error;
			}
			NDK_PrnSetUnderLine(PRN_UNDERLINE_STATUS_OPEN);
			NDK_PrnStr("2.下面这句话如果没有下划线测试失败\n");
			NDK_PrnStart();
			NDK_SysDelay(10);
			if (cls_show_msg("2.检查打印效果(是否有下划线),[确认]有,[其它]没有")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto prnt_error;
			}
			
			//case2:PRN_UNDERLINE_STATUS_CLOSE
			NDK_PrnSetUnderLine(PRN_UNDERLINE_STATUS_CLOSE);
			NDK_PrnStr("3.下面这句话如果有下划线测试失败\n");
			NDK_PrnStart();
			NDK_SysDelay(10);
			if (cls_show_msg("3.检查打印效果(是否有下划线),[确认]有,[其它]没有")==ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto prnt_error;
			}
			NDK_PrnSetUnderLine(PRN_UNDERLINE_STATUS_CLOSE);
			NDK_PrnStr("4.下面这句话如果有下划线测试失败\n");
			NDK_PrnStart();
			NDK_SysDelay(10);
			if (cls_show_msg("4.检查打印效果(是否有下划线),[确认]有,[其它]没有")==ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto prnt_error;
			}

			//case3:异常
			if((nRet = NDK_PrnSetUnderLine(-1))!=NDK_ERR_PARA) 
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			if((nRet1 = NDK_PrnSetUnderLine(2))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet1);
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
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	NDK_PrnSetUnderLine(g_PrnUnderline);
	return;
}

