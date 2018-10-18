/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt12.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnFrnFeedByPixel接口功能
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
#define TESTAPI	"NDK_PrnFeedByPixel"	
#define  MINSTEPCNT  (0)
#define  MAXSTEPCNT  (792)	
#define MAXSTEPCNT_TP (1024)
#define	MAXWAITTIME	(180)	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt12(void)
* functional description 	:让打印机走纸，参数为像素点,调用该函数，并没有马上走纸，而是存在缓冲区中，等调用start之后和打印动作一起执行 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  				 chensj   20130105	   created
*****************************************************************/
void prnt12(void)
{
	//以下为局部变量
	int nRet = 0;
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet1 = 0,nCnt = 0;//unOldLen=0,unNewLen=0;
	int nType = lib_getprintertype();
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*///init后mode为 normal
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}

	NDK_PrnSetFont(PRN_HZ_FONT_12x16,PRN_ZM_FONT_8x8);
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
			
			if(nType == PRN_TYPE_TP)
			{
				cls_show_msg1(MAXWAITTIME/5, "%d秒后,将进纸%d个像素", MAXWAITTIME/5, MAXSTEPCNT_TP);
				print_Ex("---%d秒后,将进纸%d个像素---\n", MAXWAITTIME/5, MAXSTEPCNT_TP);
				if((nRet=NDK_PrnFeedByPixel(MAXSTEPCNT_TP))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
					goto prnt_error;//return;
				}
				print_Ex("---已进纸%d个像素---\n", MAXSTEPCNT_TP);
				NDK_PrnStart();

				//异常: 测试
				if((nRet=NDK_PrnFeedByPixel(MAXSTEPCNT_TP+1))!=NDK_ERR_PARA)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
					goto prnt_error;//return;
				}
			}
			
			cls_show_msg1(MAXWAITTIME/5, "%d秒后,将进纸%d个像素", MAXWAITTIME/5, MAXSTEPCNT);
			print_Ex("---%d秒后,将进纸%d个像素---\n", MAXWAITTIME/5, MAXSTEPCNT);
			if((nRet=NDK_PrnFeedByPixel(MAXSTEPCNT))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, nRet1);
				goto prnt_error;//return;
			}
			print_Ex("---已进纸%d个像素---\n", MAXSTEPCNT);
			NDK_PrnStart();
		
			nCnt = rand()%MAXSTEPCNT+1+MAXSTEPCNT_TP/*MAXSTEPCNT*/;	/* chensj modifie 2013/06/18 :MAXSTEPCNT修改成MAXSTEPCNT_TP防止热敏测试可能成功可能失败的BUG*/
			//cls_show_msg1(MAXWAITTIME/5, "%d秒后,将进纸%d个像素", MAXWAITTIME/5, MINSTEPCNT);
			//print_Ex("---%d秒后,将进纸%d个像素---\n", MAXWAITTIME/5, MINSTEPCNT);
			if((nRet=NDK_PrnFeedByPixel(MINSTEPCNT))!=NDK_OK||(nRet1=NDK_PrnFeedByPixel(nCnt))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, nRet1);
				goto prnt_error;//return;
			}
			print_Ex("---已进纸%d个像素---\n", MINSTEPCNT);
			NDK_PrnStart();

			//NDK_PrnSetMode(PRN_MODE_NORMAL,0);
			nCnt = rand()%MAXSTEPCNT;
			cls_show_msg1(MAXWAITTIME/5, "%d秒后,将随机进纸%d个像素", MAXWAITTIME/5, nCnt);
			print_Ex("---%d秒后,将随机进纸%d个像素---\n", MAXWAITTIME/5, nCnt);
#if 0	//因无法测算像素点数,不测了
			unOldLen = lib_getprinterlen();
#endif
			if((nRet=NDK_PrnFeedByPixel(nCnt))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto prnt_error;//return;
			}
#if 0	//因无法测算像素点数,不测了
			NDK_PrnStart();
			unNewLen = lib_getprinterlen();
			if(unNewLen-unOldLen!=nCnt)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI,unNewLen-unOldLen, nCnt);
				goto prnt_error;//return;
			}
#endif
			print_Ex("---已进纸%d个像素---\n", nCnt);			
			NDK_PrnStart();

			NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE,0);
			//nCnt = rand()%MAXSTEPCNT;
			cls_show_msg1(MAXWAITTIME/5, "%d秒后,将随机进纸%d个像素", MAXWAITTIME/5, nCnt);
			print_Ex("---%d秒后,将随机进纸%d个像素---\n", MAXWAITTIME/5, nCnt);
#if 0	//因无法测算像素点数,不测了			
			unOldLen = lib_getprinterlen();
#endif
			if((nRet=NDK_PrnFeedByPixel(nCnt))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto prnt_error;//return;
			}
#if 0	//因无法测算像素点数,不测了
			NDK_PrnStart();
			unNewLen = lib_getprinterlen();
			if(unNewLen-unOldLen!=nCnt/**2*/)//lib_getprinterlen,NDK_PrnFeedByPixel均应该为实际(打印或走纸)长度,和模式无关
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI,unNewLen-unOldLen, nCnt);
				goto prnt_error;//return;
			}
#endif			
			print_Ex("---已进纸%d个像素(应与上一次走同样长度)---\n", nCnt);	
			NDK_PrnStart();

			//test over
			nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,[确认]通过,[其它]失败");
			if (nKeyin==ENTER||nKeyin==0)
				send_result("%s测试通过", TESTAPI);
			else
				send_result("%s测试失败", TESTAPI);
				
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
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

