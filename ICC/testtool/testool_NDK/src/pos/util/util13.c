/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util13.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试NDK_IsDigitChar 和NDK_IsDigitStr   接口功能
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
#define 	TESTAPI		"NDK_IsDigitChar,NDK_IsDigitStr "

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
void util13(void)
{
	//以下为局部变量
	int ret = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:ch为NULL,应返回失败
	if((ret=NDK_IsDigitChar(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:ch为a,那么应该返回失败
	if((ret=NDK_IsDigitChar('a'))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:正常测试:ch为3,返回成功
	if((ret=NDK_IsDigitChar('3'))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:异常测试:pszString为NULL,应返回失败
	if((ret=NDK_IsDigitStr(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:pszString为abc23de,则应返回失败
	if((ret=NDK_IsDigitStr((uchar *)"abc23de"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case6:正常测试:pszString为"123456",则返回成功NDK_OK 
	if((ret=NDK_IsDigitStr((uchar *)"123456"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}

