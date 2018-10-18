/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util5.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_IntToC4  和NDK_C4ToInt    接口功能
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
#define 	TESTAPI		"NDK_IntToC4,NDK_C4ToInt"

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
void util5(void)
{
	//以下为局部变量
	uint unum = 0;
	uchar sbuf[10] = {0};
	uchar c4buf[4] = {0xff,0xff,0xff,0xff};
	int ret = 0, i = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:异常测试:各个参数为NULL,应返回失败
	if((ret=NDK_IntToC4(NULL,unum))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_C4ToInt(NULL,sbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_C4ToInt(&unum,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试:输入一个整数356789，则其转成四字节字符为0x571B5
	memset(sbuf,0,sizeof(sbuf));
	if((ret=NDK_IntToC4(sbuf,356789))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	c4buf[0] = 0x00;
	c4buf[1] = 0x05;
	c4buf[2] = 0x71;
	c4buf[3] = 0xB5;
	for(i=0;i<4;i++)
	{
		if(sbuf[i]!=c4buf[i])
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}	
	}
	//正常测试:输入字符数组0x571B5，则其转换成整数为356789
	unum = 0;
	if((ret=NDK_C4ToInt(&unum,c4buf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(unum!=356789)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:输入数为-1，其转成四字节字符为0xFFFFFFFF,输入数为4294967296，其转换为四字节字符0x00000000
	memset(sbuf,0,sizeof(sbuf));
	if((ret=NDK_IntToC4(sbuf,-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(i=0;i<4;i++)
	{
		if(sbuf[i]!=0xff)
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}		
	}
	memset(sbuf,0,sizeof(sbuf));
	if((ret=NDK_IntToC4(sbuf,4294967296))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(i=0;i<4;i++)
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

