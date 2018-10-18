/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: wlan通讯模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20171130
* directory 			: 
* description			: 测试wlan 通讯模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#define APNUM1 64
/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"wlan模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	   WiFiGetNetList,
	   WiFiInit,
	   WiFiSignalCover,
	   WiFiGetSec,
	   WiFiConnect,
	   WiFiIsConnected,
	   WiFiConnectState,
	   WiFiGetNetInfo,
	   WiFiGetMac,
	   WiFiShutdown,
	   WiFiDisconnect,
	   MaxNum,
}EM_WLAN;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: wlan 通讯模块随机组合函数
* functional description 	: 测试wlan 通讯模块随机组合情况
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 			: author		date			remarks
*							liny				20171207		created
*****************************************************************/
static int wlan_randtest(int funnum)
{ 
	int i=0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];
	char szMac[64]={0};
	int numList=0,signal = 0;
	EM_WIFI_NET_SEC sec;
	ST_WIFI_PARAM param;
	EM_WPA_CONSTATE state;
	ST_WIFI_APINFO ESSIDlist2[64];
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];
	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];

    	switch(funnum)
	{	
	   case WiFiGetNetList:  
			NDK_WiFiGetNetList(ESSIDlist, &numList);
			break;
		case WiFiInit:
			NDK_WiFiInit();
			break;
		case WiFiSignalCover:
			NDK_WiFiSignalCover(WlanEssid,&signal);
			break;
		case WiFiGetSec:
			NDK_WiFiGetSec(WlanEssid,&sec);
			break;
		case WiFiConnect:
			NDK_WiFiConnect(WlanEssid, &param);
			break;
		case WiFiIsConnected:
			NDK_WiFiIsConnected();
			break;
		case WiFiConnectState:
			NDK_WiFiConnectState(&state);
			break;
		case WiFiGetNetInfo:
			NDK_WiFiGetNetInfo(ESSIDlist2,APNUM1, &numList);
			break;
		case WiFiGetMac:
			NDK_WiFiGetMac(szMac);
			break;
		case WiFiShutdown:
			NDK_WiFiShutdown();
			break;
	    case WiFiDisconnect:
			NDK_WiFiDisconnect();
			break;
    }
    return SUCC;
}

void wlan200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定	
	char *wlanstr[] = {"NDK_WiFiGetNetList","NDK_WiFiInit","NDK_WiFiSignalCover","NDK_WiFiGetSec","NDK_WiFiConnect","NDK_WiFiIsConnected","NDK_WiFiConnectState","NDK_WiFiGetNetInfo","NDK_WiFiGetMac",
		                    "NDK_WiFiShutdown","NDK_WiFiDisconnect"};


	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;

		//产生随机数
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
		}
		cls_printf("第%d次随机顺序:",bak);
		for(i=0;i<len;i++)
		{
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
			
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = wlan_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",wlanstr[num[j]]);
				goto ERR;
			}
		}	
		
	}
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WiFiShutdown();
	return;
}


