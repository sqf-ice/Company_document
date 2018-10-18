/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util14.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试NDK_IsLeapYear接口功能
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_IsLeapYear"

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
*			  	  	     jiangym         20130110  	   created
*****************************************************************/
void util14(void)
{
	//以下为局部变量
	int ret = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:nYear为非法数-1返回失败
	if((ret=NDK_IsLeapYear(-1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试:nYear为2006 或2008，测其结果
	if((ret=NDK_IsLeapYear(2006))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IsLeapYear(2008))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	 	
	send_result("%s测试通过", TESTAPI);
	return;
}

