/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth1.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_EthSetAddress和NDK_EthGetNetAddr接口功能
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
#define 	TESTAPI		"NDK_EthSetAddress,NDK_EthGetNetAddr"
//#define		MAX_SIZE	128
#define		MAXSETTIME	2//静态设置IP地址的时间上限

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
void eth1(void)
{
	//以下为局部变量
   	int ret=-1, ret1 = -1, ret2 = -1, ret3 = -1,ret4 = -1, i = 0;
	char szIPaddr[16] = {0}, szMaskaddr[16] = {0}, szGateway[16] = {0}, szDNS[16*3+2] = {0}, szDNS3[16*3+2] = {0};
	float settime = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:传入非法参数,参数为NULL
	if((ret=NDK_EthSetAddress(NULL, NULL, NULL, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_EthGetNetAddr(NULL, NULL, NULL, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:传入非法参数,参数为不正确的地址
	if(((ret=NDK_EthSetAddress("192.168.4", netsetting.LocalMask.Str, netsetting.LocalGateway.Str, NULL)) != NDK_ERR_PARA/*NDK_ERR*/)//底层增加地址非法判断返回 NDK_ERR_PARA
		||((ret1=NDK_EthSetAddress(netsetting.LocalIP.Str, "255.255.255", netsetting.LocalGateway.Str, NULL)) != NDK_ERR_PARA/*NDK_ERR*/)
		||((ret2=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, "192.168.4.2254", NULL)) != NDK_ERR_PARA/*NDK_ERR*/)
		||((ret3=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, "")) != NDK_ERR)
		/*||((ret4=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str,"256.256.256.256")) != NDK_ERR_NET_ADDRILLEGAL)*/){//设置非法的DNS 应该返回NDK_ERR_NET_ADDRILLEGAL 获取地址格式错误20140926 jiangym add
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3/*, ret4*/);
		RETURN;
	}
#if K21_ENABLE //ME50C 返回-6 20180428modify
	if((ret4=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str,"256.256.256.256")) != NDK_ERR_PARA)
#else
	if((ret4=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str,"256.256.256.256")) != NDK_ERR_NET_ADDRILLEGAL)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret4);
		RETURN;
	}

#if K21_ENABLE    //根据开发解释低端平台返回NDK_ERR_PARA  20180319 modify
	if((ret4=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str,"256.256.256.256")) != NDK_ERR_PARA)
