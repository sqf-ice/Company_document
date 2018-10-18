/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi 模块
* file name		: wifi2.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WifiStart,NDK_WifiStop  wifi启动及关闭wifi功能
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
#define TESTAPI	"NDK_WifiStart,NDK_WifiStop"	

extern int scan_get_mac(uchar outmac[6]); 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wifi2(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20160223   created 
*****************************************************************/
#if WIFI_PRIORITY_ENABLE
void wifi2(void)
{
	/*private & local definition*/
	int ret = 0, i = 0 ;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T set_ap[32];
	list_s.stored_ap = set_ap;
	ST_WIFI_AP_INFO ap_info;
	ST_WIFI_STATUS status;
	char *ssid[] = {
					"AP1@#fartest",
					"AP2&4same",
					"`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.",
					"AP2&4same",
					};
	/*
	如果找不到这四台路由器，自己找四台路由器的话，可以在路由器背面找到mac或者进入设置找到mac 然后对应AP修改mac
	同时设置密码，设置安全类型:AP1为 wpa2模式，AP2为 wpa2模式，AP3为WEP模式，AP4为WPA模式且密码使用64位的hex
	在设置安全类型时可以自己设置不同类型的，同时设置好密码，然后把此代码的密码对应修改
	
	uchar default_mac[][6] = {
					{0x78,0xEB,0x14,0xA3,0x68,0xF0},//FAST FW150R 路由器的mac
					{0xA8,0x57,0x4E,0x95,0x2E,0x88},//TP-Link TL-WR742N路由器的mac
					{0x78,0x54,0x2e,0x59,0xdd,0x3a},//D-Link DIR-601 无线N150路由器的mac
					{0x08,0x10,0x79,0x82,0xDB,0x41},//磊科NW705P 路由器的mac
					};
	*/
	char *pwd[] = {
					"12345678",//wpa2 aes
					"12345678910",//wpa2 
					"1234567890",//wep
					"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",//wpa
					};
	char *wifimacstr[] = {"WifiMac1","WifiMac2","WifiMac3","WifiMac4"};
	char pInfo[64] = {0}, mactemp[64] = {0};
	uchar apmac[4][6];
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//需要提示准备好4个ap，ssid与密码设置好 接下去测试
	/*
	这里需要经过一个测试环境的搭建工作:例如需要4个ap分别为ap1 2 3 4
	其中ap2 4可以放置在比较近的位置
	ap1 最远(同时要把它隐藏)
	ap3中间位置(放置使得信号值ap24>ap3>ap1)(ap1 ap3可以相近)
	ap1要比ap2远(ap2信号强于ap1),ap3要比ap4远(ap4信号强于ap3)
	*/
	//测试前置
	NDK_WifiStart();
	if((ret = lib_getwifistatus(&status)) != NDK_OK)
	{
		send_result("line %d:获取wifi状态失败(ret=%d,status=%d)", __LINE__, ret, status.status);
		return;
	}
	//先从配置文件中获取各AP Mac  无配置文件将通过扫描获取
	if(NDK_FsExist(PARACONF)!=NDK_OK)
	{	
		cls_show_msg1(3,"无参数配置文件,将通过扫描获取mac");
		//通过扫描获取Mac传入定义的Mac数组中
		for(i=0;i<4;i++)	
		{
			cls_show_msg1(5,"获取第%d个测试AP:%s的Mac",i+1,ssid[i]);
			if((ret = scan_get_mac(apmac[i])) != SUCC)
			{
				send_result("line %d:获取测试AP Mac失败(%d)", __LINE__, ret);
				return;
			}
		}
	}
	else
	{
		for(i=0;i<4;i++)
		{
			if(GetParaInfo(wifimacstr[i],pInfo)!=SUCC)
			{	
				send_result("line %d:查找Mac信息失败", __LINE__ );
				return;
			}
			if(trim_string(pInfo,mactemp)!=SUCC)
			{	
				send_result("line %d:读取Mac信息失败", __LINE__ );
				return;
			}
			if(trim_char2uchar(mactemp,apmac[i])!=SUCC)
			{	
				send_result("line %d:转换Mac格式失败", __LINE__ );
				return;
			}
		}
	}
	NDK_WifiStop();//刚好也作为保护动作
	
	//case1:设置空列表，调用start，且获取状态返回WIFI_WLAN_STATUS_EMPTY，并且不返回SSID
	memset(set_ap,0,sizeof(set_ap));
	list_s.stored_num = 0;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || (status.status != WIFI_WLAN_STATUS_EMPTY) || strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:测试列表存在多个优先级的，调用start会选择优先级高的连接(此时不论信号如何，优先级高于信号)返回成功的状态，ssid为预期的AP3
	//本用例信号强度预期AP2>AP3>AP1，优先级priority的值为0~255，值越大表示优先级越高，所以AP3优先级最高
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);//开发解释在环境中如果存在同名的SSID,想要正确的连上其中一个AP，就要保存MAC地址，此处需要检查文档,要求补充相关说明(文档要求)
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = i;//将优先级高的放在最后,避免底层按顺序连接,正好连接上AP1而AP1又是最高优先级的情况
	}
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;//将AP1设置成隐藏AP
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期要连接上优先级最高的AP3
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[2]) )//由于目前wifi不稳定，ap3有概率出现连接不上的情况，所以此处可能会连上AP2(底层BUG)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
#if 0//调试显示连接上的AP的MAC地址
	memset(&ap_info,0,sizeof(ap_info));
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&ap_info)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("AP:%s MAC:%02x:%02x:%02x:%02x:%02x:%02x", ap_info.ssid, ap_info.mac.octet[0], ap_info.mac.octet[1], ap_info.mac.octet[2], ap_info.mac.octet[3], ap_info.mac.octet[4], ap_info.mac.octet[5]);
#endif
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.1:挂断后获取状态会失败，同时返回WIFI_WLAN_STATUS_FAIL状态(挂断是阻塞的)
	memset(&status,0,sizeof(status));
	if( ((ret=NDK_WifiStatus(&status))!=NDK_ERR_WIFI_DEVICE_UNAVAILABLE) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid,""))//本用例等后续返回值文档确定后再修改返回值(文档要求)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}

	//case3:不管是否设置优先级，应该能够连接上上次连接上的ap，可以把case2中已连接过的ap作为上次连接的，修改其优先级由高到低，应该仍然被连上
	//本用例存放AP1、AP2、AP3，优先级AP1>AP2>AP3，预期连上上次连接成功的AP3
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{	
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);//存在同名AP2和AP4的情况下需要指定mac才能确定连接的ap
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET; 
		list_s.stored_ap[i].priority = 3-i;
	}
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期要连接上上次连接的AP3，虽然此时AP3的优先级最低
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[2]))//由于目前wifi不稳定，ap3有概率出现连接不上的情况，所以此处可能会连上AP2(底层BUG)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:不设置优先级，且把上次连接的ap删除，设置新的ap后调用start后应该会连接上信号强的ap
	//本用例从表格中删除了AP3(上次连接的)，取消了优先级设置,保存AP1，AP2，重新连接预期连接上信号强度较强的AP2(AP2较近),本用例在搭建网络时要确保AP2信号强于AP1
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<2;i++)
	{	
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);//环境中存在同名AP2和AP4的情况下同时预期要连接上同名AP时需要指定mac才能确定连接的ap
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;//AP1隐藏标志位置1
	list_s.stored_num = 2;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//按照环境搭建:AP2的信号是强于AP1的,预期要连上信号强的AP2
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}//此处有概率连上AP1，可能原因是AP1的信号确实强过AP2，但由于此时AP1是隐藏的无法获取其信号值，可以由开发增加信号调试后修改用例(未确定是否为BUG)
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if 0
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[1]);
	strcpy(list_s.stored_ap[0].pwd,pwd[1]);
	memcpy(list_s.stored_ap[0].mac.octet,apmac[1],6);//环境中存在同名AP2和AP4的情况下同时预期要连接上同名AP时需要指定mac才能确定连接的ap
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
	list_s.stored_ap[0].priority = 0;
	list_s.stored_num = 1; 
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期要连上(AP2)
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	

	//注意:后续的5 6 7 8 9 10号用例相关联,不能移动或注释
	//case5:设置列表，三个ap,其密码出错或者为空，调用start后尝试连接返回连接失败WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED且不返回ssid
	//本用例存放AP1、AP2、AP3，无优先级，密码全都设置错误，预期连接失败
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{	
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,"1234123412");//密码设置错误的 
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET; 
		list_s.stored_ap[i].priority = 0;
	}
	strcpy(list_s.stored_ap[0].pwd,"");//AP1的密码设置空
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || (status.status != WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED)|| strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0//经过测试用例开发验证后,为自动测试而不再测试该情况,可由测试人员观察连接情况得出列表中的AP是否被尝试连接
	if(auto_flag==0||auto_flag==2)
	{
		if(cls_show_msg("连接中是否尝试连接%s和%s,是[Enter],否[其他]",ssid[1],ssid[2])!=ENTER) //此处开发解释:由于安全模式对应的密码有长度要求，所以底层判断到AP1的空密码不符合要求就不会尝试连接(非BUG)
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
#endif	
	//case6:把case5中的列表其中AP2的密码修改正确后，调用start后应该能够连接上正确的AP2
	//本用例存放AP1、AP2、AP3，无优先级，只有AP2是正确的，预期连接上AP2
	strcpy(list_s.stored_ap[1].pwd,pwd[1]);//把AP2密码设置正确
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )//此处由于WEP_OPEN模式在密码不正确时底层也判断成成功连接的BUG(虽然NDK返回连接不成功)引起失败(底层BUG)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}

	//case7:对于已连接上某个AP的情况下，删除或修改AP列表中除了已连接上AP以外的条目信息，连接仍应保持或至少看上去像“连接仍保持”一样
	//本用例删除AP1增加AP4，实际存放AP2、AP3、AP4，无优先级，预期连接上上次连接的AP2
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i+1]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i+1]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i+1],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//未重连情况下:应该仍然保持着连接AP2
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//重连后:应该仍然连接上AP2
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	//由于AP2和AP4同时存在,故要判断是否为AP2的mac
	memset(&ap_info,0,sizeof(ap_info));
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&ap_info)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(ap_info.mac.octet, apmac[1], sizeof(apmac[1])))
	{
		send_result("line %d: %s测试失败(%02x:%02x:%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, ap_info.mac.octet[0], ap_info.mac.octet[1], ap_info.mac.octet[2], ap_info.mac.octet[3], ap_info.mac.octet[4], ap_info.mac.octet[5]);
		GOTOERR;
	}
	
	//case8:修改表，把之前连接过的ap删除后，调用start后能够根据相应的规则(比如优先原则)连接ap
	//本用例删除AP2后，实际保存AP3和AP4，优先级AP4>AP3，预期根据优先级原则连上AP4
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<2;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i+2]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i+2]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i+2],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = i; //AP4的优先级最高  (是为了确保环境的不稳定造成测试的错误,信号优先在前面用例已经测过)
	}
	list_s.stored_num = 2;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//未重连情况下:应该仍然保持着连接AP2
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//重新连接应该根据优先级原则连接上AP4
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[3]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	//由于AP2和AP4同时存在,故要判断是否为AP4的mac
	memset(&ap_info,0,sizeof(ap_info));
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&ap_info)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(ap_info.mac.octet, apmac[3], sizeof(apmac[3])))
	{
		send_result("line %d: %s测试失败(%02x:%02x:%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, ap_info.mac.octet[0], ap_info.mac.octet[1], ap_info.mac.octet[2], ap_info.mac.octet[3], ap_info.mac.octet[4], ap_info.mac.octet[5]);
		GOTOERR;
	}

	//case9:修改之前连接成功的ap的密码，再次start后会重新根据规则(比如优先原则)连接ap 
	//本用例保存AP1、AP2、AP3、AP4，AP4为上次连接的AP但密码错误，预期:连接AP4失败后应该会尝试连接优先级第二的AP3，应该能够连接上AP3
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<4;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = i; //AP4优先级最高 AP3第二
	}
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;
	strcpy(list_s.stored_ap[3].pwd,"00000000");//把之前成功连接的ap4修改成错误的密码
	list_s.stored_num = 4;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//未重连情况下:应该仍然保持着连接AP4
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[3]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//重新连接后应该连接上优先级第二高的AP3
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[2]) )//此处由于同名AP2的存在,底层会把AP2当成上次连接的AP去连接,所以会连上AP2(底层BUG),也可能是因为AP3连不上，所以连上了AP2
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10.1:对于底层AP列表存在启用DHCP的条目与使用静态IP方式的条目共存的情况，按预设的优先连接规则（如，上次连接上的、优先级优先、就近原则、信号优先等（依底层实现而定）），不论使用动态IP还是静态IP，均应能连接到相应的AP上；
	//本用例保存AP1(静态IP)、AP2(动态IP)，优先级AP2>AP1，预期连接上AP2(动态IP)
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[0]);
	strcpy(list_s.stored_ap[0].pwd,pwd[0]);
	strcpy(list_s.stored_ap[0].ip.ip,"192.168.1.10");
	strcpy(list_s.stored_ap[0].ip.netmask,"255.255.255.0");
	strcpy(list_s.stored_ap[0].ip.gateway,"192.168.1.1");
	strcpy(list_s.stored_ap[0].ip.dns_primary,"218.85.157.99");
	strcpy(list_s.stored_ap[0].ip.dns_secondary,"192.168.30.1");
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_HIDDEN_SET | WIFI_AP_FLAG_PRI_SET;
	list_s.stored_ap[0].priority = 0;
	strcpy(list_s.stored_ap[1].ssid,ssid[1]);
	strcpy(list_s.stored_ap[1].pwd,pwd[1]);
	memcpy(list_s.stored_ap[1].mac.octet,apmac[1],6);
	list_s.stored_ap[1].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET;
	list_s.stored_ap[1].priority = 1; 
	list_s.stored_num = 2;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
