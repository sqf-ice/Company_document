/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util1.c
* Author 			: jiangym
* version			:
* DATE				: 20130107
* directory 	:
* description	: 测试NDK_AddDigitStr接口功能
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
#define 	TESTAPI		"NDK_AddDigitStr"

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
void util1(void)
{
	//以下为局部变量

	uchar digstr1[14] = {0}, digstr2[14] = {0}, resultstr[14] = {0};
	int len = 13, ret = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_AddDigitStr(NULL, digstr2, resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddDigitStr(digstr1, NULL, resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddDigitStr(digstr1, digstr2, NULL, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddDigitStr(digstr1, digstr2, resultstr, NULL))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:pszDigStr1参数长度超过12，或pszDigStr2参数长度超过12，应返回失败
	if((ret=NDK_AddDigitStr((uchar *)"1234567890123", (uchar *)"1234", resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddDigitStr((uchar *)"1234", (uchar *)"1234567890123", resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:pszDigStr1参数或pszDigStr2参数不合法如123ABC,返回失败
	if((ret=NDK_AddDigitStr((uchar *)"123ABC", (uchar *)"1234", resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddDigitStr((uchar *)"1234", (uchar *)"123abc", resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:两个数之和pszResult超过12情况，返回失败
	memset(resultstr,0,sizeof(resultstr));
	len = 13;
	if((ret=NDK_AddDigitStr((uchar *)"653224567821", (uchar *)"653224567821", resultstr, &len))!=NDK_ERR) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case5:初始化 len为5小于计算出来的长度6,应该返回参数错误
	memset(resultstr,0,sizeof(resultstr));
	len = 5;
	if((ret=NDK_AddDigitStr((uchar *)"1234",(uchar *) "123456", resultstr, &len))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case6:正常测试
	memset(resultstr,0,sizeof(resultstr));
	len = 6;
	if((ret=NDK_AddDigitStr((uchar *)"1234", (uchar *)"123456", resultstr, &len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("124690",(char *)resultstr)||len!=6)
	{
		send_result("line %d:%s测试失败%s,%d", __LINE__, TESTAPI, resultstr, len);
		RETURN;
	}
	
	//case7:正常测试12位数加12位数得到12位数
	memset(resultstr,0,sizeof(resultstr));
	len = 12;
	if((ret=NDK_AddDigitStr((uchar *)"111122221111", (uchar *)"222233332222", resultstr, &len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("333355553333",(char *)resultstr)||len!=12)
	{
		send_result("line %d:%s测试失败%s,%d", __LINE__, TESTAPI, resultstr, len);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}
