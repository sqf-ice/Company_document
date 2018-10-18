/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan13.c
* Author 		: 
* version		: 
* DATE			: 
* directory 		: 
* description		: 测试NDK_WiFiConnect_8021x能连接到指定的802.1x认证模式的AP功能
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
#define TESTAPI	"NDK_WiFiConnect_8021x"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan13(void)
* functional description 	        : 连接到指定的802.1x认证模式的AP  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20161202   created		
*****************************************************************/
void wlan13(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	char key[20]={0};
	ST_WIFI_8021x_PARAM param;
	EM_WPA_CONSTATE state;
	time_t diff = 0, oldtime = 0;
	ST_WIFI_APINFO ESSIDlist[64];	

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	//如果入口处配置不是802.1x则提示重新配置
	if(linksetting.WLANEncMode!=WIFI_NET_SEC_WPA_EAP)
	{
		send_result("%s测试需配置WIFI加密模式为802.1x模式后快选本用例进行测试", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//将入口处配置的全局变量传递给函数参数
	param.comm_param.pszKey = linksetting.WLANPwd;
	param.comm_param.emKeyType = linksetting.WLANKeyType;
	param.comm_param.emSecMode = linksetting.WLANEncMode;
	param.comm_param.psEthIp = netsetting.LocalIP.Str;
	param.comm_param.psEthGateway = netsetting.LocalGateway.Str;
	param.comm_param.psEthNetmask =  netsetting.LocalMask.Str;
	param.comm_param.ucIfDHCP = netsetting.DHCPenable;
	param.comm_param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.comm_param.psEthDnsSecondary = netsetting.DNSHost[1];
	param.emEap = linksetting.WLAN8021XEapMode;
	param.pszIdentity = linksetting.WLAN8021XIdentity;
	param.pszca_cert = NULL;
	
	//case1:测试WIFI设备未初始化就调用NDK_WiFiConnect_8021x函数应该返回失败
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
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

	//case2:应该能够获取到所有的AP信息，并在屏幕上显示，其中应包含由待测试的WPA/WPA2模式的AP信息:包括ssid,安全模式,信号强度等
	memset(ESSIDlist,0,sizeof(ESSIDlist));
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist,64, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("Line=%d\nAP[%d]->Essid:%s\nsKeyModeStr:%s\nnFrequency:%d\nsSignal:%s", __LINE__,i, ESSIDlist[i].sEssid,ESSIDlist[i].sKeyModeStr,ESSIDlist[i].nFrequency,ESSIDlist[i].sSignal);
				break;
			}
		}
		sleep(1);
	}
	if (j == 3)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case3:异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, NULL)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiConnect_8021x(NULL, &param)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(1,"子用例case3测试通过");

	//case4:正常测试:预期应该能够连接成功
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
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
	cls_show_msg1(1,"子用例case4测试通过");

	//case5:断开连接 应该能够成功且获取状态是未连接状态
	if((ret = NDK_WiFiDisconnect()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    GOTOERR;
	}
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		 send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    	GOTOERR;
	}
	cls_show_msg1(1,"子用例case5测试通过");
	
	//case6:测试可以连接隐藏SSID的路由器
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	cls_show_msg("请将测试用路由器设置成隐藏SSID,后按任意键继续测试");
	memset(ESSIDlist,0,sizeof(ESSIDlist));
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist,64, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("Line=%d\nAP[%d]->Essid:%s\nsKeyModeStr:%s\nnFrequency:%d\nsSignal:%s", __LINE__,i, ESSIDlist[i].sEssid,ESSIDlist[i].sKeyModeStr,ESSIDlist[i].nFrequency,ESSIDlist[i].sSignal);
				break;
			}
		}
		sleep(1);
	}
	if (j == 3)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	/*//验证应该扫描不到隐藏的SSID，该用例由于之前已经链接过，所以仍然能够被扫描到，一直一来都是这个现象所以不测试；
	for(i=0; i<numList; i++)
	{
		if(!strcmp(ESSIDlist[i],WlanEssid))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}*/
	//验证能够正常连接
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		show_stopwatch(1, diff);
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
	//挂断后执行后续用例
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		 send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    	GOTOERR;
	}
	cls_show_msg("case6测试通过,请将测试用路由器设置成广播方式(不隐藏)");

	//case7:连接参数错误的情况测试
	//case7.1:传入加密模式不在范围内
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	param.comm_param.emSecMode = WIFI_NET_SEC_WPA_EAP+1;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//直接返回参数错误，延时5s即可
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.comm_param.emSecMode = linksetting.WLANEncMode;
	
	//case7.2:传入密码为NULL时应该失败 
	param.comm_param.pszKey = NULL;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//密码为NULL,直接返回参数错误
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.comm_param.pszKey=linksetting.WLANPwd;

	//case7.3:传入的认证用户名为空返回失败
	param.pszIdentity = NULL;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.pszIdentity = linksetting.WLAN8021XIdentity;

	//case7.4:传入的802.1x认证方式不在范围内返回失败
	param.emEap = EAP_MODE_TTLS+1;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.emEap = linksetting.WLAN8021XEapMode;
	
	//case7.5:传入的802.1x认证方式与路由器设置的不正确时应该连接不上
	if(param.emEap == EAP_MODE_TTLS)//EAP_MODE_PEAP
		param.emEap = EAP_MODE_PEAP;
	else if(param.emEap == EAP_MODE_PEAP)
		param.emEap = EAP_MODE_MD5;
	else if(param.emEap == EAP_MODE_MD5)
		param.emEap = EAP_MODE_TTLS;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)//非阻塞函数，参数合法情况下直接返回成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(50);//动态连接时连不上,延时50s确保连接函数执行完毕
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)//但实际连接结果应该是连接不成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.emEap = linksetting.WLAN8021XEapMode;
	cls_show_msg1(1,"子用例case7.5测试通过");

	//case7.6:传入的认证用户名不正确应该连接不上
	strcpy(param.pszIdentity, "ERRidentity");
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)//非阻塞函数，参数合法情况下直接返回成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(50);//动态连接时连不上,延时50s确保连接函数执行完毕
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)//但实际连接结果应该是连接不成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.pszIdentity = linksetting.WLAN8021XIdentity;
	cls_show_msg1(1,"子用例case7.6测试通过");
	
	//case7.7:传入密码不正确应该连接不上
	memset(key, 0x00, sizeof(key));
	strcpy(key, "ERRpwd1234");
	param.comm_param.pszKey = key;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)//非阻塞函数，参数合法情况下直接返回成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(50);//动态连接时连不上,延时50s确保连接函数执行完毕
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED)//但实际连接结果应该是连接不成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	param.comm_param.pszKey =  linksetting.WLANPwd;
	cls_show_msg1(1,"子用例case7.7测试通过");
	
	//现在开发对IP ，网关，掩码，DNS在为NULL的情况下都未做参数判断（不管是静态还是动态方式），
	//都交由底层连接后返回（动态链接的上，静态链接不上）
    if(param.comm_param.ucIfDHCP)//对于动态分配
    {
		//case6.8:IP地址为NULL
    	param.comm_param.psEthIp=NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(30);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_OK) //连接成功
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthIp=netsetting.LocalIP.Str;
		cls_show_msg1(1,"子用例case7.8测试通过");
    	
    	//case6.9:网关为NULL
    	param.comm_param.psEthGateway=NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(30);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_OK) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthGateway=netsetting.LocalGateway.Str;
		cls_show_msg1(1,"子用例case7.9测试通过");
    	
    	//case6.10:子网掩码为NULL
    	param.comm_param.psEthNetmask =NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(30);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_OK) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthNetmask=netsetting.LocalMask.Str;
		cls_show_msg1(1,"子用例case7.10测试通过");
    }
    else//对于静态分配
    {
		//case6.8:IP地址为NULL
		param.comm_param.psEthIp= NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthIp=netsetting.LocalIP.Str;
		cls_show_msg1(1,"子用例case7.8测试通过");
    	
    	//case6.9:网关为NULL
    	param.comm_param.psEthGateway= NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthGateway=  netsetting.LocalGateway.Str;
		cls_show_msg1(1,"子用例case7.9测试通过");
    	
    	//case6.10:子网掩码为NULL
    	param.comm_param.psEthNetmask = NULL;
    	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	param.comm_param.psEthNetmask=  netsetting.LocalMask.Str;
		cls_show_msg1(1,"子用例case7.10测试通过");

		//case6.11:DNS服务器为NULL
		param.comm_param.psEthDnsPrimary=NULL;
		if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		sleep(10);
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		param.comm_param.psEthDnsPrimary= netsetting.DNSHost[0];
		param.comm_param.psEthDnsSecondary = NULL;
		if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		sleep(10);
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		param.comm_param.psEthDnsPrimary = netsetting.DNSHost[0];
		param.comm_param.psEthDnsSecondary = netsetting.DNSHost[1];
		cls_show_msg1(1,"子用例case7.11测试通过");
}
	
	sleep(10);
	//case8:异常测试后正常测试看看是否正常
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		show_stopwatch(1, diff);
		
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
	//挂断后执行后续用例
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		 send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    	GOTOERR;
	}
	cls_show_msg1(1,"子用例case8测试通过");

 	//case8:应允许所设置的本地IP所在子网和所设置的网关不在同一子网的情况,连接应该成功 
 	//WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	param.comm_param.psEthIp = "40.40.40.103";
	param.comm_param.psEthGateway = "172.16.60.1";
	param.comm_param.psEthNetmask =  "255.255.255.0";
	param.comm_param.ucIfDHCP = 0;
	if ((ret=NDK_WiFiConnect_8021x(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime = time(NULL);
	while(1)
	{
		if ((ret=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		show_stopwatch(1, diff);
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
	
	send_result("%s测试通过", TESTAPI);
ERR:
	//测试后置:WIFI设备挂断
	NDK_WiFiShutdown();
	return;
}

