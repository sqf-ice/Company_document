/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt8.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetFont接口功能
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
#define TESTAPI	"NDK_PrnSetFont"	
#define MAXWAITTIME 180

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt8(void)
* functional description 	:设置打印字体
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt8(void)
{
	//以下为局部变量
#include	"dotfont.h"
#include	"hzfont.h"

	int nRet = 0;
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,i=-2,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int j=0;
	int nRet1=0;

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
			switch(i)
			{
			case -2:
				if ((nRet = NDK_PrnSetFont(-1,-1))!= NDK_ERR_PARA||(nRet1=NDK_PrnSetFont(100,100))!=NDK_ERR_PARA)
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__,nRet,nRet1);
					goto prnt_error;//return;
				}
				i++;
				break;
			case -1:
				NDK_PrnStr("[默认,默认]045NewLand新大陆\n\n");

				if((nRet =NDK_PrnSetFont(PRN_HZ_FONT_32x32,0)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__,nRet);
					goto prnt_error;//return;
				}				
				NDK_PrnStr("[HZ24x24,默认]045NewLand新大陆\n\n");			
				if((nRet =NDK_PrnSetFont(0,PRN_ZM_FONT_24x32)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__,nRet);
					goto prnt_error;//return;
				}
				NDK_PrnStr("[HZ24x24,ZM24x32]045NewLand新大陆\n\n");
				
				if (cls_show_msg("检查打印效果,按ENTER通过,其它键不通过")!=ENTER)
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);

				i++;
				break;				
			default:		
				if(j==0)
					print_Ex("---以%s方式组合上%d种HZ字体进行打印(ESC可跳过本轮打印)---\n", dotfontstr[i], SZ_ARRAY(hzfont));

				if(j<SZ_ARRAY(hzfont))
				{
					if(NDK_PrnSetFont(hzfont[j],dotfont[i]) != NDK_OK)
					{
						send_result("line %d:设置打印字体[%s,%s]失败!", __LINE__, dotfontstr[i], hzfontstr[j]);
						goto prnt_error;//return;
					}
					print_Ex("以[%s,%s]方式打印\"045NewLand$$新大陆两横人民币符号\xA3\xA4\xA3\xA4\"\n", hzfontstr[j], dotfontstr[i]);
					NDK_PrnStr("045NewLand新大陆\n\n");
					
					if(wait_anykey(1)==ESC)//if (lib_kbhit()==ESC)
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
			case SZ_ARRAY(dotfont):	/**<测试结束恢复默认值*/
				if(NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16 )!=NDK_OK)
				{
					send_result("line %d:设置打印字体[DOT8x16,HZ16x16]失败!", __LINE__);
					goto prnt_error;//return;
				}
				NDK_PrnStr("NewLand打印过程中设置字体[DOT8x16,HZ16x16]");
				
				i++;
				break;
			case SZ_ARRAY(dotfont)+1:
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

