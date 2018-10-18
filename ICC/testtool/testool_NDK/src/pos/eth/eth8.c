/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: eth
* file name	: eth8.c
* Author 		: huangjianb
* version		: 
* DATE			: 20140616
* directory 		: 
* description		: 测试NDK_EthDisConnect函数关闭以太网功能
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
#define 	TESTAPI		"NDK_EthDisConnect"

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
* history 		 		: author				date				 remarks
*									:	huangjianb    20140617     created
*****************************************************************/
void eth8(void)
{
	int ret=0;
	char szIPaddr[16] = {0}, szMaskaddr[16] = {0}, szGateway[16] = {0}, szDNS[16*3+2] = {0};
	
	//case1:在静态方式下打开以太网成功后，可以获取到以太网信息，
	//调用NDK_EthDisConnect应该能够正常关闭，且关闭后无法获取IP地址。
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_EthDisConnect()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(auto_flag==0||auto_flag==2)
	{
		//case4:以太网关闭后,IP地址未设置,此时状态栏图标应该显示为:IP未设置图标 20140623  jiangym
		if(cls_show_msg("状态栏图标是否显示IP未设置图标,是[Enter]继续,否[其他]失败")!=ENTER)
		{
			send_result("line %d:状态栏图标应该显示IP未设置图标", __LINE__);
			GOTOERR;
		}
	}
	else
		send_result("line %d:%s状态栏IP未设置图标需另外手动测试", __LINE__, TESTAPI);
	
	
	//case 2:在动态方式下打开以太网成功后，可以获取到以太网信息，
	//调用NDK_EthDisConnect应该能够正常关闭，且关闭后无法获取IP地址。
	cls_show_msg1(30, "请将POS连接到一台支持DHCP的路由器上,任意键开始测试");
	if((ret=NDK_NetDHCP()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_EthDisConnect()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	};
	if((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:在切换回静态方式打开以太网成功后，可以获取到以太网信息，
	//调用NDK_EthDisConnect应该能够正常关闭，且关闭后无法获取IP地址。
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_EthDisConnect()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	};
	if ((ret=NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1]);
	return;
}
