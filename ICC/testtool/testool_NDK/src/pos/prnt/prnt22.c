/************************************************************************
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt22.c
* Author 		: jiangym
* version		: 
* DATE			: 20130821
* directory 		: 
* description		: 测试NDK_PrnFeedPaper接口功能
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
#define TESTAPI	"NDK_PrnFeedPaper"	
#define MAXWAITTIME 180

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt22(void)
* functional description 	: 操作热敏打印机在打印单页前或打印完成后的进退纸操作 
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			
*****************************************************************/
void prnt22(void)
{
	//以下为局部变量
	int nBusy = FALSE, nKeyin = 0;
	time_t nOldtime = 0, nDiff = 0;
	int ret = 0;
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

	while(1)
	{
		if((ret=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,ret);
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
			
			//case1:参数异常-1,PRN_FEEDPAPER_AFTER+1
			if((ret=NDK_PrnFeedPaper(-1))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER+1))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}

			//case5:补充用例验证走纸函数能够准确的将纸张走出纸槽
			NDK_PrnStr("本行文字应完全在纸槽外");
			NDK_PrnStart();
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if(cls_show_msg("注意[本行文字应完全在纸槽外]是否都在纸槽外,[Enter]通过,[其他]失败")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				return;	
			}
			
			//case2:正常测试先进纸后退纸
			NDK_PrnStr("打印前后进退纸测试\n   Newland   \n123       321");
			NDK_PrnStart();
			//case3:支持连续进纸和连续退纸
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_BEFORE))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_BEFORE))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			NDK_PrnStr("123       321\n   Newland   \n上面的数据不会相互覆盖");
			NDK_PrnStart();
			//case4:进纸后再退纸应该能够回到原来进纸前的位置,从打印现象上看：打印效果不会出现覆盖打印和行间间隔太大的现象
			if((ret=NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;	
			}
			if(cls_show_msg("打印的几行数字不出现覆盖,[Enter]通过,[其他]失败")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				return;	
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
