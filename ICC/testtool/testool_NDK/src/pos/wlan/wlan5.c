/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan5.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiConnectState接口功能
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
#define TESTAPI	"NDK_WiFiConnectState"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan5(void)
* functional description 	: 获取连接状态信息 ,判断是否已连上AP  
* input parameter	 				:无
* output parameter	 				:无
* return value		 				:无
* history 		 					:  author		date		remarks
*			  	  	  			   chensj   201305013	created
*****************************************************************/
void wlan5(void)
{
	/*private & local definition*/
	int ret = 0;
	ST_WIFI_PARAM param;
	EM_WPA_CONSTATE state;
	time_t diff = 0, oldtime = 0;
	
	/*process body*/
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_WiFiConnectState函数应该返回失败
	if ((ret=NDK_WiFiConnectState(&state)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE) 
	{
    	send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试前置:WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	
	//case4:设备未初始化调用SDK_WiFiConnectState函数应该返回失败
	if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||state!=WIFI_WPA_CONSTATE_UNLINKED) 
	{
		send_result("line %d:%s测试失败ret=%d,state=%d", __LINE__, TESTAPI, ret, state);
		RETURN;
	}

	//case2:异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiConnectState(NULL)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//LINKING,        ///< Linking to WiFi
	//LINKED,         ///< Selected but still not authenticated nor connected
    //AUTHENTICATED,  ///< After linked, we are authenticated but we have no context
    //CONTEXT   

	//case3:正常测试构造各种可能的状态分别进行验证
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	oldtime = time(NULL);
	while(1) 
	{
    	if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT) 
		{
    		send_result("line %d:%s测试失败ret=%d,state=%d", __LINE__, TESTAPI, ret, state);
			goto ERR;
    	}
		show_stopwatch(1, diff);
		if(state==WIFI_WPA_CONSTATE_UNLINKED)
			cls_printf("is UNLINKED...");
		if(state==WIFI_WPA_CONSTATE_LINKING)
			cls_printf("is LINKING...");
		if(state==WIFI_WPA_CONSTATE_LINKED)
			cls_printf("is LINKED...");
		if(state==WIFI_WPA_CONSTATE_AUTHENTICATED)
			cls_printf("is AUTHENTICATED...");
		if (state == WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
    }
	
	if ((ret=NDK_WiFiIsConnected()) != NDK_OK) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret = NDK_WiFiDisconnect()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||state!=WIFI_WPA_CONSTATE_UNLINKED) 
	{
		send_result("line %d:%s测试失败ret=%d,state=%d", __LINE__, TESTAPI, ret, state);
		GOTOERR;
	}
    
	//测试后置:WIFI设备挂断
	send_result("%s测试通过", TESTAPI);
ERR:
	//测试后置:WIFI设备挂断
	NDK_WiFiShutdown();
	return;
}

