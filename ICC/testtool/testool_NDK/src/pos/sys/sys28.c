/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys28.c
* Author 			: zhengry
* version			: 
* DATE				: 20150727
* directory 		: 
* description		: 本例测试NDK_SysWakeUp休眠唤醒功能
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysWakeUp"	 
#define WAITTIME 30
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
* history 		 		: author		date		remarks
*			  	  	  zhengry       20150727       created
*****************************************************************/
void sys28()//无参数无返回
{
	/*private & local definition*/
	int ret=0;
	
	if(auto_flag==1)
	{
		//send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*process body*/
	cls_printf("测试%s...", TESTAPI);  

	//测试前置 

	//case1:3s后进入休眠，调用唤醒函数后，应该立即唤醒
	cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//NDK_SysDelay(5);//由于ndk.so中调用该函数会发送指令给k21端也会唤醒休眠,所以不能用该函数
	usleep(5000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}

	if(cls_show_msg("是否已开启:K21长时间不工作,安卓端会调用休眠函数让K21进入休眠的功能,是按[ENTER],否则按其它")!=ENTER)
	{
		send_result("%s用户取消测试", TESTAPI);
		return;
	}
	
	//case2:打开POS自动休眠，等待休眠后，调用唤醒函数后，应该立即唤醒
	cls_show_msg("等待K21自动进入休眠,休眠后按任意键继续,5s后将自动唤醒");//此处N900AZ的K21端会在30秒(安卓框架设计)由于安卓框架部分调用NDK_SysGoSuspend函数进入休眠，该功能看起来类似自动休眠，但实质还是函数休眠
	//NDK_SysDelay(5);
	usleep(5000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if(cls_show_msg("已唤醒,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}

	//case3:休眠唤醒后,IC卡/射频/打印/触屏都可以正常工作。

	send_result("%s测试通过", TESTAPI);
	return;	
}