#else
	if((ret4=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str,"256.256.256.256")) != NDK_ERR_NET_ADDRILLEGAL)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret4);
		RETURN;
	}

	//case3:非ipv4格式,ipv6格式
	cls_printf("测试设置IPv6...");
	if ((ret=NDK_EthSetAddress("AAAA.AAAA.AAAA.AAAA.AAAA.AAAA", "FFFF.FFFF.FFFF.FFFF.FFFF.0000", "AAAA.AAAA.AAAA.AAAA.AAAA.FFFE", NULL)) != NDK_ERR_PARA/*NDK_ERR*/)//底层增加地址串长度超过16直接返回参数错误的判断,所以IPV6超过16所以直接返回了参数错误
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:正确IPv4格式,且不设置DNS
	cls_printf("测试设置IPv4...");
	if ((ret=NDK_EthSetAddress("1.0.0.1", "255.0.0.0", "1.0.0.254", NULL)) != NDK_OK)//不是C类地址,但符合IPv4也应该设置成功
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthGetNetAddr(szIPaddr, szMaskaddr, szGateway, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (strcmp(netsetting.LocalIP.Str, szIPaddr) || strcmp(netsetting.LocalMask.Str, szMaskaddr) || strcmp(netsetting.LocalGateway.Str, szGateway))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
#if !defined ME50C  // 低端没有状态栏 不做判断 20180316 modify
	if(auto_flag==0||auto_flag==2)
	{
		//case9:NDK_EthSetAddress函数调用成功后且插网线情况下,状态栏图标显示:已连接网线图标 20140618  jiangym
		if(cls_show_msg("状态栏图标是否显示已连接网线图标,是[Enter]继续,否[其他]失败")!=ENTER)
		{
			send_result("line %d:状态栏图标应该显示已连接网线", __LINE__);
			RETURN;
		}
	}
	else
		send_result("line %d:%s状态栏图标已连接网线图标需另外手动测试", __LINE__, TESTAPI);
#endif

	//case5:设置一个DNS服务器IP地址
	cls_printf("测试设置DNS服务器...");
	memset(szDNS,0,sizeof(szDNS));
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, DNS3)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthGetNetAddr(szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (memcmp(DNS3, szDNS, strlen(DNS3)))
	{
		send_result("line %d:%s测试失败DNS:%s", __LINE__, TESTAPI, szDNS);
		RETURN;
	}

	//case6:设置三个DNS服务器IP地址
	memset(szDNS3,0,sizeof(szDNS3));
	memset(szDNS,0,sizeof(szDNS));
	sprintf(szDNS3, "%s;%s;%s", DNS1, DNS2, DNS3);
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, szDNS3)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(szDNS, 0, sizeof(szDNS));
	if ((ret=NDK_EthGetNetAddr(szIPaddr, szMaskaddr, szGateway, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (strcmp(szDNS3, szDNS))
	{
		send_result("line %d:%s测试失败DNS:%s", __LINE__, TESTAPI, szDNS);
		RETURN;
	}
	
	//case7:单独设置每一项
	cls_printf("测试单独设置每个参数...");
	if ((ret=NDK_EthSetAddress("1.0.0.1", "255.0.0.0", "1.0.0.254", "192.168.30.1")) != NDK_OK)	//先重置每项参数
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, NULL, NULL, NULL)) != NDK_OK)	//单独设置IP
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthSetAddress(NULL, netsetting.LocalMask.Str, NULL, NULL)) != NDK_OK)	//单独设置Mask
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthSetAddress(NULL, NULL, netsetting.LocalGateway.Str, NULL)) != NDK_OK)	//单独设置网关
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthSetAddress(NULL, NULL, NULL, DNS3)) != NDK_OK)	//单独设置DNS
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(szIPaddr, 0, sizeof(szIPaddr));
	memset(szMaskaddr, 0, sizeof(szMaskaddr));
	memset(szGateway, 0, sizeof(szGateway));
	memset(szDNS, 0, sizeof(szDNS));
	if ((ret=NDK_EthGetNetAddr(szIPaddr, NULL, NULL, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthGetNetAddr(NULL, szMaskaddr, NULL, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthGetNetAddr(NULL, NULL, szGateway, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_EthGetNetAddr(NULL, NULL, NULL, szDNS)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (strcmp(netsetting.LocalIP.Str, szIPaddr) 
		|| strcmp(netsetting.LocalMask.Str, szMaskaddr) 
		|| strcmp(netsetting.LocalGateway.Str, szGateway)
		|| memcmp(DNS3, szDNS,strlen(DNS3)))
	{
		send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, szDNS);
		RETURN;
	}

	//case9:应允许所设置的本地IP所在子网和所设置的网关不在同一子网的情况,设置应该成功 20160608
	if ((ret=NDK_EthSetAddress("40.40.40.103", "255.255.255.0", "172.16.60.1", netsetting.DNSHost[1]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(szIPaddr, 0, sizeof(szIPaddr));
	memset(szMaskaddr, 0, sizeof(szMaskaddr));
	memset(szGateway, 0, sizeof(szGateway));
	if ((ret=NDK_EthGetNetAddr(szIPaddr, szMaskaddr, szGateway, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (strcmp("40.40.40.103", szIPaddr) 
		|| strcmp("255.255.255.0", szMaskaddr) 
		|| strcmp("172.16.60.1", szGateway))
	{
		send_result("line %d:%s测试失败%s %s %s", __LINE__, TESTAPI, szIPaddr, szMaskaddr, szGateway);
		RETURN;
	}
	
	//case8:测试静态设置功能的执行不应大于2s(性能值规定)
	cls_printf("测试静态设置IP时间性能中,请稍等");
	lib_StartStopwatch();
	for(i=0;i<20;i++)
	{
		if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1]))!=NDK_OK)
		{                                                                                            
			send_result("line %d:%s测试失败(%d)", __LINE__, ret);
			RETURN;
		}
	}
	if((settime=lib_ReadStopwatch()/20)>MAXSETTIME)
	{
		send_result("line %d:%s测试失败(NDK_EthSetAddress时间:%f)", __LINE__, TESTAPI, settime);
		RETURN;
	}
	
	//测试结束
#if K21_ENABLE
	memset(str,0,sizeof(str));
	ftos(settime,str);
	send_result("%s测试通过(NDK_EthSetAddress时间:%s)", TESTAPI, str);
#else
	send_result("%s测试通过(NDK_EthSetAddress时间:%f)", TESTAPI, settime);
#endif
	return;
}

