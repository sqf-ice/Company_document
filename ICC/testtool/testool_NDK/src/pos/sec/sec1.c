/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec1.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetVer接口功能
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
#define TESTAPI	"NDK_SecGetVer"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*****************************************************************/
void sec1(void)
{
	int ret = -1;
	uchar ucVer[17]={0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//case1:传入非法参数
	if((ret = NDK_SecGetVer(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:测试正常获取版本
	if((ret = NDK_SecGetVer(ucVer)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	send_result("%s测试通过(ver:%s)", TESTAPI, ucVer);
	return;
}

