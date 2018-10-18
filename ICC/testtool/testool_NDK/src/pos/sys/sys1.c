/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys1.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_Getlibver接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Getlibver"//int NDK_Getlibver (char * pszVer):获取NDK库版本号 pszVer版本号字符串,缓冲大小不低于8字节

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
* history 		 		: author			date			remarks
*			  		      linwei	        20130121	 	created
*****************************************************************/
void sys1(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	char szVer[16]={0};
	int ret= -1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:异常测试，错误参数     
	if((ret=NDK_Getlibver(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:获取正确的版本信息
	if((ret=NDK_Getlibver(szVer)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过(Ver:%s)", TESTAPI, szVer);
	return;
}

