/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec8.c
* Author 		: huangjianb
* version		: 
* DATE			: 20140821
* directory 		: 
* description		: 测试NDK_SecSetCfg,NDK_SecGetCfg接口功能
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
#define TESTAPI	"NDK_SecSetCfg,NDK_SecGetCfg"	

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
* history 		 		:  author					date				remarks
*			  	  	  		 huangjianb     20140821		created
*****************************************************************/
void sec8(void)
{
	int ret = 0;
	uint punCfgInfo = 0, punCfg = 0;
	
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI);
	
	//测试前置
	if(((ret=NDK_SecGetCfg(&punCfgInfo))!=NDK_OK)||(punCfgInfo!=6)) //获取当前安全等级
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfgInfo);
		GOTOERR;
	}
	
	//非法测试
	if((ret=NDK_SecGetCfg(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//关闭限制
	if((ret=NDK_SecSetCfg(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_SecGetCfg(&punCfg))!=NDK_OK)||(punCfg!=0))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfg);
		GOTOERR;
	}
	
	//设置密钥唯一性控制位
	if((ret=NDK_SecSetCfg(2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_SecGetCfg(&punCfg))!=NDK_OK)||(punCfg!=2))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfg);
		GOTOERR;
	}
	
	//设置密钥专用性控制位
	if((ret=NDK_SecSetCfg(4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_SecGetCfg(&punCfg))!=NDK_OK)||(punCfg!=4))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfg);
		GOTOERR;
	}
	
	//设置密钥敏感次数限制
	if((ret=NDK_SecSetCfg(8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_SecGetCfg(&punCfg))!=NDK_OK)||(punCfg!=8))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfg);
		GOTOERR;
	}
	
	//设置密钥保护强度
	if((ret=NDK_SecSetCfg(16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_SecGetCfg(&punCfg))!=NDK_OK)||(punCfg!=16))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfg);
		goto ERR;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SecSetCfg(punCfgInfo);
	return;
}
