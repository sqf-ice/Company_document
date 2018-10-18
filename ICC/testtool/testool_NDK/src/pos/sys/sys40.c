/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys40.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_SysSetWeakupDelay接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysSetWeakupDelay"

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
*			  		         sull             20180709              created
*****************************************************************/
void sys40(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试,请使用手动测试验证",TESTAPI);
		return;
	}
	
	//以下为局部变量
	int nRet = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);  

	//case5:传入负数，会变成一个很大的正数
	cls_show_msg("按任意键进入休眠,休眠后等待1分钟手动唤醒,手动唤醒后红灯亮起");
	if((nRet=NDK_SysSetWeakupDelay(-1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_LedStatus(LED_RED_X_ON/*LED_RFID_RED_ON*/);
	if(cls_show_msg("进入休眠后短时间内不会自动唤醒?是[ENTER],否[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}	

	//case1:设置休眠10s后能够自动唤醒
	cls_show_msg("按任意键进入休眠,10s后应该自动唤醒休眠且蓝灯亮起");
	if((nRet=NDK_SysSetWeakupDelay(10)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_LedStatus(LED_BLUE_X_ON/*LED_RFID_BLUE_ON*/);
	if(cls_show_msg("进入休眠10s后自动唤醒,且蓝灯亮起?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case2:未设置休眠唤醒时间，应只能通过手动唤醒
	cls_show_msg("按任意键进入休眠,等待1分钟后按任意键唤醒休眠,唤醒的时候红灯应该亮起");
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_LedStatus(LED_RED_X_ON/*LED_RFID_RED_ON*/);
	if(cls_show_msg("进入休眠后不会自动唤醒,只能手动唤醒?是[ENTER],否[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
		
	//case3:设置休眠唤醒时间为0s，应无效
	cls_show_msg("按任意键进入休眠,进入休眠后等待10s后按任意键唤醒休眠,唤醒时蓝灯亮起");
	if((nRet=NDK_SysSetWeakupDelay(0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_LedStatus(LED_BLUE_X_ON/*LED_RFID_BLUE_ON*/);
	if(cls_show_msg("进入休眠后需按键才能唤醒,且唤醒时蓝灯亮起?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
		
	//case4:设置休眠唤醒时间为5分钟，5分钟后应能自动唤醒
	cls_show_msg("按任意键进入休眠,耐心等待5分钟，5分钟后应该自动唤醒休眠且蓝灯亮起");
	if((nRet=NDK_SysSetWeakupDelay(5*60)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_LedStatus(LED_BLUE_X_ON/*LED_RFID_BLUE_ON*/);
	if(cls_show_msg("进入休眠5分钟后自动唤醒,且蓝灯亮起?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}	

