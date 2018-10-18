/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt15.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetDotLine接口功能
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
#define TESTAPI	"NDK_PrnGetDotLine"	
#define MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt15(void)
* functional description 	:获得该次打印的点阵行数. 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt15(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet;
	uint unLine=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	//if(NDK_PrnInit(0/*g_PrnSwitch*/) != NDK_OK)//本API必须在关闭掉边送边打情况下测试
	if(lib_initprn(0) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
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

			//case1:异常
			if(NDK_PrnGetDotLine(NULL) != NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败",__LINE__,TESTAPI);
				goto prnt_error;
			}	
			
			//case2:获取打印字符点行( NDK_PrnInit 初始化后是:PRN_MODE_NORMAL  横向正常、纵向正常模式)
			NDK_PrnSetFont(PRN_HZ_FONT_16x16, PRN_ZM_FONT_16x16);
			NDK_PrnStr("下面打印两行\n两行哦AAA!\n");
			//NDK_PrnGetDotLine必须 在start之前获取点行,移到后面去.NDK_PrnStart();
			if((nRet=NDK_PrnGetDotLine(&unLine)) != NDK_OK ||(unLine!=32 && unLine!=32+56 && unLine!=32+48))//测试版本程序会多出"注意：本机为测试版本，本打印单页无效"的提示热敏增加48点行，针打增加56行  linwl20140505
			{
				send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI,nRet,unLine);
				NDK_PrnStart();
				goto prnt_error;
			}
			NDK_PrnStart();
			
			//case3:获取打印图形点行( NDK_PrnInit 初始化后是:PRN_MODE_NORMAL  横向正常、纵向正常模式)
			NDK_PrnImage(128, 64, 0, CCB_BMP);
			//NDK_PrnGetDotLine必须 在start之前获取点行,移到后面去.NDK_PrnStart();
			if(NDK_PrnGetDotLine(&unLine) != NDK_OK ||(unLine!=64 && unLine!=64+56 && unLine!=64+48))
			{
				send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI,nRet,unLine);
				NDK_PrnStart();
				goto prnt_error;
			}
			NDK_PrnStart();

			//test over
			if (cls_show_msg("检查打印效果,[确认]通过,[其它]失败")==ENTER)
				send_result("%s测试通过", TESTAPI);
			else
				send_result("%s测试失败", TESTAPI);
			
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

