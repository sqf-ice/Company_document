/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt10.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetGreyScale接口功能
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
#define TESTAPI	"NDK_PrnSetGreyScale"
#define	MAXWAITTIME	(180)	//30->180 for 针打

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt10(void)
* functional description 	:设置打印灰度
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt10(void)
{
	//以下为局部变量
	char sPrintbuf[(128*64)/8] = {0};
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int i = GREY_MINVALID;
	int nRet=0;
	
	//目前仅有热敏打印支持灰度设置,故以下暂写死
	int MAXPRLINEWIDE_old = MAXPRLINEWIDE_TP, MAXPRLINEWIDE=MAXPRLINEWIDE_old/2;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	memset(sPrintbuf,0xff,sizeof(sPrintbuf));
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);	
	NDK_PrnSetMode(PRN_MODE_NORMAL/*PRN_MODE_ALL_DOUBLE*/,0);
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

			if(i==GREY_MAXVALID+1)
			{
				//说明[MIN,MAX]正常范围已测了,现在进行异常参数测试
				if(NDK_PrnSetGreyScale(GREY_MAXVALID+1) != NDK_ERR_PARA)
				{
					send_result("line %d:设置灰度%d应失败!\n", __LINE__, GREY_MAXVALID+1);
					goto  prnt_error;
				}
#if 0 //不必测试了
				if(NDK_PrnSetGreyScale(GREY_MINVALID-1) != NDK_ERR_PARA)
				{
					send_result("line %d:设置灰度%d应失败!\n", __LINE__, GREY_MINVALID-1);
					goto  prnt_error;
				}
#endif

				//test over
				//NDK_PrnSetGreyScale(g_PrnGrey);
				nKeyin = cls_show_msg("验证打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER)
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;
			}
			else
			{
				if(NDK_PrnSetGreyScale(i)==NDK_OK)
				{
					print_Ex("设置打印灰度值%d成功%c请注意打印灰度效果", i, (i%2)?'\n':',');
					if((nRet=NDK_PrnImage(128, 64, (i%2)?0:(MAXPRLINEWIDE-128),sPrintbuf))!=NDK_OK)
					{
						send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI, nRet, i);//cls_show_msg("line %d:打印填充单失败!\n", __LINE__);
						goto prnt_error;//return;
					}
					
					NDK_PrnStr("\n");
					NDK_PrnStart();  
				}
				else
				{
					send_result("line %d:设置打印灰度值%d失败", __LINE__, i);
					goto prnt_error;
				}
				i++;//测试完i,自增为下一个参数测试
			}
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
	NDK_PrnSetGreyScale(g_PrnGrey);
	return;
}

