/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 6.c
* Author 			: chenfm
* version			: V1.0
* DATE			: 20130116
* directory 		: 
* description		: 磁卡刷卡压力测试
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  20130116 9.36
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"磁卡性能,压力"
#define MAXWAITTIME 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		chenfm  		   	20130116  	 	created
*****************************************************************/
static int testtime(void)
{
	/*private & local definition*/
#define	DEFAULT_CNT_STR	"10"//"60"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(60)
	int cnt = DEFAULT_CNT_VLE, ret = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR;

	/*process body*/
	while(1)
	{
		cls_printf("输入测试的时间(分钟):\n");
		//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default 
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
		
		if(cnt>0)
			break;
		else
			show_msg1(2, "输入应该大于0!\n");
	}
	return cnt;
}

static int IsDisplayed(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	while(1)
	{
		ret = cls_show_msg("_选择显示模式_\n"
							"1.不显示2.显示\n");
		
		switch(ret) 
		{
		case ESC://ESC,返回默认
			show_msg1(2, "将使用1.不显示(默认)...");
		case '1':
			return FALSE;
			break;
		case '2':
			return TRUE;
			break;
		default:
			continue;
			break;
		}
	}
}

static int select_TK(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	while(1)
	{
		ret  = cls_show_msg("_选择要读磁道_\n"
							"1.23道 2.123道\n");
		
		switch(ret) 
		{
		case ESC://ESC,返回默认
			show_msg1(2, "将读23道(默认)...");
		case '1':
			return TK2_3;
			break;
		case '2':
			return TK1_2_3;
			break;
		default:
			continue;
			break;
		}
	}
}

void mag_abnormal(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin = 0;
	
	/*process body*/
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:磁卡打开失败(%d)", __LINE__, ret);
		return;
	}
	while(1)
	{
		nKeyin=cls_show_msg("按住电源键,选择【切换应用】后,重新运行磁卡压力或自检中的磁卡,应能正常工作.");
		if(nKeyin == ESC)
		{
			NDK_MagClose();
			break;
		}
	}

	return;
}

static void mag_press(int SelTk, int IsDisp, float TestLong)
{
	/*private & local definition*/
	int ret = 0;
	float testalltime = 0.0;
#if defined ME15CHX
    float tmptime = 0.0;
#endif
#if K21_ENABLE
	char str[32] = {0},str1[32] = {0};
#endif
	int StrCountErr = 0, StrCountOK = 0, strTotle = 0;

	/*process body*/
#if defined ME15CHX
	tmptime =  lib_ReadStopwatch();
#else
	lib_StartStopwatch();
#endif
	do
	{
		switch((ret=MagcardReadTest(SelTk, IsDisp, /*99*/MAXWAITTIME)))
		{ 
		//case FUNCQUIT:
		case NDK_ERR_QUIT:
			continue;
			break;
		case STRIPE:
			StrCountOK++;
			break;
		case FAIL:
		case NDK_ERR_TIMEOUT:
		default://包括TK1,TK2...
			StrCountErr++;
			break;  
		}
		strTotle++;
#if defined ME15CHX
		testalltime = lib_ReadStopwatch()-tmptime;
#else
		testalltime = lib_ReadStopwatch();
#endif
		if(TestLong<testalltime)
			break;
	}while(ret!=NDK_ERR_QUIT);
#if !K21_ENABLE
	cls_show_msg("刷卡设定时间:%fs\n刷卡实际时间:%fs\n", TestLong, testalltime);
#else
	memset(str,0,sizeof(str));
	memset(str1,0,sizeof(str1));
	ftos(TestLong,str);
	ftos(testalltime,str1);
	cls_show_msg("刷卡设定时间:%ss\n刷卡实际时间:%ss\n", str, str1);
#endif
	cls_show_msg_record(FILENAME,FUNCNAME,"共错误%d次\n"
				"刷磁卡成功率:\n"
				"正确%d次/总%d次\n", StrCountErr, StrCountOK, strTotle);	
}

//磁卡的压力与性能应主要解决在一定强度下,刷卡成功率(性能)的问题
void systest6(void)
{
	
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}
	/*private & local definition*/
	int tk = 0, Isdisp = 0, keyin = 0;
	float testlong = 0.0;

	/*process body*/
	cls_show_msg1(1, "%s测试中...\n", TESTITEM);

	while (1)
	{	
		keyin = cls_show_msg("1.磁卡综合\n"
							"2.异常测试\n"	
							"0.参数配置");
		switch(keyin)
		{
		case '1':
			mag_press(tk, Isdisp, testlong);
			break;
		case '2':
			mag_abnormal();
			break;
		case '0':
			testlong =  testtime()*60; //请输入测试的时间(分钟),该函数保证返回时间有效(>0)
			tk = select_TK();
			Isdisp = IsDisplayed();
			break;
		case ESC:
			cls_show_msg("请换其它强/普/弱磁卡重新测试!任意键继续...");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

