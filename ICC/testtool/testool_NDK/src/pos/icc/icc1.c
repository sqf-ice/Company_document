/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: 测试NDK_IccGetVersion是否可以正确的获得版本信息
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_IccGetVersion"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc1
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 chenfm     20130107	created
*				  	 
*****************************************************************/
void icc1(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	char version[128]={0};
	int ret= -1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置

	//case1:异常测试，错误参数     
	if((ret=NDK_IccGetVersion(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:获取正确的版本信息
	if((ret=NDK_IccGetVersion(version))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过(Ver:%s)", TESTAPI, version);
	return;
}

