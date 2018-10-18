/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt9.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetMode接口功能
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
#define TESTAPI	"NDK_PrnSetMode"	
#define	MAXWAITTIME	(180)	
#define	NUM_GUO	(36)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt9(void)
* functional description 	:设置打印模式
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt9(void)
{
	/*private & local definition*/
#include	"hzfont.h"

	//以下为局部变量
	int nRet = 0;
	int i = -1, k = 0, nBusy = FALSE;
	time_t nDiff = 0, nOldtime = 0;
	int nKeyin = 0, nIndex = 0;
	EM_PRN_STATUS emStatus=0;
	int loop = 0,j=0;
	int sErr_args[4][2] = {{-1, 0}, {0, -1}, {0, 5}, {10, 0}};
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

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x8);	
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
			case -1:
				for(; loop<SZ_ARRAY(sErr_args); loop++)
				{
					if((nRet = NDK_PrnSetMode(sErr_args[loop][0],sErr_args[loop][1])) != NDK_ERR_PARA)
					{
						send_result("line %d:设置打印模式应失败(%d,%d)", __LINE__,nRet,loop);
						goto prnt_error;//return;
					}
				}
				i++;
				break;
			default:
				
				if(j<2)
				{
					if (NDK_PrnSetMode(i, j) != NDK_OK)
					{
						send_result("line %d:设置打印模式失败(%d,%d)", __LINE__,i,j);
						goto prnt_error;//return;
					}
					nIndex = rand()%(SZ_ARRAY(hzfont));
					NDK_PrnSetFont(hzfont[nIndex], 0);
					if((nRet=print_Ex("以[%s,模式%d,%s向]方式打印%d个\"国\"\n", hzfontstr[nIndex], i, j==0?"单":"双", NUM_GUO))<0)
					{
						send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
						goto prnt_error;//return;
					}
					for (k=1; k<=NUM_GUO; k++)
					{
						if((NDK_PrnStr("国"))!=NDK_OK)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,k);
							goto prnt_error;//return;
						}
					
					}
					NDK_PrnStr("\r");
					if (lib_kbhit()==ESC)
					{
						j = 0;
						i++;
					}
					else
						j++;
				}
				else
				{
					j = 0;
					i++;
				}
				break;
			case PRN_MODE_NORMAL +1: //恢复默认的设置
				switch (nType)
				{
				case PRN_TYPE_HIP:
				case PRN_TYPE_FIP:
					if(NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE,1) != NDK_OK)
					{
						send_result("line %d:设置打印模式[PRN_MODE_ALL_DOUBLE, 1]失败", __LINE__);
						goto prnt_error;//return;
					}
					NDK_PrnStr("045NewLand新大陆,恢复默认打印模式[PRN_MODE_ALL_DOUBLE, 1]");
					break;
				case PRN_TYPE_TP:
					if(NDK_PrnSetMode(PRN_MODE_NORMAL ,0) != NDK_OK)
					{
						send_result("line %d:设置打印模式[PRN_MODE_NORMAL, 0]失败", __LINE__);
						goto prnt_error;//return;
					}
					NDK_PrnStr("045NewLand新大陆,恢复默认打印模式[PRN_MODE_NORMAL, 0]");
					break;
				default:
					send_result("line %d:未能获取到打印机类型", __LINE__);
					goto prnt_error;//return;					
					break;
				}
				i++;
				break;
			case PRN_MODE_NORMAL +2:
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;
				break;
			}
			NDK_PrnStart();
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

