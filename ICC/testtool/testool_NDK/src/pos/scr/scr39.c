/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 39.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试NDK_ScrSetBrightness能否正常控制液晶背光
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrSetBrightness"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr39
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	      sull             20180425         created
*****************************************************************/
void scr39(void)
{
	/*private & local definition*/
	int ret = 0, i = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//case1:参数异常:val分别为0,21
	if((ret = NDK_ScrSetBrightness(0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_ScrSetBrightness(21)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:关闭背光的情况下，设置背光参数应该自动开启背光且生效
	cls_show_msg1(5,"5秒后即将关闭背光，请不要按键，关闭后自动打开背光,且屏幕逐渐变亮");
	if((ret=NDK_ScrBackLight(BACKLIGHT_OFF))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);
	for(i=1;i<=20;i++)
	{
		if((ret = NDK_ScrSetBrightness(i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		NDK_SysMsDelay(500);
	}
	if(cls_show_msg1(MAXWAITTIME,"是否自动开启背光,且屏幕逐渐变亮,[ENTER]通过,[其它]不通过")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:背光亮度设置为从大到小，应能正常显示
	cls_show_msg1(5,"5秒后请观察屏幕是否逐渐变暗");
	if((ret=NDK_ScrBackLight(BACKLIGHT_ON))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=20; i>=1; i--)
	{
		if((ret = NDK_ScrSetBrightness(i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		NDK_SysMsDelay(500);
	}
	if(cls_show_msg1(MAXWAITTIME,"屏幕亮度是否逐渐变暗,[ENTER]通过,[其它]不通过")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	send_result("%s测试通过", TESTAPI);
	goto ERR;
	
ERR:
	NDK_ScrBackLight(BACKLIGHT_ON);
	NDK_ScrSetBrightness(12); //恢复默认设置
	return;
}


