/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util8.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_IntToBcd   和NDK_BcdToInt     接口功能
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
#define 	TESTAPI		"NDK_IntToBcd ,NDK_BcdToInt "

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
void util8(void)
{
	//以下为局部变量
	uchar bbuf[2] = {0x00,0x00}, cbuf[2] = {0x00,0x00};
	int num = 0, i = 0;
	int len = 2, ret = 0;

	//以下为函数体 
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:异常测试:各个参数为NULL,应返回失败
	if((ret=NDK_IntToBcd(NULL,&len,num))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IntToBcd(bbuf,NULL,num))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BcdToInt(NULL,&num))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BcdToInt(bbuf,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:nNum为-1或10000的测试，结果返回失败
	if((ret=NDK_IntToBcd(bbuf,&len,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IntToBcd(bbuf,&len,10000))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//psBcd输入的不是0到9的BCD码 比如0xaabb，应该返回失败
	bbuf[0] = 0xaa;
	bbuf[1] = 0xbb;
	if((ret=NDK_BcdToInt(bbuf,&num))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:参数pnBcdLen长度小于2，返回失败 
	len = 1;
	if((ret=NDK_IntToBcd(bbuf,&len,56))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:正常测试:输入整数nNum 1234，则其返回判断psBcd是0x1234nBcdLen为2
	memset(bbuf,0,sizeof(bbuf));
	len = 2;
	if((ret=NDK_IntToBcd(bbuf,&len,1234))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cbuf[0] = 0x12;
	cbuf[1] = 0x34;
	for(i=0;i<2;i++)
	{
		if(bbuf[i]!=cbuf[i])
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI, i);
			RETURN;
		}
		
	}
	if(len!=2)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	num = 0;
	if((ret=NDK_BcdToInt(bbuf,&num))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(num!=1234)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}

