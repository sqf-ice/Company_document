/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util6.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_IntToC2  和NDK_C2ToInt    接口功能
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
#define 	TESTAPI		"NDK_IntToC2,NDK_C2ToInt"

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
void util6(void)
{
	//以下为局部变量
	uint unum = 0;
	uchar sbuf[5] = {0};
	uchar c2buf[2] = {0xff,0xff};
	int ret = 0, i = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_IntToC2(NULL,unum))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_C2ToInt(NULL,sbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_C2ToInt(&unum,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试:输入一个整5876，其转换成字符数组0x16F4
	if((ret=NDK_IntToC2(sbuf,5876))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	c2buf[0] = 0x16;
	c2buf[1] = 0xF4;
	for(i=0;i<2;i++)
	{
		if(sbuf[i]!=c2buf[i])
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}
	}
	unum = 0;
	if((ret=NDK_C2ToInt(&unum,c2buf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(unum!=5876)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:unNum超出其范围的数如65536,或-1,其返回值均为0 
	memset(sbuf,0,sizeof(sbuf));
	if((ret=NDK_IntToC2(sbuf,65536))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(i=0;i<2;i++)
	{
		if(sbuf[i]!=0x00)
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}
	}
	memset(sbuf,0,sizeof(sbuf));
	if((ret=NDK_IntToC2(sbuf,-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(i=0;i<2;i++)
	{
		if(sbuf[i]!=0x00)
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

