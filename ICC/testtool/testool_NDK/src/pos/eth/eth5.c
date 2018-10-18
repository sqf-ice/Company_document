/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth5.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_NetDHCP接口功能
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
#define 	TESTAPI		"NDK_NetDHCP"
#define		MAXDHCPTIME		12		//DHCP时间上限

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
void eth5(void)
{
	//以下为局部变量
	int ret = -1;
	char getIPaddr[16] = {0}, getMask[16] = {0}, getGateway[16] = {0};
	//char oldgetIPaddr[16] = {0}, oldgetMask[16] = {0}, oldgetGateway[16] = {0}, oldDns[16] = {0};
	float dhcptime = 0.0, dhcptime1 = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif

	if(auto_flag==1)
	{
		send_result("%s不支持自动测试,请重新手动测试", TESTAPI);
		return;
	}
	
	//以下为函数体
	cls_show_msg("测试%s,请将POS连接到一台支持DHCP的路由器上,任意键开始测试", TESTAPI);//屏幕提示当前所在的测试用例

	if(cls_show_msg("本用例要求开机后直接测试,如果不是开机后运行本用例请ESC键退出")==ESC)
	{
		send_result("line %d:非开机第一次运行,用户退出", __LINE__);
		return;
	}
	//测试前置，获取原始网络地址信息
	//NDK_EthGetNetAddr(oldgetIPaddr, oldgetMask, oldgetGateway, oldDns);

	//将网络地址信息设置成NULL
	if((ret=NDK_EthSetAddress("1.0.0.1", "255.0.0.0", "1.0.0.254", NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:正常进行DHCP功能验证,可以用于获取第一次开机DHCP时间
	lib_StartStopwatch();
	if((ret=NDK_NetDHCP()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//测试动态获取功能的执行不应大于规定值。(性能值规定)
	if((dhcptime1=lib_ReadStopwatch())>MAXDHCPTIME)
	{
		send_result("line %d:%s测试失败(DHCP时间:%f)", __LINE__, TESTAPI, dhcptime1);
		GOTOERR;
	}
	//验证动态分配后的IP应该和静态配置的不一致
	if ((ret=NDK_EthGetNetAddr(getIPaddr, getMask, getGateway, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(!strcmp(getIPaddr,"1.0.0.1") || !strcmp(getMask,"255.0.0.0") || !strcmp(getGateway,"1.0.0.254"))//linwl20130606修改DHCP函数功能有效性认证方式
	//if (strlen(getIPaddr) == 0 || strlen(getMask) == 0 || strlen(getGateway) == 0)
	{
		//send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, strlen(getIPaddr), strlen(getMask), strlen(getGateway));
		send_result("line %d:%s测试失败(%s,%s,%s)", __LINE__, TESTAPI, getIPaddr, getMask, getGateway);
		GOTOERR;
	}
	
#if !K21_ENABLE  //低端无状态栏就不进行网络图标的判断 20180316
	//case4:NDK_NetDHCP函数调用成功后且插网线情况下,状态栏图标显示:已连接网线图标 20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示已连接网线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示已连接网线", __LINE__);
		GOTOERR;
	}
#endif	

	//case1:当拔掉或不插网线时，进行DHCP使能，耗时应在规定时间以内，同时，不应获取到相关网络参数；
	cls_show_msg("请拔出网线,按任意键继续测试");
	lib_StartStopwatch();
#if K21_ENABLE  //低端返回未插线 20180328
	if((ret=NDK_NetDHCP()) != NDK_ERR_MODEM_NOLINE)
#else
	if((ret=NDK_NetDHCP()) != NDK_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((dhcptime=lib_ReadStopwatch())>MAXDHCPTIME)
	{
		send_result("line %d:%s测试失败(DHCP时间:%f)", __LINE__, TESTAPI, dhcptime);
		GOTOERR;
	}
	memset(getIPaddr, 0, sizeof(getIPaddr));
	memset(getMask, 0, sizeof(getMask));
	memset(getGateway, 0, sizeof(getGateway));
	if((ret=NDK_EthGetNetAddr(getIPaddr, getMask, getGateway, NULL)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if !K21_ENABLE  //低端无状态栏就不进行网络图标的判断 20180316
	//case5:NDK_NetDHCP成功过,未插网线情况下,状态栏图标显示:未连接网线图标 20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示未连接网线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示未连接网线", __LINE__);
		GOTOERR;
	}
#endif	

	//case2:在条件1之后，重插上网线，再次进行DHCP使能，耗时应在规定时间以内，同时，应能获取到相关网络参数；
	//将网络地址信息设置成NULL
	if((ret=NDK_EthSetAddress("1.0.0.1", "255.0.0.0", "1.0.0.254", NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请插上网线,按任意键继续测试");
	lib_StartStopwatch();
	if((ret=NDK_NetDHCP()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//测试动态获取功能的执行不应大于规定时间。(性能值规定)
	if((dhcptime=lib_ReadStopwatch())>MAXDHCPTIME)
	{
		send_result("line %d:%s测试失败(DHCP时间:%f)", __LINE__, TESTAPI, dhcptime);
		GOTOERR;
	}
	
	//验证动态分配后的IP应该和静态配置的不一致
	memset(getIPaddr, 0, sizeof(getIPaddr));
	memset(getMask, 0, sizeof(getMask));
	memset(getGateway, 0, sizeof(getGateway));
	if ((ret=NDK_EthGetNetAddr(getIPaddr, getMask, getGateway, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if(!strcmp(getIPaddr,"1.0.0.1") || !strcmp(getMask,"255.0.0.0") || !strcmp(getGateway,"1.0.0.254"))//linwl20130606修改DHCP函数功能有效性认证方式
	//if (strlen(getIPaddr) == 0 || strlen(getMask) == 0 || strlen(getGateway) == 0)
	{
		//send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, strlen(getIPaddr), strlen(getMask), strlen(getGateway));
		send_result("line %d:%s测试失败(%s,%s,%s)", __LINE__, TESTAPI, getIPaddr, getMask, getGateway);
		GOTOERR;
	}
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1]))!=NDK_OK)
	{                                                                                            
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
#if K21_ENABLE
	memset(str,0,sizeof(str));
	ftos(dhcptime1,str);
	send_result("%s测试通过(DHCP时间:%s)", TESTAPI, str); 
#else	
	send_result("%s测试通过(DHCP时间:%f)", TESTAPI, dhcptime1); 
#endif
ERR:
	NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1]);
	return;
}

