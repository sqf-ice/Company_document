/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth3.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_GetDnsIp接口功能
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
#define 	TESTAPI		"NDK_GetDnsIp"

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
*			  						chensl		   20130105	 		created
*****************************************************************/
void eth3(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1;
	char DnsName[16] = "www.baidu.com";
	char DnsIP[16] = {0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//测试前置,设置以太网地址，加载以太网模块
	//if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, DNS2)) != NDK_OK)
	//{
	//	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
	//	return;
	//}
	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	//case1:传入非法参数,不存在域名
	if(((ret=NDK_GetDnsIp(NULL, DnsName)) != NDK_ERR_PARA)
		||((ret1=NDK_GetDnsIp(DnsIP, NULL)) != NDK_ERR_PARA)){
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	
	//case2:正常使用测试
	memset(DnsIP, 0, sizeof(DnsIP));
	if ((ret=NDK_GetDnsIp(DnsIP, DnsName)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过!域名:%s IP:%s", TESTAPI, DnsName, DnsIP);
ERR:
	NetDown();
	return;
}

