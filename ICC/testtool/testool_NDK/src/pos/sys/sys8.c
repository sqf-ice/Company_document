/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys8.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysTimeBeep接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysTimeBeep"

#define	MAXWAITTIME 30
#define WUCHASEC 0.015
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
*			  		        linwei	           20130121	 	created
*****************************************************************/
void sys8(void)
{
	if(auto_flag==1)
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTAPI);
	//以下为局部变量
	int ret = -1, nKeyin = 0;
	float fTimeElapsed = 0.0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置
#if !K21_ENABLE
	NDK_SysSetBeepVol(5);
#endif
	//case1:异常测试，错误参数
	if((ret=NDK_SysTimeBeep(0,1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysTimeBeep(1,0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysTimeBeep(4001,1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:测试最小边界值
	if((ret=NDK_SysTimeBeep(1, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:蜂鸣器声音持续时间差值必须在误差以内
	cls_printf("蜂鸣器将连续响5秒");
	lib_StartStopwatch();
	if((ret=NDK_SysTimeBeep(2000, 5000))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	fTimeElapsed=lib_ReadStopwatch();
	if((fTimeElapsed-5)>=WUCHASEC)
	{
		send_result("line %d:%s测试失败(%f)", __LINE__, TESTAPI, fTimeElapsed);
		RETURN;
	}
	
	//case4: 按100,1000,4000频率响1秒
	cls_show_msg1(3, "测试低中高频率蜂鸣响应情况");
	if((ret=NDK_SysTimeBeep(100,1000)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysTimeBeep(1000,1000)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysTimeBeep(4000,1000)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if !K21_ENABLE
	nKeyin = cls_show_msg1(MAXWAITTIME, "高中低频率各蜂鸣1秒,[确认]通过,[其它]失败");
#else
	nKeyin = cls_show_msg1(MAXWAITTIME, "低端平台连续蜂鸣3秒,[确认]通过,[其它]失败");//低端平台不支持频率参数，蜂鸣效果一样 20141022 linwl
#endif
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;	
}

