/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi 模块
* file name		: wifi6.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_NetAddRouterTable在使用socket之前调用该函数,选择数据通过WiFi还是GPRS进行收发。
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
#define TESTAPI	"NDK_NetAddRouterTable"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 				: void wifi6(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			   zhengry      20160321    created 		 
*****************************************************************/
void wifi6(void)
{
	/*private & local definition*/
	int ret=0;
	
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	//case1:参数异常测试
	/*	低端代码实现上第2个IP参数没有实际使用,故不测试其非法情况 20170303 chenjs解释
	if((ret = NDK_NetAddRouterTable(COMM_TYPE_WIFI, NULL) ) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_NetAddRouterTable(COMM_TYPE_WIFI, "0.0.0.1") ) != NDK_ERR_NET_INVALIDIPSTR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}*/
	if((ret = NDK_NetAddRouterTable(COMM_TYPE_ETH, "0.0.0.0") ) != NDK_ERR_NET_UNKNOWN_COMMTYPE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_NetAddRouterTable(COMM_TYPE_BTH, "0.0.0.0") ) != NDK_ERR_NET_UNKNOWN_COMMTYPE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if 0
	//case2:正常测试,该用例移到conf.c中
	if((ret = NDK_NetAddRouterTable(COMM_TYPE_WIFI, "0.0.0.0") ) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	send_result("%s测试通过", TESTAPI);
	return;
}
