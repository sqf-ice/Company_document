/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: eth 模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20180130
* directory 			: 
* description			: 随机组合调用eth 模块函数，是否会跑飞
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#define		TIMEOUT		3.0

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"ETH模块内随机组合"

/*------------global variables definition-----------------------*/
/*----------global variables declaration("extern")--------------*/
/*---------------structure definition---------------------------*/
//定义枚举
typedef enum {
    EthSetAddress,
    EthGetMacAddr,
    NetPing,
    NetDHCP,
    NetGetAddr,
    NetAddRouterTable,
    NetDnsResolv,
    EthGetNetAddr,
    EthDisConnect,
#if !defined ME50C
     GetDnsIp,
     NetSetDns,
#endif
    MaxNum
}EM_ETH;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*                                 linying            20180211           created
*****************************************************************/
//随机调用ETH模块函数
static int ETH_RandTest(int funnum)
{
    	//以下为局部变量
    	int ret=-1;
	char szIPaddr[16] = {0}, szMaskaddr[16] = {0}, szGateway[16] = {0}, szDNS[16*3+2] = {0};	
	char DnsName[16] = "www.baidu.com";
	char MacAddr[18] = {0};
	char DnsIP[16] = {0};
	EM_COMM_TYPE commtype = 0;
   	srand((unsigned)time(NULL));
	switch(funnum)
    	{
	    case EthSetAddress:
		    if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, DNS3)) != NDK_OK)
	        {
		       	 send_result("line %d:设置网络地址失败(%d)", __LINE__, ret);
		         return ret;
		     }
			break;
		case EthGetMacAddr:
			memset(MacAddr, 0, sizeof(MacAddr));
	        NDK_EthGetMacAddr(MacAddr);
			break;
		case NetPing:
			NDK_NetPing(netsetting.SvrIP.Str, TIMEOUT);
			break;
		case NetDHCP:
		    NDK_NetDHCP();
			break;
		 case NetGetAddr:
		 	NDK_NetGetAddr(COMM_TYPE_ETH, szIPaddr, szMaskaddr, szGateway, szDNS);
			break;
		 case NetAddRouterTable:
		 	NDK_NetAddRouterTable(COMM_TYPE_ETH,"192.168.1.1");
			break;
		 case NetDnsResolv:
		 	NDK_NetDnsResolv(COMM_TYPE_ETH,DnsIP, DnsName);
			break;
		 case EthGetNetAddr:
			memset(szDNS,0,sizeof(szDNS));
	        NDK_EthGetNetAddr(szIPaddr, szMaskaddr, szGateway, szDNS) ;
			break;
		case EthDisConnect:
		 	 if ((ret=NDK_EthDisConnect()) != NDK_OK)
		        {
			        send_result("line %d:以太网断开失败(%d)", __LINE__, ret);
			        return ret;
		        }
			break;	
#if !defined ME50C
		 case GetDnsIp:		
			memset(DnsIP, 0, sizeof(DnsIP));
			NDK_GetDnsIp(DnsIP, DnsName);
			break;
		case NetSetDns:
		 	memset(szDNS,0,sizeof(szDNS));
	        sprintf(szDNS, "%s;%s;%s", DNS1, DNS2, DNS3);	
	        NDK_NetSetDns(commtype,szDNS);	
			break;
#endif
    	}
	return SUCC;
}
//产生随机数，调用函数 ETH_RandTest进行随机组合测试
void eth200(void)
{
	/*private & local definition*/
	int i = 0, bak = 0,ret = -1,cnt =100, len = 0;//cnt 待定   
	int num[30]={0}, j = 0;
#if !defined ME50C
	char *ethstr[] = {" NDK_EthSetAddress","NDK_EthGetMacAddr","NDK_NetPing","NDK_NetDHCP","NDK_NetGetAddr","NDK_NetAddRouterTable","NDK_NetDnsResolv","NDK_EthGetNetAddr","NDK_GetDnsIp","NDK_EthDisConnect","NDK_NetSetDns"};
#else
    char *ethstr[] = {" NDK_EthSetAddress","NDK_EthGetMacAddr","NDK_NetPing","NDK_NetDHCP","NDK_NetGetAddr","NDK_NetAddRouterTable","NDK_NetDnsResolv","NDK_EthGetNetAddr","NDK_EthDisConnect"};
#endif
	/*process body*/
	if(MaxNum>20)
		len = MaxNum;	 
	else
		len = 20;
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
			if((ret =  ETH_RandTest(num[i])) != NDK_OK)
			{
				for(j=0;j<=i;j++)
					send_result("%s ",ethstr[num[j]]);
				goto ERR;
			}
		}			
	}
	send_result("%s测试通过", TESTAPI);
ERR:
#if !defined ME50C
	NDK_EthDisConnect();
#endif
	return;
}