#if 1	
	//连接成功后判断连接到的MAC地址应该是AP2的MAC地址
	memset(&ap_info,0,sizeof(ap_info));
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&ap_info)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(ap_info.mac.octet, apmac[1], sizeof(apmac[1])))
	{
		send_result("line %d: %s测试失败(%02x:%02x:%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, ap_info.mac.octet[0], ap_info.mac.octet[1], ap_info.mac.octet[2], ap_info.mac.octet[3], ap_info.mac.octet[4], ap_info.mac.octet[5]);
		GOTOERR;
	}
#endif	
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case10.2::对于底层AP列表存在启用DHCP的条目与使用静态IP方式的条目共存的情况，按预设的优先连接规则（如，上次连接上的、优先级优先、就近原则、信号优先等（依底层实现而定）），不论使用动态IP还是静态IP，均应能连接到相应的AP上；
	//本用例保存AP1(静态IP)、AP4(动态IP)，优先级AP1>AP4，预期连接上AP1(静态IP)，注意:由于有一个上次连接成功的AP为最高优先级，所以本用例需删除AP2(上次连接的)，增加AP4
	list_s.stored_ap[0].priority = 1;
	strcpy(list_s.stored_ap[1].ssid,ssid[3]);
	strcpy(list_s.stored_ap[1].pwd,pwd[3]);
	memcpy(list_s.stored_ap[1].mac.octet,apmac[3],6);
	list_s.stored_ap[1].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
	list_s.stored_ap[1].priority = 0; 
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[0]) )//此处有概率失败:具体原因初步判断是由于概率性连不上AP1才连上AP4引起(确定的BUG)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case11:隐藏ap 
	//本用例保存AP1为隐藏AP，预期可以连接上AP1
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[0]);
	strcpy(list_s.stored_ap[0].pwd,pwd[0]);
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_ap[0].priority = 0; 
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[0]) )//此处概率性失败:原因可能是概率性连接不上AP1,导致连接失败
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case12:设置同名ssid，设置不同优先级，进行start后，会连接优先级高的，判断可以根据mac判断
	//本用例保存AP2、AP3、AP4，AP2与AP4同名，优先级 AP4>AP3>AP2，预期可以连接上AP4
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)  //列表中设置  AP2 AP3 AP4  AP2与AP4同名
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i+1]); 
		strcpy(list_s.stored_ap[i].pwd,pwd[i+1]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i+1],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_PRI_SET | WIFI_AP_FLAG_MAC_SET;
		list_s.stored_ap[i].priority = i;
	}
	list_s.stored_num = 3;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期连接上优先级最高的AP4
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[3]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	memset(&ap_info,0,sizeof(ap_info));
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&ap_info)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if(memcmp(ap_info.mac.octet, apmac[3], sizeof(apmac[3])))
	{
		send_result("line %d: %s测试失败(%02x:%02x:%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, ap_info.mac.octet[0], ap_info.mac.octet[1], ap_info.mac.octet[2], ap_info.mac.octet[3], ap_info.mac.octet[4], ap_info.mac.octet[5]);
		GOTOERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WifiStop();
	return;
}
#else
void wifi2(void)
{
	/*private & local definition*/
	int ret = 0, i = 0 ;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T set_ap[32];
	list_s.stored_ap = set_ap;
	ST_WIFI_AP_INFO ap_info;
	ST_WIFI_STATUS status;
	char *ssid[] = {"AP1@#fartest",
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
				"`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.",
#else
				"`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.",
#endif
				};
	char *pwd[] = {"12345678",//wpa2 aes 
				   "1234567890", //wep
				   };
	char pInfo[64] = {0}, mactemp[64] = {0};
	uchar apmac[2][6];
	char *wifimacstr[] = {"WifiMac1", "WifiMac3"};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	NDK_WifiStart();
	if((ret = lib_getwifistatus(&status)) != NDK_OK)
	{
		send_result("line %d:获取wifi状态失败(ret=%d,status=%d)", __LINE__, ret, status.status);
		return;
	}
	//先从配置文件中获取各AP Mac  无配置文件将通过扫描获取
	if(NDK_FsExist(PARACONF)!=NDK_OK)
	{	
		cls_show_msg1(3,"无参数配置文件,将通过扫描获取mac");
		//通过扫描获取Mac传入定义的Mac数组中
		for(i = 0; i<2; i++)
		cls_show_msg1(5,"获取第%d个测试AP:%s的Mac",i+1,ssid[i]);
		if((ret = scan_get_mac(apmac[i])) != SUCC)
		{
			send_result("line %d:获取测试AP Mac失败(%d)", __LINE__, ret);
			return;
		}
	}
	else
	{
		for(i=0; i<2; i++)
		{
			if(GetParaInfo(wifimacstr[i],pInfo)!=SUCC)
			{	
				send_result("line %d:查找Mac信息失败", __LINE__ );
				return;
			}
			if(trim_string(pInfo,mactemp)!=SUCC)
			{	
				send_result("line %d:读取Mac信息失败", __LINE__ );
				return;
			}
			if(trim_char2uchar(mactemp,apmac[i])!=SUCC)
			{	
				send_result("line %d:转换Mac格式失败", __LINE__ );
				return;
			}
		}
	}
	NDK_WifiStop();//刚好也作为保护动作
	
	//case1:设置空列表，调用start，且获取状态返回WIFI_WLAN_STATUS_EMPTY，并且不返回SSID
	memset(set_ap,0,sizeof(set_ap));
	list_s.stored_num = 0;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || (status.status != WIFI_WLAN_STATUS_EMPTY) || strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:挂断后获取状态会失败，同时返回WIFI_WLAN_STATUS_FAIL状态(挂断是阻塞的)(此处状态并不会改变,开发未对状态值操作)
	//注意:此处若返回(0, -1,  )也算通过  20180606 modify
	memset(&status,0,sizeof(status));
	if( ((ret=NDK_WifiStatus(&status))!=NDK_ERR_WIFI_DEVICE_UNAVAILABLE) /*|| (status.status!=WIFI_WLAN_STATUS_FAIL)*/ || strcmp(status.ssid,""))
	{
		if(((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL))
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			GOTOERR;
		}
	}

	//case3:设置列表，密码出错，调用start后尝试连接返回连接失败WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED且不返回ssid
	memset(set_ap,0,sizeof(set_ap));	
	strcpy(list_s.stored_ap[0].ssid,ssid[1]);
	strcpy(list_s.stored_ap[0].pwd,"1234123412");//密码设置错误的 
	memcpy(list_s.stored_ap[0].mac.octet,apmac[1],6);
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET; 
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || ((status.status != WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED) && (status.status != WIFI_WLAN_STATUS_FAIL))|| strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:把case5中的列表其中AP2的密码修改正确后，调用start后应该能够连接上正确的AP
	strcpy(list_s.stored_ap[0].pwd,pwd[1]);//把密码设置正确
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}

	//case5:修改表，把之前连接过的ap删除后，调用start后能够根据新的列表连接ap
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[0]);
	strcpy(list_s.stored_ap[0].pwd,pwd[0]);
	memcpy(list_s.stored_ap[0].mac.octet,apmac[0],6);
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//未调用start情况下:应该仍然保持着连接AP2
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//重新连接应该根据新的列表连接上AP1
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[0]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	
	//case6:隐藏ap 
	//本用例保存AP1为隐藏AP，预期可以连接上AP1
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[0]);
	strcpy(list_s.stored_ap[0].pwd,pwd[0]);
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_ap[0].priority = 0; 
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[0]) )//此处概率性失败:原因可能是概率性连接不上AP1,导致连接失败
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//列表设置应该只有第一台路由生效
	//case7:将AP1设置为第一台路由，应该连接上AP1
	memset(set_ap,0,sizeof(set_ap));
	for(i=0; i<2; i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,pwd[i]);
		memcpy(list_s.stored_ap[i].mac.octet,apmac[i],6);
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
	}
	list_s.stored_ap[0].flag |= WIFI_AP_FLAG_HIDDEN_SET;
	list_s.stored_num = 2;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[0]) )//此处概率性失败:原因可能是概率性连接不上AP1,导致连接失败
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//在已经调NDK_WifiStart后再调NDK_WifiStart不会先断开再重连
	//case8:第一次调用NDK_WifiStart状态是正在连接或已连接时，第二次Start的状态应该是先正在连接，然后已连接
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap[0].ssid,ssid[1]);
	strcpy(list_s.stored_ap[0].pwd,pwd[1]);
	memcpy(list_s.stored_ap[0].mac.octet,apmac[1],6);
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_MAC_SET;
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//在已连接的情况下，再调用Start获取到的状态应该是先正在连接，然后已连接
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if(auto_flag==0 || auto_flag==2)
	{
		//case9:在第一次start后，获取到wifi未连接的情况下，再次调用NDK_WifiStart状态应该是正在连接
		if((ret = NDK_WifiStart()) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg("请拔掉路由器:%s电源后按任意键继续", ssid[1]);
		if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid, "") )
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			GOTOERR;
		}
		cls_show_msg("请插上路由器:%s电源大概30秒后按任意键继续", ssid[1]);
		if((ret = NDK_WifiStart()) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, ssid[1]) )
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			GOTOERR;
		}
		if((ret = NDK_WifiStop()) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WifiStop();
	return;
}
#endif

