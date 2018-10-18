/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys32.c
* Author 			:  
* version			: 
* DATE				:  
* directory 		: 
* description		: 测试NDk_SysGetK21Version 获取k21端指令集版本号的功能
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDk_SysGetK21Version"	 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	       jiangym  	20160603   created
*****************************************************************/
void sys32(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int ret = -1;
	char verstr[32] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1:异常测试:参数非法 
	if((ret=NDk_SysGetK21Version(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试:获取k21端指令集版本号
	if((ret=NDk_SysGetK21Version(verstr))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过(指令集Ver:%s)", TESTAPI, verstr);
	return;	
}

