/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: timer模块
* file name			: timer4.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_DeRegisterTimer函数能否删除定时器
* related document	: NDK.chm
* 
************************************************************************/
#include "mode.h"
#define TESTAPI	"NDK_DeRegisterTimer"

/****************************************************************
* function name 	 	: void timer4(void)
* functional description 	: 定时器删除函数
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	 		jiangym		20140530	created
*****************************************************************/
static void func(void)
{
	static int i=0;

	if(i<3)
		cls_printf("第%d次回调函数func(2s后将更新次数)", ++i);
	else	
	{
		i = 0;
		NDK_DeRegisterTimer();
	}
	
}

void timer4(void)
{
	int ret= -1;

	cls_printf("测试%s...", TESTAPI); 

	//case 1:注册延时回调函数,之后以time_interval间隔不断自动回调
	if((ret=NDK_RegisterTimerRoutine(2000, func))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	sleep(8);
	if(cls_show_msg("没有出现提示:第4次回调函数func,[Enter]正确,[其他]失败")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束	
	send_result("%s测试通过", TESTAPI);
	return;
}

