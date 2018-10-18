/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WLAN模块
* file name		: wlan10.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_NetDnsResolv接口功能
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
#define 	TESTAPI		"NDK_NetDnsResolv"

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
*									huangjianb   20140617     modify
*****************************************************************/
void wlan10(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1;
	char DnsName[16] = "www.baidu.com", DnsName126[16] = "www.126.com";
	char DnsIP[16] = {0}, DnsIP126[16] = {0};

	//以下为函数体
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//测试前置,设置以太网地址，加载以太网模块
	//if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, DNS2)) != NDK_OK)
	//{
	//	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
	//	RETURN;
	//}
	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	//case1:传入非法参数,不存在域名
	if(((ret=NDK_NetDnsResolv(COMM_TYPE_WIFI,NULL, DnsName)) != NDK_ERR_PARA)
		||((ret1=NDK_NetDnsResolv(COMM_TYPE_WIFI,DnsIP, NULL)) != NDK_ERR_PARA)
		||((ret2=NDK_NetDnsResolv(COMM_TYPE_UNKNOW,DnsIP, DnsName)) != NDK_ERR_NET_UNKNOWN_COMMTYPE)){
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1,ret2);
		GOTOERR;
	}
	
	//case2:正常使用测试
	memset(DnsIP, 0, sizeof(DnsIP));
	if ((ret=NDK_NetDnsResolv(COMM_TYPE_WIFI,DnsIP, DnsName)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:新增测试解析www.126.com 20161019
	memset(DnsIP126, 0, sizeof(DnsIP126));
	if ((ret=NDK_NetDnsResolv(COMM_TYPE_WIFI,DnsIP126, DnsName126)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过!\n百度域名:%s IP:%s\n126域名:%s IP:%s\n", TESTAPI, DnsName, DnsIP, DnsName126, DnsIP126);
ERR:
	NetDown();
	return;
}

