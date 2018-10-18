/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys19.c
* Author 			: jiangym
* version			: 
* DATE				: 20130821
* directory 			: 
* description			: 测试NDK_SysTime接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysTime"

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
*			  		     
*****************************************************************/
void sys19(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int nRet = 0;
	ulong ultime=0, ultime1=0, subtime=0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:参数异常测试
	if((nRet = NDK_SysTime(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case2:正常测试
	if((nRet = NDK_SysTime(&ultime)) != NDK_OK)//返回经过的秒
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	sleep(5);
	if((nRet = NDK_SysTime(&ultime1)) != NDK_OK)//返回经过的秒
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((subtime=ultime1-ultime)!=5)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, subtime);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

