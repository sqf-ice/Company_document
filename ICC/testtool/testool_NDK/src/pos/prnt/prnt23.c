/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt22.c
* Author 		: linwl
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetDotData接口根据输入字符串获取出系统指定打印字体相应的点阵缓冲
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
#define TESTAPI	"NDK_PrnGetDotData"	
#define MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt21(void)
* functional description 	:加载、设置字体、打印
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						linwl   20140105	created
*****************************************************************/
void prnt23(void)
{
	//以下为局部变量
	int nBusy = FALSE, nKeyin = 0, ret = 0;
	time_t nOldtime = 0, nDiff = 0;
	EM_PRN_STATUS emStatus=PRN_STATUS_BUSY;
	char str[]="请输入密码:1234590!@#()*&";
	char prnbuf[4096];
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	
	//测试前置:
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}
	
	//case1:各个参数异常测试
	//emGetFont参数指定字库，目前只支持PRN_GETFONT_16X16，其它值应该返回不支持
	if((ret = NDK_PrnGetDotData(0,str,prnbuf,strlen(str)*8,16))!=NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//字符串参数为NULL，应该返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,NULL,prnbuf,strlen(str)*8,16))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//接收缓冲参数为NULL，应该返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,NULL,strlen(str)*8,16))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//缓存像素宽参数为0，应该返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,prnbuf,0,16))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//缓存像素高参数为0，应该返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,prnbuf,strlen(str)*8,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//缓存像素宽,要求宽度不小于pszBuf字符串长度*字符点阵宽度(如8)，否则返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,prnbuf,strlen(str)*8-1,16))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//缓存像素高,要求不小于字符点阵高度，否则返回参数错误
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,prnbuf,strlen(str)*8,16-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试
	memset(prnbuf, 0, sizeof(prnbuf));
	if((ret = NDK_PrnGetDotData(PRN_GETFONT_16X16,str,prnbuf,strlen(str)*8,16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, ret);
		RETURN;
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
			NDK_PrnImage(strlen(str)*8,16,0,prnbuf);
			NDK_PrnStart(); 
			NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
			if ((nKeyin = cls_show_msg("请检查:%s的打印效果,[确认]通过,[其它]失败",str))!=ENTER)
				send_result("%s测试失败", TESTAPI);
			else
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

