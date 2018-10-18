/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util10.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试 NDK_LeftTrim  和NDK_RightTrim 和NDK_AllTrim   接口功能
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
#define 	TESTAPI		"NDK_LeftTrim,NDK_RightTrim,NDK_AllTrim"

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
* history 		 		:
*****************************************************************/
void util10(void)
{
	//以下为局部变量
	uchar zbuf[10] = {0};
	int ret = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常情况:pszBuf为NULL,则应返回失败
	if((ret=NDK_LeftTrim(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_RightTrim(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AllTrim(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常情况:有空格的pszBuf为"  ab  cd  "，对于NDK_LeftTrim输出结果为"ab  cd  "
	strcpy((char *)zbuf,"  ab  cd  ");
	if((ret=NDK_LeftTrim(zbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("ab  cd  ",(char *)zbuf))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:有空格的pszBuf为"  ab  cd  ",NDK_RightTrim输出结果为"  ab  cd"
	strcpy((char *)zbuf,"  ab  cd  ");
	if((ret=NDK_RightTrim(zbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("  ab  cd",(char *)zbuf))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case4:有空格的pszBuf为"  ab  cd  ",NDK_AllTrim输出结果为"ab  cd"
	strcpy((char *)zbuf,"  ab  cd  ");
	if((ret=NDK_AllTrim(zbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("ab  cd",(char *)zbuf))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

