/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 磁卡/键盘交叉
* file name			: systest48.c
* Author 			: 	chensj
* version			: 
* DATE				: 
* directory 			: 
* description			:  磁卡/键盘交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130822  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"磁卡/键盘交叉"
#define	DEFAULT_CNT_STR	"10"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)

/*------------global variables definition-----------------------*/

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
* history 				: author			date			remarks
					 chensj 			2013/08/22     created
*****************************************************************/
void systest48(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	int ret = 0, bak = 0,cnt = 0, nErrorCode = 0, succ = 0;
	char szBuf[201] = {0}, CntStr[8] = DEFAULT_CNT_STR, pszTk1[MAXTRACKLEN] = {0}, pszTk2[MAXTRACKLEN] = {0}, pszTk3[MAXTRACKLEN] = {0};
	uint unLen = 0;
	uchar cSwiped = 0;
		
	cls_printf("默认交叉测试次数:\n");
	if ((ret=NDK_KbGetInput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//模拟应用手输卡号测试,测试打开磁卡设备快速输入是否会卡键,任意键继续
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
		return;
	}

	while(1)
	{
		if(ESC==cls_show_msg1(2, "正在进行%d次%s测试,成功%d次,ESC退出", bak+1, TESTITEM, succ))
			break;
		if(bak++==cnt)
			break;
		//刷卡测试
		NDK_MagReset();//根据叶明统提供的流程在读卡之后要复位磁头且清除磁卡缓冲区数据 20140304 linwl
		memset(pszTk1,0,sizeof(pszTk1));
		memset(pszTk2,0,sizeof(pszTk2));
		memset(pszTk3,0,sizeof(pszTk3));
#if !(defined ME30MH||defined ME32)	
		cls_show_msg("请刷磁卡,任意键继续");
		if((ret=NDK_MagSwiped(&cSwiped))!=NDK_OK||cSwiped!=1)
		{
			cls_show_msg("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTITEM, ret, cSwiped);
			continue;
		}
#else
		cls_printf("请刷磁卡");
		while(1)
		{
			NDK_MagSwiped(&cSwiped);
			if(cSwiped==1)
				break;
		}
#endif
		if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &nErrorCode))!=NDK_OK||nErrorCode!=0)
		{
			cls_show_msg("line %d:%s测试失败(%d,%d)", __LINE__, TESTITEM, ret, nErrorCode);
			continue;
		} 
#if !TK1_DISABLE		
		//显示磁道信息
		if((ret=check_stripe_result_tk1(pszTk1))!=0x00)
		{
			cls_show_msg("line %d:%s测试失败(0x%x)", __LINE__, TESTITEM, ret);
			continue;
		}
#endif
		if((ret=check_stripe_result_tk2(pszTk2))!=0x00)
		{
			cls_show_msg("line %d:%s测试失败(0x%x)", __LINE__, TESTITEM, ret);
			continue;
		}
		if((ret=check_stripe_result_tk3(pszTk3))!=0x00)
		{
			cls_show_msg("line %d:%s测试失败(0x%x)", __LINE__, TESTITEM, ret);
			continue;
		}
		
		//键盘输入测试//键盘输入时快速输入应该能够精确识别输入的按键,不出现停顿,连续显示,卡键等异常现象
		cls_printf("请快速输入:");
		if((ret=NDK_KbGetInput(szBuf,20, 200, &unLen,INPUTDISP_NORMAL, 0,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
		{
			cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
			continue;
		}
		if(cls_show_msg("是否会出现卡键现象\n确认:是  其他:否")==ENTER)
		{
			cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
			continue;
		}
		succ++;
	}
	NDK_MagClose();
	cls_show_msg("%s测试完成,总共测试%d次成功%d次\n", TESTITEM, bak, succ);
	return;
}
