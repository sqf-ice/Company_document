/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt2.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnImage接口功能
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
#define TESTAPI	"NDK_PrnImage"	
#define MAXWAITTIME 180 //30->180 for 针打
#define MAXCASECNT (10)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//以下函数亦可考虑按"职责链"模式进行设计,不过目前暂无必要(条件分支不复杂)
static int transfer_state(int *row_normal, int *col_normal, int *mode)
{
	/*private & local definition*/

	/*process body*/
	//对热敏来说,单双向是一样的,只要测试其中一种即可
	if(*row_normal==DISABLE && *col_normal==DISABLE)
	{
		*col_normal = ENABLE;
		*mode = PRN_MODE_WIDTH_DOUBLE;
		return SUCC;
	}
	else if(*row_normal==DISABLE && *col_normal==ENABLE)
	{
		*row_normal = ENABLE;
		*col_normal = DISABLE;
		*mode = PRN_MODE_HEIGHT_DOUBLE;
		return SUCC;
	}
	else if(*row_normal==ENABLE && *col_normal==DISABLE)
	{
		*col_normal = ENABLE;
		*mode = PRN_MODE_NORMAL;
		return SUCC;
	}
	else
		return FAIL;//未发生状态转换返回FAIL
}

//big,small以#define	MAXPRLINEWIDE_MID	128为中界
static int testcase_MAXPRLINEWIDE_big(int MAXPRLINEWIDE, int subcase)
{
	/*private & local definition*/
	char img[128] = {0};
	int nRet = 0;

	/*process body*/
	switch (subcase)
	{
	case 0:
		//由于地址空间分配的任意性，打印完正常图片CCB_BMP后，接着打印的图形除了乱码图也可能是清晰图形，只要不跑飞即可。
		NDK_PrnStr("---以下打印CCB_BMP和等大小的乱码图或等大的清晰图---\n");
		if ((nRet =NDK_PrnImage(128, 64*2, 0, CCB_BMP))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 1:
		NDK_PrnStr("---以下打印CCB_BMP_144---\n");
		if ((nRet = NDK_PrnImage(144, 64, 0, CCB_BMP_144))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 2:
		NDK_PrnStr("---以下打印CCB_BMP---\n");
		if ((nRet = NDK_PrnImage(128, 64, 0, CCB_BMP))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 3:
		NDK_PrnStr("---以下打印CCB_BMP(xsize<128且xsize%8!=0)---\n");
		if ((nRet = NDK_PrnImage(128-1, 64, 0, CCB_BMP))!=NDK_OK)//底层会对size做(+7/8)*8的处理
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 4://分上下半幅打印
		NDK_PrnStr("---以下分上下半幅打印CCB_BMP---\n");
		if ((nRet = NDK_PrnImage(128, 32, 0, CCB_BMP))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		if ((nRet = NDK_PrnImage(128, 32, 0, CCB_BMP+128*32/8))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 5://靠右打印
		NDK_PrnStr("---以下靠右打印CCB_BMP---\n");
		if ((nRet = NDK_PrnImage(128, 64, MAXPRLINEWIDE-128, CCB_BMP))!=NDK_OK)
		{
			NDK_PrnStart();
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 6://部分打印
		//xsize+xpos>MAXPRLINEWIDE(其中:xsize与xpos均不越界)
		if ((nRet = NDK_PrnImage(128, 64, MAXPRLINEWIDE-128/2, CCB_BMP))!=NDK_ERR_PARA)//==NDK_OK)
		{
			
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 7://部分打印//xsize+xpos>MAXPRLINEWIDE(其中:xpos不越界,xsize越界)
		memset(img, 0xff, sizeof(img));
		memset(img, 0xcc, (MAXPRLINEWIDE+7)/8);
		if ((nRet = NDK_PrnImage(sizeof(img)*8, 1, 8, img))!=NDK_ERR_PARA)//==NDK_OK)
		{
			
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 8://zhangnw20131016与严明、刘罡确认:不需要支持此整8特性 //xpos不是8整数倍时按8的整数倍处理 
		print_Ex("---以下打印2幅CCB_BMP(左边界%s应对齐)---\n", /*(PRN_TYPE_TP ==nType)?"":*/"不");
		if ((nRet = NDK_PrnImage(128, 64, 16, CCB_BMP)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		if ((nRet = NDK_PrnImage(128, 64, 16+7, CCB_BMP)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	default:    /**< 错误参数*/
		//xpos>MAXPRLINEWIDE，起始位置xpos越界将不打印任何东西
		//xpos从0开始计,那么MAX_PRLINEWIDE-1不越界,但MAX_PRLINEWIDE就是越界的了
		if ((nRet = NDK_PrnImage(128, 64, MAXPRLINEWIDE+1, CCB_BMP))!=NDK_ERR_PARA)//==NDK_OK)//zhangnw20100902:由MAXPRLINEWIDE修正为MAXPRLINEWIDE+1,放宽限制
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		if ((nRet = NDK_PrnImage(128, 64, 0, NULL)) !=NDK_ERR_PARA)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		
		break;
	}

	return SUCC;
}

#if 0
static int testcase_MAXPRLINEWIDE_small(int MAXPRLINEWIDE, int subcase)
{
	/*private & local definition*/
#if !(defined GP730||defined GP710||defined GP730CP)
	char img[128] = {0};
#endif

	/*process body*/
	switch (subcase)
	{
	case 0:
		print("---以下打印NL_Icon和等大小的乱码图---\n");
		if (printimage(16, 16*2, 0, NL_Icon)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 1:
		break;
	case 2:
		print("---以下打印NL_Icon---\n");
		if (printimage(16, 16, 0, NL_Icon)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 3:
		print("---以下打印NL_Icon(xsize<16且xsize%8!=0)---\n");
		if (printimage(16-1, 16, 0, NL_Icon)==FAIL)//底层会对size做(+7/8)*8的处理
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 4://分上下半幅打印
		print("---以下分上下半幅打印NL_Icon---\n");
		if (printimage(16, 8, 0, NL_Icon)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		if (printimage(16, 8, 0, NL_Icon+16*8/8)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;
	case 5://靠右打印
		print("---以下靠右打印NL_Icon---\n");
		if (printimage(16, 16, MAXPRLINEWIDE-16, CCB_BMP)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		break;			
	case 6://部分打印
		//xsize+xpos>MAXPRLINEWIDE(其中:xsize与xpos不越界)
#if defined GP730||defined GP710||defined GP730CP
		if (printimage(16, 16, MAXPRLINEWIDE-16/2, NL_Icon)==SUCC)
#else
		print("---以下打印部分NL_Icon---\n");
		if (printimage(16, 16, MAXPRLINEWIDE-16/2, NL_Icon)==FAIL)
#endif
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
		//xsize<=0,xpos又合法时(MAXPRLINEWIDE>xpos>=0),xsize将按MAXPRLINEWIDE-xpos进行修正
#if defined GP730||defined GP710||defined GP730CP
		if (printimage(-1, 16, MAXPRLINEWIDE-16/2, NL_Icon)==SUCC)
#else
		print("---以下打印部分NL_Icon---\n");
		if (printimage(-1, 16, MAXPRLINEWIDE-16/2, NL_Icon)==FAIL)
#endif
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}		
		break;
	case 7://部分打印//xsize+xpos>MAXPRLINEWIDE(其中:xpos不越界,xsize越界)
#if !(defined GP730||defined GP710||defined GP730CP)
		print("---以下打印虚线(xpos=8)---\n");
		memset(img, 0xff, sizeof(img));
		memset(img, 0xcc, (MAXPRLINEWIDE+7)/8);
		if (printimage(sizeof(img)*8, 1, 8, img)==FAIL)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
#endif
		break;
	default://错误参数
		//xpos>MAXPRLINEWIDE
		//xpos从0开始计,那么MAX_PRLINEWIDE-1不越界,但MAX_PRLINEWIDE就是越界的了
		if (printimage(16, 16, MAXPRLINEWIDE+1, NL_Icon)==SUCC)//zhangnw20100902:由MAXPRLINEWIDE修正为MAXPRLINEWIDE+1,放宽限制
		{
			send_result("line %d: %s测试失败(xpos>PRLINEWIDE)", __LINE__, TESTAPI);
			return FAIL;
		}				
		//ysize<0
		if (printimage(16, -1, 0, NL_Icon)==SUCC)
		{
			send_result("line %d: %s测试失败(ysize<0)", __LINE__, TESTAPI);
			return FAIL;
		}		
		break;
	}		

	return SUCC;
}
#endif

/****************************************************************
* function name 	 				: void prnt2(void)
* functional description 	: 打印图形
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt2(void)
{
	//以下为局部变量
	time_t nOldtime=0,nDiff=0;
	int nBusy = FALSE,i=0,nKeyin=0;
	int nRow_normal=DISABLE, nCol_normal=DISABLE, nMode=0;
	EM_PRN_STATUS  emStatus=0;
	int MAXPRLINEWIDE_old =0;
	int MAXPRLINEWIDE = 0;
	int nRet =0;
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
	
	MAXPRLINEWIDE_old =(PRN_TYPE_TP==nType)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP==nType)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);

BEGIN:
	cls_show_msg1(2, "测试%s(横向%s,纵向%s)...\n", TESTAPI, nRow_normal==DISABLE?"放大":"正常", nCol_normal==DISABLE?"放大":"正常");
	print_Ex("测试%s(横向%s,纵向%s)...\n", TESTAPI, nRow_normal==DISABLE?"放大":"正常", nCol_normal==DISABLE?"放大":"正常");
	NDK_PrnStart();

	NDK_PrnSetMode(nMode, 0);
	
	MAXPRLINEWIDE = MAXPRLINEWIDE_old/((nRow_normal==DISABLE)?2:1);//修正MAXPRLINEWIDE
	
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
			//test begin
			nBusy=FALSE;
			switch(i)
			{
			default:
				if(/*(MAXPRLINEWIDE>=MAXPRLINEWIDE_MID)&&*/(testcase_MAXPRLINEWIDE_big(MAXPRLINEWIDE, i )==FAIL))
					return;
#if 0	//从目前数据看,MAXPRLINEWIDE总会大于MAXPRLINEWIDE_MID,故暂封掉,减小目标代码大小
				else if((MAXPRLINEWIDE<MAXPRLINEWIDE_MID)&&(testcase_MAXPRLINEWIDE_small(MAXPRLINEWIDE, i)==FAIL))
					return;
#endif
				break;
			case MAXCASECNT:
				if(transfer_state(&nRow_normal, &nCol_normal, &nMode)==SUCC)
				{
					i = 0;
					goto BEGIN;
				}

				//由于本例可能会修改默认的压缩模式,测试毕后要恢复一下
				//恢复默认的设置
				switch (nType)
				{
				case PRN_TYPE_HIP:
				case PRN_TYPE_FIP:
					if(NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE,1) != NDK_OK)
					{
						send_result("line %d:设置打印模式[PRN_MODE_ALL_DOUBLE, 1]失败", __LINE__);
						goto prnt_error;
					}
					break;
				case PRN_TYPE_TP:
					if(NDK_PrnSetMode(PRN_MODE_NORMAL ,0) != NDK_OK)
					{
						send_result("line %d:设置打印模式[PRN_MODE_NORMAL, 0]失败", __LINE__);
						goto prnt_error;
					}
					break;
				default:
					send_result("line %d:未能获取到打印机类型", __LINE__);
					goto prnt_error;
					break;
				}				
				
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)//zhangnw20061031 modify
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;
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
			goto prnt_error;//return;
			break;
		}
	}
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

