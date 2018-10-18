/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt7.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetVersion接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_PrnGetVersion"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt7(void)
* functional description 	:获取打印驱动的版本信息
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt7(void)
{
	//以下为局部变量
	int nRet = 0;
	char VerBuf[32]={0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	
	//case1:传入一个buf能否真的返回正确的版本号
	if((nRet = NDK_PrnGetVersion(VerBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		return ;
	}
	
	//case2:传入buf为空,应返回错误码
	if((nRet = NDK_PrnGetVersion(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		return ;
	}
	
	//测试结束	
	send_result("%s测试通过(ver:%s)", TESTAPI,VerBuf);
	return;
}

