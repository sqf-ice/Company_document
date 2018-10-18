/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: eth模块
* file name		: eth10.c
* Author 			: linwl
* version			: 
* DATE				: 20140623
* directory 	: 
* description	: 测试NDK_NetSetDns接口设置DNS功能
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
#define 	TESTAPI		"NDK_NetSetDns"

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
*			  				linwl			20150703	 		created
*****************************************************************/
void dns2(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1, ret2 = -1, ret3= -1;
	char DnsName[16] = "www.baidu.com";
	//char baiduip[16] = "115.239.211.112";//百度的IP地址
	char DnsIP[16] = {0};
	LinkType dnstype = 0;
	EM_COMM_TYPE commtype = 0;
	char szDNS[16*4+2] = {0};
	
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

	memset(szDNS,0,sizeof(szDNS));
	sprintf(szDNS, "%s;%s;%s;%s", DNS1, DNS2, DNS3, "192.168.30.5");
	//case1:传入非法参数
	if((ret=NDK_NetSetDns(COMM_TYPE_BTH,NULL))!=NDK_ERR_PARA
		||(ret1=NDK_NetSetDns(COMM_TYPE_BTH,szDNS))!=NDK_ERR_NET_UNKNOWN_COMMTYPE
		||(ret2=NDK_NetSetDns(COMM_TYPE_UNKNOW,DNS1))!=NDK_ERR_NET_UNKNOWN_COMMTYPE
		||(ret3=NDK_NetSetDns(COMM_TYPE_ETH,"256.256.256.256"))!=NDK_ERR_NET_ADDRILLEGAL
		)
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3);
		goto ERR;
	}
	
	//case2:设置错误的DNS,进行域名解析应该失败,预期获取到的IP为空
	if((ret=NDK_NetSetDns(commtype,DNS1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	memset(DnsIP, 0, sizeof(DnsIP));
	if((ret=NDK_NetDnsResolv(commtype, DnsIP, DnsName))!= NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(strlen(DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}
	
	//case3:设置正确的DNS,进行域名解析应该成功,其中DNS2是正确的
	memset(szDNS,0,sizeof(szDNS));
	sprintf(szDNS, "%s;%s;%s", DNS1, DNS2, DNS3);	
	if((ret=NDK_NetSetDns(commtype,szDNS))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	memset(DnsIP, 0, sizeof(DnsIP));
	if((ret=NDK_NetDnsResolv(commtype, DnsIP, DnsName))!= NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIP))
	//if(strcmp(baiduip,DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}
	//send_result("case1测试通过,域名:%s IP:%s", DnsName, DnsIP);
	
	//case4:设置NULL,预期不会对DNS列表进行修改，域名解析应该成功
	if((ret=NDK_NetSetDns(commtype,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	memset(DnsIP, 0, sizeof(DnsIP));
	if((ret=NDK_NetDnsResolv(commtype, DnsIP, DnsName))!= NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(!strlen(DnsIP))
	//if(strcmp(baiduip,DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}
	//send_result("case1测试通过,域名:%s IP:%s", DnsName, DnsIP);
	
	//case5:连续设置应该是最后一次生效,设置设置正确再设置错误的DNS,进行域名解析应该失败
	if((ret=NDK_NetSetDns(commtype,DNS2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_NetSetDns(commtype,DNS3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	memset(DnsIP, 0, sizeof(DnsIP));
	if((ret=NDK_NetDnsResolv(commtype, DnsIP, DnsName))!= NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(strlen(DnsIP))
	{
		send_result("line %d:%s测试失败IP:%s", __LINE__, TESTAPI, DnsIP);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过!", TESTAPI);
ERR:
	NetDown();
	return;
}

