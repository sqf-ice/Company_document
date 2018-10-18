/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util11.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试NDK_AddSymbolToStr 接口功能
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
#define 	TESTAPI		"NDK_AddSymbolToStr"

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
void util11(void)
{
	//以下为局部变量
	uchar zstr[16] = {0};
	int len = 10;
	uchar ch = 'A';
	int op = 0, ret = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:参数为NULL情况，应返回失败
	if((ret=NDK_AddSymbolToStr(NULL,len,ch,op))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:nlen为-1情况，应返回失败
	if((ret=NDK_AddSymbolToStr(zstr,-1,ch,op))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:pszString 字符串长度>nlen情况，应返回失败
	strcpy((char *)zstr,"12ab34de7");
	if((ret=NDK_AddSymbolToStr(zstr,strlen((char *)zstr)-1,ch,op))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if 0
	//case4:pszString缓冲的长度<nlen情况，实际会补充字符到缓冲区长度后成功返回
	//这个case是否需要考虑??// 这个用例可能会引起后续测试异常，因为会溢出，故屏蔽该用例 20150125 linwl
	strcpy((char *)zstr,"12ab34de7");
	if((ret=NDK_AddSymbolToStr(zstr,sizeof(zstr)+1,ch,op))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, zstr);
		RETURN;
	}
#endif

	//case5:nOption参数为3 或-1，应返回失败
	if((ret=NDK_AddSymbolToStr(zstr,len,ch,3))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AddSymbolToStr(zstr,len,ch,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:正常测试:pszString[9]为"ab12cde",ch为A,分别测试nOption为0,1,2三种情况
	strcpy((char *)zstr,"ab12cde");
	if((ret=NDK_AddSymbolToStr(zstr,9,ch,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("AAab12cde",(char *)zstr))
	{
		send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, zstr);
		RETURN;
	}
	strcpy((char *)zstr,"ab12cde");
	if((ret=NDK_AddSymbolToStr(zstr,9,ch,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("ab12cdeAA",(char *)zstr))
	{
		send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, zstr);
		RETURN;
	}
	strcpy((char *)zstr,"ab12cde");
	if((ret=NDK_AddSymbolToStr(zstr,11,ch,2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("AAab12cdeAA",(char *)zstr))
	{
		send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, zstr);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

