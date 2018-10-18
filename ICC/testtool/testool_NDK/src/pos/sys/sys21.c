/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys21.c
* Author 			: jiangym
* version			: 
* DATE				: 20130926
* directory 			: 
* description			: 测试NDK_SysSetSuspendDuration接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysSetSuspendDuration"

#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE	//589X平台（SP60机型）定时唤醒的精度较低，误差在128秒左右。 所以需要设置大于128的时间才能有效,小于128的时间效果随机
#define WAKETIME	(180)
#else
#define WAKETIME	(60)
#endif

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
*			  		     
*****************************************************************/
void sys21(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int ret = 0;

	//以下为函数体
	//测试前置
	if(ENTER!=cls_show_msg("测试%s,请在管理中分别设置[开深/开浅]模式后按ENTER继续测试", TESTAPI))
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg("请分别对只插电源与只有电池两种情况测试(5892允许唤醒时间存在128秒以内的误差),任意键继续");

	//case1:异常测试:当unSec小于60秒的时候，将返回错误
	if((ret=NDK_SysSetSuspendDuration(1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(59))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:先设置休眠唤醒时间为WAKETIME秒,进休眠,不按键则休眠60秒后会唤醒(测试正常的RTC休眠唤醒功能)
	cls_show_msg1(3,"3s后进休眠,休眠后不按键,%ds后自动唤醒", WAKETIME);
	if((ret=NDK_SysSetSuspendDuration(WAKETIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(3,"已唤醒,3s后再次进入休眠,%ds后自动唤醒", WAKETIME);
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("休眠两次均%ds后自动唤醒,且唤醒时背光亮,正确[Enter],错误[其他]", WAKETIME)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:先设置休眠唤醒时间为WAKETIME秒,再设置休眠唤醒时间为0,进休眠,不按键则休眠60秒后不会唤醒(则参数传入0实现实时取消休眠定时唤醒功能)
	cls_show_msg1(3,"3s后进休眠,等待%ds后不会自动唤醒,需要按键唤醒,唤醒时屏幕会刷新", WAKETIME);
	if((ret=NDK_SysSetSuspendDuration(WAKETIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("屏幕已刷新,休眠%ds后无按键不唤醒,正确[Enter],错误[其他]", WAKETIME)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case4:先设置休眠唤醒时间为WAKETIME+10秒,进休眠,按键唤醒,再进休眠,不按键会唤醒
	cls_show_msg1(3,"3s后进休眠,休眠%ds内请按键唤醒,唤醒时屏幕会刷新", WAKETIME+10);
	if((ret=NDK_SysSetSuspendDuration(WAKETIME+10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	cls_show_msg("已唤醒,屏幕已刷新,按任意键[确认键除外]进入休眠,%ds后自动唤醒", WAKETIME+10);
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("休眠后%ds无按键会唤醒,唤醒时背光亮,正确[Enter],错误[其他]", WAKETIME+10)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:先设置休眠唤醒时间为WAKETIME秒,等待自动进入休眠,不按键则休眠60秒后会唤醒(测试正常的RTC休眠唤醒功能)
	if((ret=NDK_SysSetSuspendDuration(WAKETIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("等待自动进入休眠,休眠后不按键,%ds后自动唤醒,唤醒后按键继续", WAKETIME);
	cls_show_msg("等待再次进入休眠,休眠后不按键,%ds后自动唤醒,唤醒后按键继续", WAKETIME);
	if(cls_show_msg("休眠两次均%ds后自动唤醒,正确[Enter],错误[其他]", WAKETIME)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:先设置休眠唤醒时间为WAKETIME秒,再设置休眠唤醒时间为0,等待自动进入休眠,不按键则休眠60秒后不会唤醒(则参数传入0实现实时取消休眠定时唤醒功能)	
	if((ret=NDK_SysSetSuspendDuration(WAKETIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("等待自动进入休眠,不按键不会自动唤醒,按键唤醒后按键继续");
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("休眠%ds后无按键不唤醒,正确[Enter],错误[其他]", WAKETIME)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case7:先设置休眠唤醒时间为WAKETIME+10秒,等待自动进入休眠,按键唤醒,再等待自动进入休眠,不按键会唤醒
	if((ret=NDK_SysSetSuspendDuration(WAKETIME+10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("等待自动进入休眠,休眠%ds内请按键唤醒,按键唤醒后按键继续", WAKETIME+10);
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	cls_show_msg("已唤醒,等待再次进入休眠,%ds后自动唤醒,唤醒后按键继续", WAKETIME+10);
	if(cls_show_msg("休眠后无按键会唤醒,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SysSetSuspendDuration(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SysSetSuspendDuration(0);
	return;
}

