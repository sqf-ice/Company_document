/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt11.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetForm接口功能
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
#define TESTAPI	"NDK_PrnSetForm"	
#define	MAXWAITTIME	(180)
#define	LEFTBORDER_MIN		(0)
#define	LEFTBORDER_MAX_TP	(288)
#define	LEFTBORDER_MAX_HIP	(288)
#define	LEFTBORDER_MAX_FIP	(288)
#define	RANGE_MIN		(0)
#define	RANGE_MAX_TP	(255)
#define	RANGE_MAX_HIP	(255)
#define	RANGE_MAX_FIP	(255)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int testcase(int subcase, int LEFTBORDER_MAX, int RANGE_MAX)
{
	/*private & local definition*/
	int nRnd = rand()%LEFTBORDER_MAX, nRet = SUCC;
	int nColumn = rand()%RANGE_MAX, nRow = rand()%RANGE_MAX;
	
	/*process body*/
	switch (subcase)
	{
	case 0:		
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);
		print_Ex("---左边距(%d)行列间距(%d,%d)(文字和图形应左边界对齐,行列间距不影响图形效果)---\n", LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);//但影响图形行间距  此要求已经过时 20150403 linwl和linw确认
		NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");
		NDK_PrnImage(128, 64, 0, CCB_BMP);
		NDK_PrnImage(16, 16, 0, NL_Icon);
		break;
	case 1:  
#if 1 /*这个案例暂时不测，边界值开发不好定，左间距+字间距超过最大打印的点阵行程序跑飞*/
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);    
		print_Ex("---左边距(%d)行列间距(%d,%d)(图形应左边界仍为0,行列间距不影响图形效果)---\n", LEFTBORDER_MAX, RANGE_MAX, RANGE_MAX);//但影响图形行间距
		if((nRet=NDK_PrnSetForm(LEFTBORDER_MAX,RANGE_MAX,RANGE_MAX))!=NDK_OK)
		{
			cls_show_msg1(5,"line %d:%s测试失败(nRet=%d)",__LINE__,TESTAPI,nRet);
			return -5;	
		}	
		NDK_PrnStr("NL新大陆045\n");//NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");
		NDK_PrnImage(128, 64, 0, CCB_BMP);
		NDK_PrnImage(16, 16, 0, NL_Icon);
#endif
		break;	
	case 2:
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);   			
		print_Ex("---左边距(%d)行列间距(%d,%d)---\n",nRnd,nColumn,nRow);
		if((nRet=NDK_PrnSetForm(nRnd,nColumn,nRow))!=NDK_OK)
		{
			cls_show_msg1(5,"line %d:%s测试失败(nRet=%d)",__LINE__,TESTAPI,nRet);
			return -6;	
		}	
		NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n");//NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");
		NDK_PrnImage(128, 64, 0, CCB_BMP);
		NDK_PrnImage(16, 16, 0, NL_Icon);//小图标
		NDK_PrnImage(128, 64, MIN(288-nRnd, 30), CCB_BMP);
		NDK_PrnImage(16, 16, MIN(288-nRnd, 30), NL_Icon);//小图标
		break;
	case 3://异常
#if ANDIROD_ENABLE//20170407 linqy反馈修改
		if(NDK_PrnSetForm(385,0,0)!=NDK_ERR_PARA)
#else
		if(NDK_PrnSetForm(299,0,0)!=NDK_ERR_PARA)
#endif
			return -2;
		if(NDK_PrnSetForm(50,34,256)!=NDK_ERR_PARA)
			return -3;
		if(NDK_PrnSetForm(50,256,30)!=NDK_ERR_PARA)
			return -4;
		break;
	case 4://边界测试:w刚好显示在边界处 20150425新增边界值测试,linwl
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);   			
		print_Ex("---左边距(155)行列间距(63,0)---\n");
		if((nRet=NDK_PrnSetForm(155,63,0))!=NDK_OK)
		{
			cls_show_msg1(5,"line %d:%s测试失败(nRet=%d)",__LINE__,TESTAPI,nRet);
			return -6;	
		}	
		NDK_PrnStr("新NewLand\n");
		break;
	case 5://边界测试,w正好换行
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);   			
		print_Ex("---左边距(156)行列间距(63,0)---\n");
		if((nRet=NDK_PrnSetForm(156,63,0))!=NDK_OK)
		{
			cls_show_msg1(5,"line %d:%s测试失败(nRet=%d)",__LINE__,TESTAPI,nRet);
			return -6;	
		}	
		NDK_PrnStr("新NewLand\n");	
		break;
	default://恢复默认
		NDK_PrnSetForm(LEFTBORDER_MIN,RANGE_MIN,RANGE_MIN);  
		print_Ex("---左边距(%d)行列间距(%d,%d)(默认)---\n", LEFTBORDER_MIN,RANGE_MIN, RANGE_MIN);
		NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");	
		NDK_PrnImage(128, 64, 0, CCB_BMP);
		NDK_PrnImage(16, 16, 0, NL_Icon);
		nRet = FAIL;
		break;
	}	
	
	return nRet;
}

/****************************************************************
* function name 	 				:void prnt11(void)
* functional description 	:设置打印左边界、字间距、行间距。在对打印机有效设置后将一直有效，直至下次设置
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt11(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int i=0;
	int nLeftborder_max=0;
	int nRange_max = 0;
	int nRet = 0;
	int nType = lib_getprintertype();
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}

	nLeftborder_max=(PRN_TYPE_TP ==nType)?LEFTBORDER_MAX_TP:((PRN_TYPE_HIP ==nType)?LEFTBORDER_MAX_HIP:LEFTBORDER_MAX_FIP);
	nRange_max = (PRN_TYPE_TP==nType)?RANGE_MAX_TP:((PRN_TYPE_HIP==nType)?RANGE_MAX_HIP:RANGE_MAX_FIP);
	
	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
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

			if((nRet = testcase(i++, nLeftborder_max,nRange_max))==SUCC)
			{
				NDK_PrnStart();
				//continue;
			}
			else
			{
				if(nRet !=FAIL)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				NDK_PrnStart();
				nKeyin = cls_show_msg("检查打印效果,[确认]通过,[其它]失败");
				if (nKeyin!=ENTER)
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;
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
	
	//测试结束
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

