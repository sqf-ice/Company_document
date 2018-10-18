/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan4.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiConnect,NDK_WiFiIsConnected接口功能
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
#define TESTAPI	"NDK_WiFiConnect,NDK_WiFiIsConnected"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan4(void)
* functional description 	: 获取连接状态信息 ,判断是否已连上AP  
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/
void wlan4(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	char key[20]={0};
	ST_WIFI_PARAM param;
    EM_WPA_CONSTATE state;
	time_t diff = 0, oldtime = 0;
	char szIp[32]={0};
	EM_WIFI_NET_SEC sec;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];
	
	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

    param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];
	
	//case1:测试WIFI设备未初始化就调用NDK_WiFiConnect函数应该返回失败
	if ((ret=NDK_WiFiConnect("XXX", &param)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试前置:WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}

	//扫描AP,连续扫描3轮
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg1(3, "共扫描到%d个AP,line:%d,AP[%d]=%s", numList,__LINE__, i, ESSIDlist1[i]);
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

	//case2:异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiConnect("XXX", NULL)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiConnect(NULL, &param)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(1,"子用例case2测试通过");

#if 1
	//case3:连接WIFI之前调用NDK_WiFiIsConnected函数应该返回失败
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif//该用例底层返回NDK_ERR_WIFI_DEVICE_UNAVAILABLE 设备无效不合适，底层修改

	//case4:正常测试:
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:测试连接后的状态变化
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
	
	//case8:增加用例验证WIFI模块DHCP功能能够正常获取到地址,获取的地址应该跟静态设置的不一致
	if(netsetting.DHCPenable==ENABLE)
	{
		memset(szIp, 0, sizeof(szIp));
		//if((ret = NDK_EthGetNetAddr(szIp,NULL,NULL,NULL)) != NDK_OK)
		if((ret=NDK_NetGetAddr(COMM_TYPE_WIFI,szIp,NULL,NULL,NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		/*
		if(!strcmp(szIp,netsetting.LocalIP.Str))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}*/
		//20130813 linwl修改判断方式为人工判断，因为在入口处连接性判断时已经修改netsetting.LocalIP.Str为动态分派到的IP值所以上面的比较方法无效
		if(cls_show_msg("WIFI动态获取到的IP地址为:%s,确认键通过,其他键失败", szIp)!=ENTER)
		{
			send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szIp);
			GOTOERR;
		}
	}
	
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		 send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    	GOTOERR;
	}
	cls_show_msg1(3,"子用例case3、case4测试通过");
	
	//WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	//case7:测试可以连接隐藏SSID的路由器
	cls_show_msg("请将测试用路由器设置成隐藏SSID,后按任意键继续测试");
	//扫描AP,连续扫描3轮
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg("共扫描到%d个AP,line:%d,AP[%d]=%s", numList,__LINE__, i, ESSIDlist1[i]);
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
	//验证应该扫描不到被隐藏的AP
	for(i=0; i<numList; i++)
	{
		if(!strcmp(ESSIDlist1[i],WlanEssid))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	//验证能够正常连接
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

	//case8:连接过一次之后再扫描应该仍然扫描不到
	//扫描AP,连续扫描3轮
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg("共扫描到%d个AP,line:%d,AP[%d]=%s", numList,__LINE__, i, ESSIDlist1[i]);
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
	//验证应该扫描不到被隐藏的AP
	for(i=0; i<numList; i++)
	{
		if(!strcmp(ESSIDlist1[i],WlanEssid))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	
	//挂断后执行后续用例
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		 send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    	GOTOERR;
	}
	cls_show_msg("case7测试通过,请将测试用路由器设置成广播方式(不隐藏)");

	//WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}

	//case6:连接参数错误的情况测试
	//case6.1:传入加密模式不在合理范围内
	param.emSecMode = WIFI_NET_SEC_WPA2+1;
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//直接返回参数错误，延时5s即可
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(3,"子用例case6.1测试通过");

	//case6.2:传入加密模式和路由器加密模式不一致
	//扫描AP,连续扫描3轮
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg("共扫描到%d个AP,line:%d,AP[%d]=%s", numList,__LINE__, i, ESSIDlist1[i]);
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
	NDK_WiFiGetSec(WlanEssid, &sec);//增加获取路由器加密方式
	if(sec == WIFI_NET_SEC_WEP_OPEN||sec == WIFI_NET_SEC_WEP_SHARED)//如果路由器加密模式是WEP 则测试用WPA 20140221 linwl
		param.emSecMode = WIFI_NET_SEC_WPA2;
	else//路由器加密模式使用NONE和WPA情况下测试用 WEP模式
		param.emSecMode = WIFI_NET_SEC_WEP_OPEN;
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)//非阻塞函数，参数合法情况下直接返回成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(50);//动态连接时连不上,延时50s确保连接函数执行完毕
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/)//但实际连接结果应该是连接不成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(3,"子用例case6.2测试通过");
	param.emSecMode=linksetting.WLANEncMode;

	//case6.3:传入密码不正确
	memset(key, 0x00, sizeof(key));
	strcpy(key, "ERRpwd1234");
	param.pszKey = key;
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)//非阻塞函数，参数合法情况下直接返回成功
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(param.emKeyType!=WIFI_KEY_TYPE_NOKEY)//如果路由器设置有密码，则实际连接应该失败 20140221 linwl
	{
		sleep(50);//动态连接时连不上,延时50s确保连接函数执行完毕
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/)//但实际连接结果应该是连接不成功
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(3,"子用例case6.3测试通过");
	}
	else//如果路由器无密码，则实际连接应成功
	{
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
		//WIFI设备初始化
		if ((ret=NDK_WiFiInit()) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			return;
		}
	}

	//case6.4:传入密码为NULL,路由器无密码时应该成功，路由器有密码时应该失败 20140221 linwl
	param.pszKey = NULL;
	if(param.emKeyType!=WIFI_KEY_TYPE_NOKEY)//如果路由器设置有密码，则实际连接应该失败
	{
		if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		sleep(10);//密码为NULL,直接返回参数错误
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
	{
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
		//WIFI设备初始化
		if ((ret=NDK_WiFiInit()) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			return;
		}
	}
	cls_show_msg1(3,"子用例case6.4测试通过");
	param.pszKey=linksetting.WLANPwd;
	
	//现在开发对IP ，网关，掩码，DNS在为NULL的情况下都未做参数判断（不管是静态还是动态方式），
	//都交由底层连接后返回（动态链接的上，静态链接不上）
	//case6.5:IP地址为NULL//这里动态方式IP会返回-3006 跟网关等为NULL返回OK 有差异
    if(param.ucIfDHCP)//对于动态分配
    {
    	param.psEthIp=NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
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
    	cls_show_msg1(3,"子用例case6.5测试通过");
    	param.psEthIp=netsetting.LocalIP.Str;
    	
    	//case6.6:网关为空
    	param.psEthGateway=NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
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
    	cls_show_msg1(1,"子用例case6.6测试通过");
    	param.psEthGateway=netsetting.LocalGateway.Str;
    	
    	//case6.7:子网掩码为空
    	param.psEthNetmask =NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
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
    	cls_show_msg1(1,"子用例case6.7测试通过");
    	param.psEthNetmask=netsetting.LocalMask.Str;
    }
    else//对于静态分配
    {
		param.psEthIp= NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	cls_show_msg1(3,"子用例case6.5测试通过");
    	param.psEthIp=netsetting.LocalIP.Str;
    	
    	//case6.6:网关为空
    	param.psEthGateway= NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	cls_show_msg1(1,"子用例case6.6测试通过");
    	param.psEthGateway=  netsetting.LocalGateway.Str;
    	
    	//case6.7:子网掩码为空
    	param.psEthNetmask = NULL;
    	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	sleep(10);
    	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
    	{
    		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
    		GOTOERR;
    	}
    	cls_show_msg1(1,"子用例case6.7测试通过");
    	param.psEthNetmask=  netsetting.LocalMask.Str;

		//case6.10:DNS服务器为NULL情况测试
		param.psEthDnsPrimary=NULL;
		if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		sleep(10);
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(3,"子用例6.10测试通过");
		
		param.psEthDnsPrimary= netsetting.DNSHost[0];
		param.psEthDnsSecondary = NULL;
		if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_ERR_WIFI_INVDATA)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		sleep(10);
		if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED/*NDK_ERR_WIFI_DEVICE_UNAVAILABLE*/) 
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(3,"子用例6.11测试通过");
		
		param.psEthDnsPrimary = netsetting.DNSHost[0];
		param.psEthDnsSecondary = netsetting.DNSHost[1];
}


	//case6.8:密码类型不对，正确的密码类型是WIFI_KEY_TYPE_ASCII
    /*最终传入的密码一定会是ASCII码,也没有必要进行对十六进制到ASCII的转换
	param.emKeyType = WIFI_KEY_TYPE_HEX;
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(30);
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) 
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(1,"子用例case6.8测试通过");
    */

        
	//case6.9:密码类型不对，正确的密码类型是WIFI_KEY_TYPE_ASCII
	/*这里密码类型为NOKEY，就是之前的szKey为NULL的情况
	param.emKeyType = WIFI_KEY_TYPE_NOKEY;
	if ((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(30);
	if ((ret=NDK_WiFiIsConnected()) != NDK_ERR_WIFI_NON_CONNECTED) //此处有问题（可以连接上），密码类似实际为ASCII
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(1,"子用例case6.9测试通过");
	param.emKeyType = WIFI_KEY_TYPE_ASCII;
	*/
	
	sleep(10);
	//case7:异常测试后正常测试看看是否正常
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

	//case8:应允许所设置的本地IP所在子网和所设置的网关不在同一子网的情况,连接应该成功 20160608
	//WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		return;
	}
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = "40.40.40.103";
	param.psEthGateway = "172.16.60.1";
	param.psEthNetmask =  "255.255.255.0";
	param.ucIfDHCP = 0;
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
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		show_stopwatch(1, diff);
		//send_result("status=%d", state);
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



