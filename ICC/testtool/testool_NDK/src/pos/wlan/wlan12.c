/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan12.c
* Author 		: linwl
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiDisconnect接口功能
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
#define TESTAPI	"NDK_WiFiDisconnect"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan8(void)
* functional description 	: 获取Mac 地址
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					linwl   20151113	created
*****************************************************************/
void wlan12(void)
{
	/*private & local definition*/
	int nRet=0;
	time_t diff = 0, oldtime = 0;
	ST_WIFI_PARAM param;
	EM_WPA_CONSTATE state;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例	

	/*测试前置*/
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];

	//case1:初始化之后,未连接情况下调用断开连接不应该出现异常
	if((nRet = NDK_WiFiDisconnect()) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		GOTOERR;
	}
	
	if((nRet = NDK_WiFiInit()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		GOTOERR;
	}
	
	//case2:初始化之后,未连接情况下调用断开连接不应该出现异常
	if((nRet = NDK_WiFiDisconnect()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		GOTOERR;
	}

	//case3:连接后,调用断开连接函数,应该能够断开成功
	if ((nRet=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((nRet=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
			goto ERR;
		}
		show_stopwatch(1, diff);
		
		if (state == WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
	}
	if ((nRet=NDK_WiFiIsConnected()) != NDK_OK)
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
	    GOTOERR;
	}
	if(auto_flag==0||auto_flag==2)
	{
		if(cls_show_msg("状态栏是否显示WIFI已连接图标,[是]:确认,[否]:其它")!=ENTER)
		{	
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			GOTOERR;
		}
	}
	if((nRet = NDK_WiFiDisconnect()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	if ((nRet=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)//这里要判断挂断是否需要时间
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
	    GOTOERR;
	}
	if(auto_flag==0||auto_flag==2)
	{
		if(cls_show_msg("状态栏是否不显示WIFI图标,[是]:确认,[否]:其它")!=ENTER)
		{	
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			GOTOERR;
		}
	}
	
	//case4:断开连接后,调用连接函数,应该能够重新连接成功
	if ((nRet=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((nRet=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
			goto ERR;
		}
		show_stopwatch(1, diff);
		
		if (state == WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
	}
	if ((nRet=NDK_WiFiIsConnected()) != NDK_OK)
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
	    GOTOERR;
	}
	
	//case5:不调用断开连接函数,直接挂断芯片后,重新初始化连接操作应该成功
	if((nRet = NDK_WiFiShutdown()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	if((nRet = NDK_WiFiInit()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		GOTOERR;
	}
	if ((nRet=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((nRet=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
			goto ERR;
		}
		show_stopwatch(1, diff);
		
		if (state == WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
	}
	if ((nRet=NDK_WiFiIsConnected()) != NDK_OK)
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, nRet);
	    GOTOERR;
	}
	if((nRet = NDK_WiFiDisconnect()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	if((nRet = NDK_WiFiShutdown()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_WiFiShutdown();
	return;
}
