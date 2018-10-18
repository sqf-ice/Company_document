/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 14.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrBackLight能否开关LCD背光操作
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrBackLight"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr14
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr14(void)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//前置:禁止进入休眠
#if !K21_ENABLE
	NDK_SysSetSuspend(0);
#endif

	//case1:参数异常:emBL分别为-1,5
	if((ret=NDK_ScrBackLight(-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrBackLight(5))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_ScrBackLight(BACKLIGHT_OFF))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(5, "已关闭背光,请勿按键等待5秒后打开背光");
	
	//case2:打开背光	
	if((ret=NDK_ScrBackLight(BACKLIGHT_ON))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(MAXWAITTIME,"已打开背光!请在背光关闭前快速按任意键将关闭背光");

	//case3:关闭背光	
	sleep(1);
	if((ret=NDK_ScrBackLight(BACKLIGHT_OFF))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:背光常亮
#if !K21_ENABLE //K21平台产品不支持BACKLIGHT_LOCKON，所以背光长亮相关用例不测试
	cls_show_msg1(5, "已关闭背光,5秒后将设置背光常亮请耐心等待60秒");
	if((ret=NDK_ScrBackLight(BACKLIGHT_LOCKON))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(60);//死等60秒,直到背光自动关闭时间到了,还一直亮着说明背光常亮设置有效
	if(cls_show_msg1(MAXWAITTIME,"背光灯常亮60秒,[ENTER]通过,[其它]不通过")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case 5:常亮后调用关闭应能立马关闭
	cls_show_msg1(5, "请勿按键,5秒后将关闭背光");	
	if((ret=NDK_ScrBackLight(BACKLIGHT_OFF))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(5, "已关闭背光,5秒后将打开背光短亮请耐心等待60秒");

	//case 6:常亮后调用短亮应该会被设置成短亮	
	if((ret=NDK_ScrBackLight(BACKLIGHT_LOCKON))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrBackLight(BACKLIGHT_ON))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(60);
	if(cls_show_msg1(MAXWAITTIME, "背光关闭,[ENTER]通过,[其它]不通过")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
	send_result("%s测试通过", TESTAPI);
	goto ERR;
	//恢复默认设置
ERR://允许进入休眠
#if !K21_ENABLE
	NDK_SysSetSuspend(1);
#endif	
	NDK_ScrBackLight(BACKLIGHT_ON);
	return;
}

