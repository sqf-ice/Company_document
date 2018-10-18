/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys9.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysSetSuspend接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysSetSuspend"
#define	MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		        linwei	           20130121	 	created
*****************************************************************/
void sys9(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int ret = -1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1: 异常测试，错误参数
	if((ret=NDK_SysSetSuspend(2)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:按不同系统设置测试
	if(cls_show_msg("支持使用电池并确保没插电源,[确认]进入,[其他]跳过") != ENTER)
	{
		send_result("line %d:%s测试跳过", __LINE__, TESTAPI);
		GOTOERR;
	}
	ret=cls_show_msg("选择已设的休眠状态:\n1.从不休眠\n2.休眠时间20秒");//设置中设置休眠和不休眠可以验证NDK_SysSetSuspend函数不受设置中的设置模式影响
	switch(ret)
	{
	case '1':
		//case2.1: 不允许自动进入休眠
		if((ret=NDK_SysSetSuspend(0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if (cls_show_msg1(30, "请不要按键等待30秒,不应进入休眠.若进入休眠,则为BUG") != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		cls_show_msg1(2, "子用例测试通过!");
		
		//case2.2: 允许自动进入休眠
		if((ret=NDK_SysSetSuspend(1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if (cls_show_msg1(30, "请不要按键等待30秒,不应进入休眠.若进入休眠,则为BUG") != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		break;
	case '2':	
		//case2.3: 不允许自动进入休眠 20140603 modified by jiangym
		if((ret=NDK_SysSetSuspend(0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if (cls_show_msg1(25, "1.请不要按键等待25秒,不应进入休眠.若进入休眠,则为BUG") != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case2.4:连续两次不允许自动进入休眠
		if((ret=NDK_SysSetSuspend(0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if (cls_show_msg1(25, "2.请不要按键等待25秒,不应进入休眠.若进入休眠,则为BUG") != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case2.5:允许自动进入休眠，应该可以进入休眠说明最后一次设置有效，不需要成对使用该函数 
		if((ret=NDK_SysSetSuspend(1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(25,"1.请不按键等待20秒,应进入休眠.若进入休眠,马上按键唤醒");
		NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
		if (cls_show_msg("在第20秒时是否进入休眠,是[确认],否则[其它]") != ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case2.6:再调用一次允许自动进入休眠
		if((ret=NDK_SysSetSuspend(1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(25,"2.请不按键等待20秒,应进入休眠.若进入休眠,马上按键唤醒");
		NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
		if (cls_show_msg("在第20秒时是否进入休眠,是[确认],否则[其它]") != ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case2.7:验证定时器清零功能 20140610  add  jiangym
		cls_printf("耐心等待18s,等待过程不应进入休眠(若进入休眠,则为BUG)");
		sleep(18);
		if((ret=NDK_SysSetSuspend(0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_SysSetSuspend(1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_printf("耐心等待5s,等待过程不应进入休眠(若进入休眠,则为BUG)");
		sleep(5);
		if(ENTER!=cls_show_msg("5秒的等待过程中不进休眠,是[确认],否则按[其它]"))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case2.8:不允许自动进入休眠应该生效
		if((ret=NDK_SysSetSuspend(0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if (cls_show_msg1(30, "3.请不要按键等待30秒,不应进入休眠.若进入休眠,则为BUG") != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		break;
	default:
		send_result("line %d:%s测试跳过", __LINE__, TESTAPI);
		RETURN;
	}
	//测试结束
	send_result("%s测试通过(请设置其它休眠状态,重测本用例)", TESTAPI);
	goto ERR;
ERR:
	NDK_SysSetSuspend(1);//测试完打开休眠开关，避免退出本测试后无法自动休眠，引起测试判断错误 20140509 linwl
	return;
}

