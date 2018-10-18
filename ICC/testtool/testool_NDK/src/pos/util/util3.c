/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util3.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试NDK_FmtAmtStr  接口功能
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
#define 	TESTAPI		"NDK_FmtAmtStr"

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
void util3(void)
{
	//以下为局部变量
	uchar target[16] = {0};
	int len = 30, ret = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
					 
	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_FmtAmtStr(NULL,target,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_FmtAmtStr((uchar *)"75.36",NULL,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_FmtAmtStr((uchar *)"75.36",target,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:pszSource字符串格式不正确，如75.1.2，返回失败
	len = sizeof("75.1.2")+1;
	if((ret=NDK_FmtAmtStr((uchar *)"75.1.2",target,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}  

	//case3:输入长度小于输入字符串的长度，返回失败
	len = 0;
	if((ret=NDK_FmtAmtStr((uchar *)"75.12",target,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:pszSource为.987,则其返回值为?
	len = sizeof(".987")+1;
	if((ret=NDK_FmtAmtStr((uchar *)".987",target,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:正常测试:pszSource为755.153，则其返回值为75515,返回NDK_OK
	len = sizeof("755.153")+1;
	if((ret=NDK_FmtAmtStr((uchar *)"755.153",target,&len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("755153",(char *)target))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

