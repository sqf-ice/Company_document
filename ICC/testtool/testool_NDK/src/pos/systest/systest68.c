/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest68.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 休眠压力测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"休眠压力测试"

#define	DEFAULT_CNT_STR	"100"
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
* history 		 		: author			date			remarks
*			  						huangjianb	20140625	created
*****************************************************************/
static void SysGoSuspend_ot(void)
{
	/*process body*/
	while(1)
	{
		if(ESC==cls_show_msg1(60, "请等待自动进入休眠后按电源键唤醒，或等待180秒自动唤醒..."))
			break;		
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME, "自动休眠唤醒压力测试完成");
	return;
}

static void SysGoSuspend_hm(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = 0, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认休眠测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 1,sizeof(CntStr)-1,&unLen, INPUTDISP_OTHER, 5,INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default
			bak = cnt = DEFAULT_CNT_VLE;
	else
			bak = cnt = atoi(CntStr);	
			
	while(cnt)
	{
		if(ESC==cls_show_msg1(5, "函数休眠，电源键唤醒或180秒后自动唤醒按电源键唤醒，ESC退出" ))
			break;	
		cnt--;

#if ANDIROD_ENABLE
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
		if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
		{
			cls_show_msg1(g_keeptime, "line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
		succ++;
		
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"函数休眠测试完成，成功%d次", succ);
	return;
}


void systest68(void)
{
	int ret=0, keyin = 0, wtime = 180;
	
	if((ret=NDK_SysSetSuspendDuration(wtime)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if(auto_flag==1)//autotest_flag
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTAPI);

		//执行函数休眠压力
		cls_show_msg1(2, "函数休眠测试...");
		SysGoSuspend_hm();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束",TESTAPI);
		NDK_SysSetSuspendDuration(0);
		return;
	}
	while(1)
	{
		keyin = cls_show_msg("1.自动休眠压力\n"
							"2.函数休眠压力");
		
		switch(keyin) 
		{
		case '1':// 1.自动休眠;
			cls_show_msg1(2, "自动休眠测试...");
			SysGoSuspend_ot();
			break;	
		case '2':// 2.函数休眠；
			cls_show_msg1(2, "函数休眠测试...");
			SysGoSuspend_hm();
			break;
		case ESC:
			NDK_SysSetSuspendDuration(0);
			return;
			break;	
		default:
			continue;
			break;
		}
	}
	return;
}
