/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth7.c
* Author 			: chensj
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_NetAddRouterTable接口功能
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
#define 	TESTAPI		"NDK_NetAddRouterTable"

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
*			  		chensj		   20131127	 		created
*****************************************************************/
void eth7(void)
{
	//以下为局部变量
	char  szDestIP[32]={"192.169.4.199"}; 
	int nRet = 0;

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体,该函数的正常功能在systest45中验证，该案例只做异常测试
	//case1:异常参数
	if((nRet = NDK_NetAddRouterTable(-1,szDestIP))!=NDK_ERR_NET_UNKNOWN_COMMTYPE/*NDK_ERR_PARA*/)//20140102 天助解释开发修改了底层返回值
	{
		send_result("line %d:%s测试失败(%d)",__LINE__ , TESTAPI, nRet);
		RETURN;
	}

	//case2: 异常参数
	if((nRet = NDK_NetAddRouterTable(COMM_TYPE_UNKNOW+1,szDestIP))!=NDK_ERR_NET_UNKNOWN_COMMTYPE/*NDK_ERR_PARA*/)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__ , TESTAPI, nRet);
		RETURN;
	}

	//case3: 未知类型
	if((nRet = NDK_NetAddRouterTable(COMM_TYPE_UNKNOW,szDestIP))!=NDK_ERR_NET_UNKNOWN_COMMTYPE)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__ , TESTAPI, nRet);
		RETURN;
	}

#if !K21_ENABLE   //低端ME50C 支持传NULL 20180328  第二个参数无效 20180428 modify
	//case4: 异常参数
	if((nRet = NDK_NetAddRouterTable(COMM_TYPE_ETH,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__ , TESTAPI, nRet);
		RETURN;
	}

	//case5:无效IP
	if((nRet = NDK_NetAddRouterTable(COMM_TYPE_ETH,"192.168.4"))!=NDK_ERR_NET_INVALIDIPSTR)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__ , TESTAPI, nRet);
		RETURN;
	}
#endif
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}
