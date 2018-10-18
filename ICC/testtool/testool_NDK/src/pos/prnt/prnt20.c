/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt20.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetPageLen 接口功能
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
#define TESTAPI	"NDK_PrnSetPageLen"	
#define	MAXCASECNT		(6)
#define	MAXWAITTIME	(180)	//30->180 for 针打
#define	MAXPAGELEN_TP		(2370)	//热敏
#define	MAXPAGELEN_HIP	(2370)	//穿孔针打
#define	MAXPAGELEN_FIP	(8*88)	//摩擦针打

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int testcase_TP(int subcase)
{
	/*private & local definition*/
	char sImage[(128*64)/8] = {0};
	int nRnd = rand()%49+1;

	/*process body*/
	memset(sImage, 0xff, sizeof(sImage));
	switch (subcase)
	{
	case 0:	//测试正常打印	
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;	
	case 1: //输入参数为随机值及2371测试
		if(NDK_PrnSetPageLen(nRnd)==NDK_OK)
		{
			send_result("line %d:%s测试失败(rand:%d)", __LINE__, TESTAPI, nRnd);
			return FAIL;
		}
		if(NDK_PrnSetPageLen(MAXPAGELEN_TP+1)==NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 2://热敏打印机，输入参数为0的时候，两页之间只有一空白线		
		if(NDK_PrnSetPageLen(0)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		NDK_PrnStr("将打印\"setprintpagelen:0\"及128x64填充单");
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	case 3:	//输入参数为50测试
		if(NDK_PrnSetPageLen(50)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		NDK_PrnStr("将打印\"setprintpagelen:50\"及128x64填充单");
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	case 4://输入参数为2370测试
		if(NDK_PrnSetPageLen(MAXPAGELEN_TP)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		print_Ex("将打印\"setprintpagelen:%d\"及128x64填充单", MAXPAGELEN_TP);
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	default://输入参数为80(默认值)
		if(NDK_PrnSetPageLen(80)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		NDK_PrnStr("将打印\"setprintpagelen:80\"及128x64填充单");
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	}

	return SUCC;
}

static int testcase_xIP(int xIPtype, int subcase)
{
	/*private & local definition*/
	char sImage[(128*64)/8] = {0};
	int MAXPAGELEN_xIP = (xIPtype==PRN_TYPE_HIP )?MAXPAGELEN_HIP:MAXPAGELEN_FIP;

	/*process body*/
	memset(sImage, 0xff, sizeof(sImage));
	switch (subcase)
	{
	case 0:	//测试正常打印	
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;	
	case 1: //越界测试
		if(NDK_PrnSetPageLen(-1)==NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		if(NDK_PrnSetPageLen(MAXPAGELEN_xIP+1)==SUCC)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 2://边界测试	
		if(NDK_PrnSetPageLen(0)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		NDK_PrnStr("将打印\"setprintpagelen:0\"及128x64填充单");
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	case 3://边界测试	
		if(NDK_PrnSetPageLen(MAXPAGELEN_xIP)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		print_Ex("将打印\"setprintpagelen:%d\"及128x64填充单", (xIPtype==PRN_TYPE_HIP )?MAXPAGELEN_HIP:MAXPAGELEN_FIP);
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	default://输入参数为默认值
		if(NDK_PrnSetPageLen(396)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		print_Ex("将打印\"setprintpagelen:396\"及128x64填充单");//NDK_PrnStr("将打印\"setprintpagelen:396\"及128x64填充单");
		if (NDK_PrnImage(128, 64, 0, sImage)!=NDK_OK)
		{
			send_result("line %d:打印图形失败", __LINE__);
			return FAIL;
		}
		break;
	}

	return SUCC;
}


/****************************************************************
* function name 	 				:void prnt20(void)
* functional description 	:排列
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt20(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet=0,i=0;
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
			switch(i)
			{
			default:
				//出错行提示放到具体testcase去提示(在testcase调用了send_result),本处直接返回
				if((nType==PRN_TYPE_TP )&&(testcase_TP(i)==FAIL))
						goto prnt_error;//return;
				else if((nType==PRN_TYPE_HIP ||nType==PRN_TYPE_FIP )&&(testcase_xIP(nType, i)==FAIL))
						goto prnt_error;//return;
				else if(nType==PRN_TYPE_END )
				{
					send_result("line %d:未能获取到打印机类型", __LINE__);
					goto prnt_error;//return;
				}
				break;
			case MAXCASECNT:
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)//zhangnw20061031 modify
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;				
				break;
			}
			NDK_PrnStr("\f");//必须送一个\f设置页长才生效
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

