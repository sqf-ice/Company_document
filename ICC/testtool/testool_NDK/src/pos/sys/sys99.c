/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys99.c
* Author 			: linwl
* version			: 
* DATE				: 20140621
* directory 		: 
* description		: “自动运行”功能的压力测试用例
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"自动运行压力"

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
*			  		        linwl	           20140621	 	created
*****************************************************************/
void sys99(void)
{
	//以下为局部变量
	int ret = -1;
	
	//以下为函数体
#if 0
	NDK_SysDelay(50);//等待PMS完全运行
	if((ret=NDK_SysReboot())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	cls_show_msg("界面显示本信息则自动重启失败，测试失败");
	return;
#else
	cls_printf("重启中...请稍候");
	while(1)
	{//跟郑凯沟通的结果是NDK_SysReboot函数在PMS未启动情况下会失败，	开发建议失败情况下不断尝试重启，某次能够重启就是正常的
		NDK_SysDelay(5);
		if((ret=NDK_SysReboot())!=NDK_OK)
		{
			send_result("重启失败(%d),继续尝试中,能重启则为正常情况,长时间停留在本界面则为异常情况", ret);//该结果的显示会
			continue;
		}
	}
#endif
}

