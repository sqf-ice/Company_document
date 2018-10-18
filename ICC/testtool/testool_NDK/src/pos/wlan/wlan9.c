/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan9.c
* Author 		: chensj
* version		: 
* DATE			: 20131127
* directory 		: 
* description		: 测试NDK_NetGetAddr接口功能
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
#define TESTAPI	"NDK_NetGetAddr"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan9(void)
* functional description 	: 获取Mac 地址
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  				chensj   20131127	created
*****************************************************************/
void wlan9(void)
{
	int ret = 0,ret1=0,i=0;
	char szIPaddr[16] = {0}, szMaskaddr[16] = {0}, szGateway[16] = {0}, szDns[16*3+2] = {0}, tempDns1[16*3+2]={0}, tempDns2[16*3+2]={0};
	ST_WIFI_PARAM param;
	time_t diff = 0, oldtime = 0;
	EM_WPA_CONSTATE state;

	/*process body*/
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例	
	
	//case1:未初始化就取地址应该失败
	if((ret=NDK_NetGetAddr(COMM_TYPE_WIFI, szIPaddr, szMaskaddr, szGateway, NULL)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	

	//测试前置:WIFI设备初始化
	if((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];	

	if((ret=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	oldtime = time(NULL);
	while(1)
	{
		if((ret=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
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

	if((ret=NDK_WiFiIsConnected()) != NDK_OK)
	{
	    send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
	    GOTOERR;
	}

	//异常参数测试
	if((ret=NDK_NetGetAddr(COMM_TYPE_WIFI, NULL, NULL, NULL, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//异常参数测试
	if(((ret=NDK_NetGetAddr(COMM_TYPE_ETH-1, szIPaddr, szMaskaddr, szGateway, NULL)) != NDK_ERR_PARA)
		||((ret1=NDK_NetGetAddr(COMM_TYPE_UNKNOW, szIPaddr, szMaskaddr, szGateway, NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}

	//case2:正常功能测试，看是否可以获取到IP、子网掩码、网关
	if((ret=NDK_NetGetAddr(COMM_TYPE_WIFI, szIPaddr, szMaskaddr, szGateway, szDns)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(param.ucIfDHCP)//动态地址
	{
		if(auto_flag==0)
		{
			cls_show_msg1(3,"获取wifi地址成功IP:%s",szIPaddr);
			cls_show_msg1(3,"获取wifi地址成功子网掩码:%s",szMaskaddr);
			cls_show_msg1(3,"获取wifi地址成功网关:%s",szGateway);
			cls_show_msg1(3,"获取wifi地址成功DNS:%s",szDns);
			if(cls_show_msg("获取的IP、子网掩码、网关、DNS是否合理?[ENTER]合理 [其他]不合理")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
		else
			send_result("获取wifi地址成功IP:%s 子网掩码:%s 网关:%s DNS:%s,请分析是否合理",szIPaddr,szMaskaddr,szGateway,szDns);
	}
	else//静态地址应该和连接参数一致
	{
		sprintf(tempDns1,"%s;%s",netsetting.DNSHost[0],netsetting.DNSHost[1]);
		sprintf(tempDns2,"%s;%s",netsetting.DNSHost[1],netsetting.DNSHost[0]);
		if(strcmp(netsetting.LocalIP.Str,szIPaddr)||strcmp(netsetting.LocalGateway.Str,szGateway)||strcmp(netsetting.LocalMask.Str,szMaskaddr)||((strcmp(tempDns1,szDns)!=0)&&(strcmp(tempDns2,szDns)!=0)))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}

	//测试后置:WIFI设备挂断
	if((ret = NDK_WiFiShutdown()) != NDK_OK)
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:挂断后,获取地址应该返回失败
	memset(szIPaddr,0,sizeof(szIPaddr));
	memset(szMaskaddr,0,sizeof(szMaskaddr));
	memset(szGateway,0,sizeof(szGateway));
	memset(szDns,0,sizeof(szDns));
	if((ret=NDK_NetGetAddr(COMM_TYPE_WIFI, szIPaddr, szMaskaddr, szGateway, szDns)) != NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=0;i<16;i++)
	{	
		if(szIPaddr[i]!='\0'||szMaskaddr[i]!='\0'||szGateway[i]!='\0'||szDns[i]!='\0')
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WiFiShutdown();
	return;
}

