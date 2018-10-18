/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt17.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnIntegrate接口功能
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
#define TESTAPI	"NDK_PrnIntegrate"	
#define MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct _Prntest18_mode
{
	int	mode;
	char *msg;
}Prntest18_mode;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt17(void)
* functional description 	:打印bmp，png等格式的图片  
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt17(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet = 0;
	int i=0;
	char sText[1024] = {0};
	char sTextOver[1024*2] = {0};

	Prntest18_mode prntest18_mode[] =
	{
		{TPSET_AUTO, "自动适应，文字环绕图像，保证不重合打印"},
		{TPSET_TEXTUP, "文字在上，若出现重合，文字将直接覆写在图像上"},
		{TPSET_PICUP, "图像在上，若出现重合，图像将直接覆写在文字上"},
		{TPSET_MIX, "文字图像嵌套，若重合，文字和图像将嵌套打印"}
	};
	
	//以下为函数体
	cls_show_msg1(2, "测试%s...请注意打印效果", TESTAPI);
	memset(sText, 'A', sizeof(sText));
	memset(sTextOver, 'B', sizeof(sTextOver));

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
			nBusy = FALSE;
		
			switch(i){
			default:
				NDK_PrnStr(prntest18_mode[i].msg);
				if ((nRet = NDK_PrnIntegrate(128, 64, 64, 64, CCB_BMP, sText, prntest18_mode[i].mode))!=NDK_OK)
				{
					send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, nRet,i);
					return;
				}
				break;
			case SZ_ARRAY(prntest18_mode)://说明正常测试已OK了,现在进行异常参数测试
				//测试错误的打印模式
				NDK_PrnStr("测试错误的打印模式将以默认的打印模式进行打印");
				if ((nRet =NDK_PrnIntegrate(128, 64, 64, 64,CCB_BMP ,sText, 256))!=NDK_OK)	//按default处理并返回SUCC
				{
					send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
					return;
				}
				NDK_PrnStart();

				//补充测试测试文字字符串长度超过1K
				NDK_PrnStr("测试字符串长度超过1K时,将只打印1K(请认真观察打印效果)");
				if ((nRet =NDK_PrnIntegrate(128, 64, 64, 64, CCB_BMP, sTextOver, TPSET_AUTO))!=NDK_OK)//按default处理并返回SUCC
				{
					send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
					return;
				}
				NDK_PrnStart();
				
				NDK_PrnStr("测试错误参数应不打印任何图形和文字");
				//xsize -- 图像宽度(像素)，不能大于384像素
				if ((nRet =NDK_PrnIntegrate(512, 16, 0 , 0, CCB_BMP, sText, TPSET_AUTO))!=FAIL)
				{
					send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
					return;
				}
				NDK_PrnStart();
				
				//(xpos+xsize)不能大于384像素
				if ((nRet =NDK_PrnIntegrate(128, 64, 385-128, 64, CCB_BMP, sText, TPSET_AUTO))!=FAIL)
				{
					send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
					return;
				}
				NDK_PrnStart();
				break;
			case SZ_ARRAY(prntest18_mode)+1:
				//test over
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)	//zhangnw20061031 modify
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				return;
				break;
			}
			NDK_PrnStart();
			i++;
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
				return;
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
	return;
}

