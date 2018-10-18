/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util7.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_ByteToBcd   和 NDK_BcdToByte    接口功能
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
#define 	TESTAPI		"NDK_ByteToBcd,NDK_BcdToByte"

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
void util7(void)
{
	//以下为局部变量
	int num = 0, ret = 0 ;
	uchar ch = 0; 
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_ByteToBcd(num,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BcdToByte(ch,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2: nNum是-1与100 的测试，应返回失败
	if((ret=NDK_ByteToBcd(100,&ch))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ByteToBcd(-1,&ch))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//输入的不是0到9的BCD码ch如0xb，应返回失败
	ch = 0xb;
	if((ret=NDK_BcdToByte(ch,&num))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:正常测试:nNum为62，则其返回的ch 为0x62
	ch = 0x0;
	if((ret=NDK_ByteToBcd(62,&ch))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(ch!=0x62)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	num = 0;
	if((ret=NDK_BcdToByte(ch,&num))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(num!=62)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

