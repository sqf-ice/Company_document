/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: eth模块
* file name		: eth9.c
* Author 			: huangjianb
* version			: 
* DATE				: 20140623
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
*			  						huangjianb	20140623	 		created
*****************************************************************/
void dns1(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1;
	char DnsName[16] = "www.baidu.com", DnsName126[16] = "www.126.com";
	char ErrDnsName[16] = "err1111111", ErrDnsName1[16] = "err2222222";
	char LongDnsName[60] = "hardware-upgrade.oss-cn-hangzhou.aliyuncs.com";
	char DnsIP[16] = {0}, DnsIP126[16] = {0}, DnsIP1[16] = {0},DnsIPlong[60] = {0};
	LinkType dnstype = 0;
	EM_COMM_TYPE commtype = 0;
	
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}

	dnstype = getlinktype();
	switch(dnstype)
	{
		case GPRS:
		case CDMA:
		case TD:
		case ASYN:
			commtype = COMM_TYPE_PPP;
			break;
		case NONE:
		case SYNC:
		case SERIAL:
			commtype = COMM_TYPE_UNKNOW;
			break;
		default:
		case ETH:
			commtype = COMM_TYPE_ETH;
			break;
		case WLAN:
			commtype = COMM_TYPE_WIFI;
			break;
		case BT:
			commtype = COMM_TYPE_BTH;
			break;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	//case1:传入非法参数,不存在域名
	if(((ret=NDK_NetDnsResolv(commtype,NULL, DnsName)) != NDK_ERR_PARA)
		||((ret1=NDK_NetDnsResolv(commtype,DnsIP, NULL)) != NDK_ERR_PARA)
		||((ret2=NDK_NetDnsResolv(COMM_TYPE_UNKNOW,DnsIP, DnsName)) != NDK_ERR_NET_UNKNOWN_COMMTYPE)){
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1,ret2);
		GOTOERR;
	}

	//case5:新增传入的域名无法解析，应返回错误  20180517  sull add  //解析错误网址时跟服务器有关系进行代码优化
	memset(DnsIP, 0, sizeof(DnsIP));
	if((ret=NDK_NetDnsResolv(commtype,DnsIP,ErrDnsName)) != NDK_ERR)
	{
		if(ret == NDK_OK)  
		{			
			if((ret=NDK_NetDnsResolv(commtype,DnsIP1,ErrDnsName1)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if(strcmp(DnsIP, DnsIP1))
			{
				send_result("line %d:%s测试失败(DnsIP = %s, DnsIP1 = %s)", __LINE__, TESTAPI, DnsIP, DnsIP1);
				GOTOERR;
			}
		}
		else
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}		
	
	//case2:正常测试,此处可以通过在入口处调用不同的通讯方式、DHCP方式等验证各种正确设置情况下域名解析是否正确
	memset(DnsIP, 0, sizeof(DnsIP));
	if ((ret=NDK_NetDnsResolv(commtype,DnsIP, DnsName)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}

	//case3:连续调用
	memset(DnsIP, 0, sizeof(DnsIP));
	if ((ret=NDK_NetDnsResolv(commtype,DnsIP, DnsName)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}

	//case4:新增测试解析www.126.com  20161019
	memset(DnsIP126, 0, sizeof(DnsIP126));
	if ((ret=NDK_NetDnsResolv(commtype,DnsIP126, DnsName126)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIP126))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP126);
		goto ERR;
	}

	//case5:新增测试解析长域名20180830
	memset(DnsIPlong, 0, sizeof(DnsIPlong));
	if ((ret=NDK_NetDnsResolv(commtype,DnsIPlong, LongDnsName)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIPlong))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIPlong);
		goto ERR;
	}
	send_result("长域名:%s,IP:%s", LongDnsName, DnsIPlong);
	
	//测试结束
	send_result("%s测试通过!百度域名:%s IP:%s,126域名:%s IP:%s,ETH和WIFI请分别设置动态IP和静态IP后重复本用例测试", TESTAPI, DnsName, DnsIP, DnsName126, DnsIP126);
ERR:
	NetDown();
	return;
}

