/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util2.c
* Author 			: jiangym
* version			:
* DATE				: 20130107
* directory 	:
* description	: 测试NDK_IncNum接口功能
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
#define 	TESTAPI		"NDK_IncNum"

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
void util2(void)
{
	//以下为局部变量
	uchar strnum[10] = {0};
	int ret = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例 	

	//case1:异常测试:pszStrNum参数为NULL,应返回失败
	if((ret=NDK_IncNum(NULL))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:pszStrNum参数长度大于6，如1234567，应返回失败
	strcpy((char *)strnum,"1234567");
	if((ret=NDK_IncNum(strnum))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:pszStrNum参数如果为数字与字母的组合如123hf,应该返回失败
	strcpy((char *)strnum,"123hf");
	if((ret=NDK_IncNum(strnum))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)(%s)", __LINE__, TESTAPI, ret, strnum);
		RETURN;
	}
	
	//case4:pszStrNum参数长度小于6，应返回成功
	strcpy((char *)strnum,"12345");
	if((ret=NDK_IncNum(strnum))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("012346",(char *)strnum))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case4:正常测试:pszStrNum为999999，返回值是000001，返回NDK_OK
	strcpy((char *)strnum,"999999");
	if((ret=NDK_IncNum(strnum))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("000001",(char *)strnum))
	{
		send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, strnum);
		RETURN;
	}
	
	//case5:正常测试:pszStrNum为123456，返回值是123457，返回NDK_OK
	strcpy((char *)strnum,"123456");
	if((ret=NDK_IncNum(strnum))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("123457",(char *)strnum))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}
