/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi 模块
* file name		: wifi1.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WifiProfileGet,NDK_WifiProfileSet能否正确设置列表及获取列表信息的功能
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
#define TESTAPI	"NDK_WifiProfileGet,NDK_WifiProfileSet"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wifi1(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20160223   created 		 
*****************************************************************/
void wifi1(void)
{
	/*private & local definition*/
	int ret = 0, i = 0;
	uchar ap_mac[6] = {0x30,0x31,0x32,0x33,0x34,0x35};
	char ap_ip[16] = "192.168.1.10", ap_netmask[16] = "255.255.255.0", ap_gateway[16] = "192.168.1.254";
	char ap_dns_pri[16] = "218.85.157.99", ap_dns_sec[16] = "1982.168.30.1";
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_PROFILE_LIST list_g;
	ST_WIFI_AP_ENTRY_T set_ap[32], get_ap[32];
	memset(set_ap,0,sizeof(set_ap));//清零
	memset(get_ap,0,sizeof(get_ap));
	list_s.stored_ap = set_ap;
	list_g.stored_ap = get_ap;
	char *ssid[] = {//三条分别表示
					"test0000",
					"`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.",//特殊字符
					"一二三四五",//中文
					};
	char *ssid1[] = {
					"0000",
					"wifi123@$#",
					};
	char *pwd[] = {
					"12345678",
					"1234",
					"",
					};

	/*process body*/
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_WifiStop();//避免因开机自动启动wifi引起的问题
	
	//case1:参数异常
	if((ret = NDK_WifiProfileSet(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiProfileGet(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:(出厂第一次才会为空，之后会保存之前的) 设置空表，获取的应该是空表:设置传入为0  那么就是空表,类似模拟"删"除表格
	list_s.stored_num = 0;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiProfileGet(&list_g)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(list_g.stored_num != 0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, list_g.stored_num);
		RETURN;
	}
	
	//case3:设置三个AP,应该能够成功,类似模拟"增"加表格有3项
	//设置后获取跟获取函数中的参数stored_num有关，其既作为输入参数也作为输出参数；
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET;//第1个ssid为英文,设置DHCP,无优先权
	list_s.stored_ap[1].flag = WIFI_AP_FLAG_MAC_SET;//第2个AP为特殊字符,设置MAC及静态IP,无优先权
	memcpy(list_s.stored_ap[1].mac.octet,ap_mac,6);
	strcpy(list_s.stored_ap[1].ip.ip,ap_ip);
	strcpy(list_s.stored_ap[1].ip.netmask,ap_netmask);
	strcpy(list_s.stored_ap[1].ip.gateway,ap_gateway);
	strcpy(list_s.stored_ap[1].ip.dns_primary,ap_dns_pri);
	strcpy(list_s.stored_ap[1].ip.dns_secondary,ap_dns_sec);
	list_s.stored_ap[2].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET;//第3个AP为中文,使用DHCP,设置优先权
	list_s.stored_ap[2].priority = 1;
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case3.1:列表中存入的是3条信息，期望想取2条，获取的应该是2条
	list_g.stored_num = 2;//既是输入参数也是输出参数 预期想输出几个
	memset(get_ap,0,sizeof(get_ap));
	if( ((ret=NDK_WifiProfileGet(&list_g))!=NDK_OK) || (list_g.stored_num!=2) )//每次get都是一次"查"的动作
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, list_g.stored_num);
		RETURN;
	}
	//case3.2:列表存入的是3条信息，期望想取5条，获取的应该是3条
	list_g.stored_num = 5;
	memset(get_ap,0,sizeof(get_ap));
	if( ((ret=NDK_WifiProfileGet(&list_g))!=NDK_OK) || (list_g.stored_num!=3) )//每次get都是一次"查"的动作
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, list_g.stored_num);
		RETURN;
	}
	
	//case4:测试获取信息的正确性
	for(i=0;i<3;i++)
	{
		if(strcmp(list_g.stored_ap[i].ssid,ssid[i]) || strcmp(list_g.stored_ap[i].pwd,pwd[i]))
		send_result("line %d: %s测试失败(%s,%s)", __LINE__, TESTAPI, list_g.stored_ap[i].ssid,list_g.stored_ap[i].pwd);
		RETURN;	
	}
	if(list_g.stored_ap[0].flag != WIFI_AP_FLAG_DHCP_SET || list_g.stored_ap[0].priority != 0 || 
		list_g.stored_ap[1].flag != WIFI_AP_FLAG_MAC_SET ||  list_g.stored_ap[1].priority != 0 ||
		list_g.stored_ap[2].flag != (WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET)||  list_g.stored_ap[2].priority != 1 )
	{
		send_result("line %d: %s测试失败(%d,%d,%d,%d,%d,%d)", __LINE__, TESTAPI, list_g.stored_ap[0].flag, list_g.stored_ap[0].priority,
			 list_g.stored_ap[1].flag, list_g.stored_ap[1].priority, list_g.stored_ap[2].flag, list_g.stored_ap[2].priority);
		RETURN;	
	}
	if(memcmp(list_g.stored_ap[1].mac.octet,ap_mac,6))
	{
		send_result("line %d: %s测试失败(mac:%02x:%02x:%02x:%02x:%02x:%02x,)", __LINE__, TESTAPI,list_g.stored_ap[1].mac.octet[0],list_g.stored_ap[1].mac.octet[1],
			list_g.stored_ap[1].mac.octet[2],list_g.stored_ap[1].mac.octet[3],list_g.stored_ap[1].mac.octet[4],list_g.stored_ap[1].mac.octet[5]);
		RETURN;	
	}
	if(strcmp(list_g.stored_ap[1].ip.ip,ap_ip) || strcmp(list_g.stored_ap[1].ip.netmask,ap_netmask) || strcmp(list_g.stored_ap[1].ip.gateway,ap_gateway) 
		|| strcmp(list_g.stored_ap[1].ip.dns_primary,ap_dns_pri) || strcmp(list_g.stored_ap[1].ip.dns_secondary,ap_dns_sec))
	{
		send_result("line %d: %s测试失败(%s,%s,%s,%s,%s)", __LINE__, TESTAPI,list_g.stored_ap[1].ip.ip,list_g.stored_ap[1].ip.netmask,list_g.stored_ap[1].ip.gateway,
			list_g.stored_ap[1].ip.dns_primary,list_g.stored_ap[1].ip.dns_secondary);
		RETURN;	
	}
	
	//case5:进行两次设置且设置不同内容，获取的应该是后一次的情况
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET;
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)//"增"加3条 AP1,AP2,AP3
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<2;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid1[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET;
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_num = 2;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)//"减"掉上面的3条,增加2条新的
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	list_g.stored_num = 3;//虽然想获取3条但实际应该只能获取2条
	memset(get_ap,0,sizeof(get_ap));
	if( ((ret=NDK_WifiProfileGet(&list_g))!=NDK_OK) || (list_g.stored_num!=2) )//每次get都是一次"查"的动作
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, list_g.stored_num);
		RETURN;
	}
	//判断list_g 应该剩下2条新的AP信息
	for(i=0;i<2;i++)
	{
		if(strcmp(list_g.stored_ap[i].ssid,ssid1[i]) || strcmp(list_g.stored_ap[i].pwd,pwd[i])|| list_g.stored_ap[i].flag != WIFI_AP_FLAG_DHCP_SET || list_g.stored_ap[i].priority != 0)
		{
			send_result("line %d: %s测试失败(%s,%s,%d,%d)", __LINE__, TESTAPI, list_g.stored_ap[i].ssid,list_g.stored_ap[i].pwd, list_g.stored_ap[i].flag,list_g.stored_ap[i].priority);
			RETURN;	
		}
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

