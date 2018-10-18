/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys12.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysDelay,NDK_SysMsDelay,NDK_SysStartWatch,NDK_SysStopWatch接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if defined ME15CHX||defined ME15B||defined ME66
#define	TESTAPI		"NDK_SysMsDelay,NDK_SysReadWatch"
#else
#define	TESTAPI		"NDK_SysDelay,NDK_SysMsDelay,NDK_SysStartWatch,NDK_SysStopWatch"
#endif

#define MAXWAITTIME 30
#if K21_ENABLE //K21平台所有浮点显示和计算都无效
#if defined ME15C||defined ME15CHX||defined ME15B||defined ME66
#define WUCHASEC  2 //这个值待定
#else
#define WUCHASEC  1 //这个值待定
#endif
#else
#define WUCHASEC  0.03 //这个值待定
#endif
#define	MAX_WUCHASEC	1.0
#define	TESTTIME	60

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
*			  		      linwei	        20130121	 	created
*****************************************************************/
#if !(defined ME15CHX||defined ME15B||defined ME66)
void sys12(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int i,ret = -1;
	unsigned int unTime = 0, unTime2 = 0;

	//以下为函数体
	cls_printf("测试延时API,测试过程中请勿进入休眠(需要一些时间,请等待3分钟)...");

	//case1: 异常测试，错误参数
	if((ret=NDK_SysStopWatch(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysDelay(0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysMsDelay(0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: NDK_SysDelay延时TESTTIME秒
	NDK_SysStartWatch();
	NDK_SysDelay(TESTTIME*10);//100ms为单位
	NDK_SysStopWatch(&unTime);
#if K21_ENABLE
	if (abs(unTime/1000-60) >= WUCHASEC)
#else
	if (fabs(unTime/1000.0-60.0) >= WUCHASEC)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, unTime);
		RETURN;
	}
	
	//case3: NDK_SysMsDelay延时TESTTIME秒
	NDK_SysStartWatch();
	NDK_SysMsDelay(TESTTIME*1000);// 1ms为单位
	NDK_SysStopWatch(&unTime);
#if K21_ENABLE
	if (abs(unTime/1000-60) >= WUCHASEC)
#else
	if (fabs(unTime/1000.0-60.0) >= WUCHASEC)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, unTime);
		RETURN;
	}

	//case4: 如果多次调用NDK_SysStartWatch，每次调用都会导致跑表重新开始计时。
	for(i=0; i<1000; i++)
		NDK_SysStartWatch();
	NDK_SysStopWatch(&unTime);
#if K21_ENABLE
	if(unTime/1000 >= MAX_WUCHASEC)
#else
	if(unTime/1000.0 >= MAX_WUCHASEC)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, unTime);
		RETURN;
	}

	//case5:多次NDK_SysStopWatch
	NDK_SysMsDelay(1);
	NDK_SysStopWatch(&unTime2);
#if !K21_ENABLE
	if(unTime2 <= unTime)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, unTime2, unTime);
		RETURN;
	}
#endif
	//测试结束
	send_result("跑表,延时API测试通过");
	return;
}
#else
void sys12(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int ret = -1;
	uint unTime = 0, unTime2 = 0, time = 0;

	//以下为函数体
	cls_printf("测试延时API,测试过程中请勿进入休眠(需要一些时间,请等待1分钟)...");

	//case1: 异常测试，错误参数
	if((ret=NDK_SysReadWatch(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysMsDelay(0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: NDK_SysMsDelay延时TESTTIME秒
	NDK_SysReadWatch(&unTime);
	NDK_SysMsDelay(TESTTIME*1000);// 1ms为单位
	NDK_SysReadWatch(&unTime2);
	time = unTime2-unTime;
	if (abs(time/1000-60) >= WUCHASEC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, time);
		RETURN;
	}

	send_result("%s测试通过%d,%d", TESTAPI,unTime,unTime2);
	return;
}
#endif

