/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys34.c
* Author 			:  
* version			: 
* DATE				:  
* directory 		: 
* description		: 测试NDK_SysGetPubkeyInfo函数能否获取公钥信息(仅低端支持SP610最先导入)
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetPubkeyInfo"	 

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
*			  	  	       linwl  	20170111   created
*****************************************************************/
void sys34(void)
{
	//以下为局部变量
	int ret = 0;
	char pinfo[128] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1:参数异常测试
	if((ret=NDK_SysGetPubkeyInfo(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_SysGetPubkeyInfo(pinfo))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	send_result("%s测试完成,获取到的公钥信息:%s,请人工判断是否正确", TESTAPI, pinfo);
	return;
}
