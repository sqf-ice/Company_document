/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: timer模块
* file name		: timer3.c
* Author 			: huangbs
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: 测试NDK_RegisterTimerRoutine接口功能
* related document	: NDK.chm
* 
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_RegisterTimerRoutine"

#if CPU5810X_ENABLE
static void func1(void)
{
	static int i=0;

	if(i<600)
		++i;//cls_printf("第%d次回调函数func(10ms后将更新次数)", ++i);
	else
		cls_printf("停止计时，秒表计时时间应为6秒，请判断是否测试通过!");
}
#endif

static void func(void)
{
	static int i=0;

	if(i<5)
		cls_printf("第%d次回调函数func(3s后将更新次数)", ++i);
	else
		cls_printf("测试完毕,请判断是否测试通过.请关机重启!");
}

/****************************************************************
* function name 	 	: void timer3(void)
* functional description 	: 注册延时回调函数
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  linrq      huangbs	created
*****************************************************************/
void timer3(void)
{
	int ret= -1;

	cls_printf("测试%s...", TESTAPI); 

	//case 1 传入非法参数
	if((ret=NDK_RegisterTimerRoutine(10, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
#if CPU5810X_ENABLE   //针对5810x平台产品此函数精度测试增加测试点
	//case 3:回调精度达到10ms	2018003 sull add
	cls_show_msg1(30,"请准备秒表，按任意键开始计时");
	cls_printf("计时中...");
	if(NDK_OK!=(ret=NDK_RegisterTimerRoutine(7, func1)))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(7);
#endif

	//case 2,注册延时回调函数,之后以time_interval间隔不断自动回调
	if(NDK_OK!=(ret=NDK_RegisterTimerRoutine(ONESECOND*3, func)))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	while(1);
	//send_result("%s测试通过", TESTAPI);
	//return;
}

