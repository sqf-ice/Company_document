/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: 硬件信息测试
* file name			: systest31.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 硬件信息测试
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"
#define  MAXWAITTIME 30

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"硬件信息"

/*------------global variables definition-----------------------*/
#define	ITEMPERPAGE	(4)

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//case 1.1:刷卡总数
void CardCountTest(void)
{
	unsigned int unLen=0;
	char sBuf1[128]={0};
	char sBuf2[128]={0};
	int ret = 0;

#if K21_ENABLE//K21平台不支持取磁卡刷卡次数
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, &unLen, sBuf1))!=NDK_ERR_NOT_SUPPORT)
	{	
		cls_show_msg("line %d:刷卡统计失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	cls_show_msg1(2,"line %d:该机器不支持获取刷卡次数,故不做此项测试",__LINE__);
	return;
#endif	
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, &unLen, sBuf1))!=NDK_OK)
	{	
		cls_show_msg("line %d:刷卡统计失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	if(cls_show_msg("当前刷卡总数为:%s次,[确认]正确,[其他]错误",sBuf1) != ENTER)
	{
		cls_show_msg("line %d:刷卡统计失败(%s次)",__LINE__,sBuf1);
		return;
	}
	MagcardReadTest(TK2, FALSE, 0);
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, &unLen, sBuf2))!=NDK_OK)
	{	
		cls_show_msg("line %d:刷卡统计失败(%d,%s)",__LINE__,ret,sBuf2);
		return;
	}
	if(atoi(sBuf2) - atoi(sBuf1) !=  1)
	{
		cls_show_msg("line %d:刷卡统计失败(%s,%s)",__LINE__,sBuf2,sBuf1);
		return;
	}

	cls_show_msg1(2, "刷卡统计成功");
	return;
}

#if PRN_ENABLE
//case 1.2:打印总长度
void PrintLenTest(void)
{
	unsigned int unLen=0;
	char sBuf1[128]={0};
	char sBuf2[128]={0};
	int ret = 0;
	
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_PRN_LEN, &unLen, sBuf1))!=NDK_OK)
	{	
		cls_show_msg("line %d:获取打印长度失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	if(cls_show_msg("当前打印总长度:%smm,[确认]正确,[其他]错误",sBuf1) != ENTER)
	{
		cls_show_msg("line %d: 获取打印长度失败", __LINE__);
		return;
	}
	//防止开机直接进行打印测试未进行初始化而引起程序跑飞，增加初始化操作 20130531 zhengdc
	if (FAIL==NDK_PrnInit(g_PrnSwitch))
	{
		cls_show_msg("line %d: 打印机复位失败", __LINE__);
		return;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg("line %d:打印机状态异常!", __LINE__);
		return;                         
	}
	print_bill();
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_PRN_LEN, &unLen, sBuf2))!=NDK_OK)
	{	
		cls_show_msg("line %d:获取打印长度失败(%d,%s)",__LINE__,ret,sBuf2);
		return;
	}
	if(cls_show_msg("请手工测量打印的票据长度是否为:%d mm,[确认]正确,[其他]错误",atoi(sBuf2) - atoi(sBuf1)) != ENTER)//修改判断方式为人工判断 linwl20130717
	{
		cls_show_msg("line %d: :打印统计失败(%s,%s)",__LINE__,sBuf2,sBuf1);
		return;
	}
	/*
	if (atoi(sBuf2) - atoi(sBuf1) != 632)
	{
		cls_show_msg("line %d:打印统计失败(%s,%s)",__LINE__,sBuf2,sBuf1);
		return;
	}
	*/
	cls_show_msg1(2,"打印统计成功");
	return;
}
#else
void PrintLenTest(void){return;}
#endif

//case 1.3:开机运行时间
void RunTimeTest(void)
{
	unsigned int unLen=0;
	char sBuf1[128]={0};
	char sBuf2[128]={0};
	int ret = 0;
	
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, &unLen, sBuf1))!=NDK_OK)
	{	
		cls_show_msg("line %d:开机运行时间统计失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	if(cls_show_msg("开机运行时间:%s秒,[确认]正确,[其他]错误",sBuf1) != ENTER)
	{
		cls_show_msg("line %d:开机运行时间统计失败(%s)",__LINE__,sBuf1);
		return;
	}
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, &unLen, sBuf1))!=NDK_OK)
	{	
		cls_show_msg("line %d:开机运行时间统计失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	NDK_SysMsDelay(3000);
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, &unLen, sBuf2))!=NDK_OK)
	{	
		cls_show_msg("line %d:开机运行时间统计失败(%d,%s)",__LINE__,ret,sBuf2);
		return;
	}
	if (atoi(sBuf2) - atoi(sBuf1) < 2 || atoi(sBuf2) - atoi(sBuf1) > 4)
	{
		cls_show_msg("line %d:开机运行时间统计失败(%s,%s)",__LINE__,sBuf2,sBuf1);
		return;
	}

	cls_show_msg1(2,"开机运行时间统计成功");
	return;
}

//case 1.4:按键次数
void KeyCountTest(void)
{
	unsigned int unLen=0;
	char sBuf1[128]={0};
	char sBuf2[128]={0};
	int ret = 0;
	
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_KEY_COUNT, &unLen, sBuf1))!=NDK_OK)
	{	
		cls_show_msg("line %d:按键统计失败(%d,%s)",__LINE__,ret,sBuf1);
		return;
	}
	if(cls_show_msg("当前按键次数:%s,[确认]正确,[其他]错误",sBuf1) != ENTER)
	{
		cls_show_msg("line %d:按键统计失败(%s)",__LINE__,sBuf1);
		return;
	}
	if((ret = NDK_SysGetPosInfo(SYS_HWINFO_GET_KEY_COUNT, &unLen, sBuf2))!=NDK_OK)
	{	
		cls_show_msg("line %d:按键统计失败(%d,%s)",__LINE__,ret,sBuf2);
		return;
	}
	if (atoi(sBuf2) - atoi(sBuf1) != 1)
	{
		cls_show_msg("line %d:按键统计失败(%s,%s)",__LINE__,sBuf2,sBuf1);
		return;
	}

	cls_show_msg1(2,"按键统计成功");
	return;
}

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		      linwei	        20130121	 	created
*****************************************************************/
void systest31(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	//以下为局部变量
	int nKeyin = 0;
	
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	while(1)
	{
		nKeyin=cls_show_msg("1.刷卡总数\n"		
					 	 	"2.打印总长度\n"
					 		"3.开机运行时间\n"	
					 		"4.按键次数\n");
		switch(nKeyin)
		{
		case '1':
				CardCountTest();
				break;
		case '2':
				PrintLenTest();
				break;
		case '3':
				RunTimeTest();
				break;
		case '4':
				KeyCountTest();
				break;
		case ESC:
				return;
				break;
		default:
				continue;
				break;
		}
	}
	return;
}

