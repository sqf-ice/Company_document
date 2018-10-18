/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: event1模块
* file name	: event1.c
* Author 		: linwl
* version		: 
* DATE			: 20160912
* directory 		: 
* description		: 测试Ndk_beginTransactions函数开启事务,独占安全支付模块.测试Ndk_endTransactions关闭事务,放弃独占安全支付模块.测试Ndk_getStatus获取安全支付模块的状态的功能
* related document	: 高端平台统一API动态链接库编程手册V2.00.doc
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"Ndk_beginTransactions,Ndk_endTransactions,Ndk_getStatus"
#define PINTIMEOUT_MAX	(200*1000)
#define PINTIMEOUT_MIN	(5*1000)

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
*			  	  	   		linwl       20160912	created
*****************************************************************/
void event1(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, get_status = -1;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	
	//case1:异常测试，超时时间非法情况
	if((ret = Ndk_beginTransactions(-1))==NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:第一次正确调用Ndk_beginTransactions应该返回成功,超时时间合法10秒
	if((ret = Ndk_beginTransactions(10))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:事务结束前重复开启事务应该返回NDK_ERR_POSNDK_TRANS_ALREADY
	if((ret = Ndk_beginTransactions(10))!=NDK_ERR_POSNDK_TRANS_ALREADY)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:调用获取状态函数应该返回成功
	if((ret = Ndk_getStatus())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case5:主动结束事务应该成功
	if((ret = Ndk_endTransactions())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case6:主动结束事务后,再调用主动结束应该返回NDK_ERR_POSNDK_TRANS_NOEXIST
	if((ret = Ndk_endTransactions())!=NDK_ERR_POSNDK_TRANS_NOEXIST)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case7:主动事务结束后,再次开启应该成功,超时时间合法3秒
	if((ret = Ndk_beginTransactions(3))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case8:事务超时结束后,再次开启事务应该成功,超时时间合法3秒
	sleep(4);
	if((ret = Ndk_beginTransactions(3))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case9:事务超时结束后,再调用主动结束应该返回NDK_ERR_POSNDK_TRANS_NOEXIST
	sleep(4);
	if((ret = Ndk_endTransactions())!=NDK_ERR_POSNDK_TRANS_NOEXIST)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case10:调用获取状态函数应该返回成功
	if((ret = Ndk_getStatus())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

