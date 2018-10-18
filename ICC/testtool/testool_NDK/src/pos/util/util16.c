/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util16.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试NDK_IsValidDate   接口功能
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
#define 	TESTAPI		"NDK_IsValidDate"

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
void util16(void)
{
	//以下为局部变量
	int ret = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:pszDate为NULL,应返回失败
	if((ret=NDK_IsValidDate(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:pszDate字符串格式为YYYYMMDD如果输入的不是这个格式的，返回失败
	if((ret=NDK_IsValidDate((uchar *)"201304"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:pszDate为19990733，或19008927，或19cd2305，返回失败
	if((ret=NDK_IsValidDate((uchar *)"19990733"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IsValidDate((uchar *)"19cd2305"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:pszDate 不在一定范围内的数如12120304，返回失败
	if((ret=NDK_IsValidDate((uchar *)"12120304"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:润年和非润年2月29日的测试
	//case5.1:正常测试:pszDate为20060229，返回成功NDK_ERR_PARA
	if((ret=NDK_IsValidDate((uchar *)"20060229"))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case5.2:正常测试:pszDate为20080229，返回成功NDK_OK 
	if((ret=NDK_IsValidDate((uchar *)"20080229"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case6:正常测试:pszDate为20150104，返回成功NDK_OK 
	if((ret=NDK_IsValidDate((uchar *)"20150104"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
	send_result("%s测试通过", TESTAPI);
	return;
}

