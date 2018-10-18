/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi模块
* file name		: wifi4.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WifiGetFunc是否能够正确获取wifi相关信息功能
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
#define TESTAPI	"NDK_WifiGetFunc"	

extern int scan_get_mac(uchar outmac[6]); 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
char * displaywifimode(int modevalue)
{
	/*private & local definition*/
	char *sectypestr[] = {"WIFI_AUTH_OPEN", "WIFI_AUTH_WEP_OPEN", "WIFI_AUTH_WEP_SHARED", "WIFI_AUTH_WPA_TKIP_PSK", "WIFI_AUTH_WPA_AES_PSK", "WIFI_AUTH_WPA_MIXED_PSK", "WIFI_AUTH_WPA2_AES_PSK", "WIFI_AUTH_WPA2_TKIP_PSK", "WIFI_AUTH_WPA2_MIXED_PSK", "WIFI_AUTH_WPS_OPEN", "WIFI_AUTH_WPS_SECURE"};
	char *tmp = "未知的加密模式";
	
	/*process body*/
	if(modevalue<WIFI_AUTH_OPEN||modevalue>WIFI_AUTH_WPS_SECURE)
		return tmp;
	
	tmp = sectypestr[modevalue];
	return tmp;
}

char * displaywififrequency(int frequencyvalue)
{
	/*private & local definition*/
	char *frequencystr[] = {"5G", "2.4G"};
	char *tmp = "未知的频率";
	
	/*process body*/
	if(frequencyvalue<0 || frequencyvalue>1)
		return tmp;
	
	tmp = frequencystr[frequencyvalue];
	return tmp;
}
/****************************************************************
* function name 	 				: void wifi4(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  				jiangym		20160301	created	 
*****************************************************************/
void wifi4(void)
{
	/*private & local definition*/
	int ret = 0, i = 0;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T st_ap;
	list_s.stored_ap = &st_ap;
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	list_scan.list = ap;
	ST_ETHER_ADDR_T mac;
	ST_WIFI_AP_INFO succ_ap;
	ST_WIFI_STATUS status;
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.";
#else
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.";
#endif
	char pwd[] = "1234567890"; //此处可以做修改AP名字及密码和对应的MAC
	//uchar default_mac[6] = {0x78,0x54,0x2e,0x59,0xdd,0x3a};
	uchar ap_mac[6] = {0};
	char pInfo[64] = {0}, mactemp[64] = {0};
	char ssidout[32][33];
	uint ssidoutlen = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_WifiStart();
	if((ret = lib_getwifistatus(&status)) != NDK_OK)
	{
		send_result("line %d:获取wifi状态失败(ret=%d,status=%d)", __LINE__, ret, status.status);
		return;
	}
	//从配置文件中获取ap Mac  无配置文件就通过扫描获取
	if(NDK_FsExist(PARACONF)!=NDK_OK)
	{	
		cls_show_msg1(3,"无参数配置文件,将通过扫描获取mac");
		//通过扫描获取Mac传入定义的Mac数组中
		cls_show_msg1(5,"获取测试AP:%s的Mac",test_ap);
		if((ret = scan_get_mac(ap_mac)) != SUCC)
		{
			send_result("line %d:获取测试AP Mac失败(%d)", __LINE__, ret);
			return;
		}
	}
	else
	{
		if(GetParaInfo("WifiMac3",pInfo)!=SUCC)
		{	
			send_result("line %d:查找Mac信息失败", __LINE__ );
			return;
		}
		if(trim_string(pInfo,mactemp)!=SUCC)
		{	
			send_result("line %d:读取Mac信息失败", __LINE__ );
			return;
		}
		if(trim_char2uchar(mactemp,ap_mac)!=SUCC)
		{	
			send_result("line %d:转换Mac格式失败", __LINE__ );
			return;
		}
	}
	NDK_WifiStop();//刚好也作为保护动作
	
	//case1:在驱动装载之前,调用该函数不应该出现跑飞等异常
	if ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan)) != NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(ap,0,sizeof(ap));
	list_scan.num = 32;//期望值为32
	if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num==0) )
	{
		send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
		GOTOERR;
	}
	
	//测试前置:扫描功能需要在start(wifi驱动开启)后且状态要为稳定(包括空、失败、成功三种状态)时才能返回成功
	NDK_WifiStart();
	if((ret = lib_getwifistatus(&status)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:参数异常测试:WIFI_GET_FUNC_SCAN_RESULT-1,WIFI_GET_FUNC_UNDEFINE+1应该返回参数错误
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT-1,&list_scan)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_UNDEFINE+1,&list_scan)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_UNDEFINE,&list_scan)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_MAC, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:测试获取扫描信息:期望值是0的情况
	cls_printf("测试扫描AP信息...");
	memset(ap,0,sizeof(ap));
	list_scan.num = 0;//期望值为0
	if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num!=0) )
	{
		send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
		GOTOERR;
	}
	
	//case4:测试获取扫描信息:有多个ap(包括特殊字符或者中文字符的ap)时返回结果,希望获取3个小于底层扫描到的实际个数时应返回3个
	memset(ap,0,sizeof(ap));
	list_scan.num = 3;//期望值为3
	if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num!=3) )
	{
		send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
		GOTOERR;
	}
	
	//case5:测试获取扫描信息:期望值128>实际获取的数量10,则应该返回实际或取到的个数,信息包括: num ssid auth_mode frequency rssi channel mac
	memset(ap,0,sizeof(ap));
	list_scan.num =32/*128*/;//希望数量设置成足够大,测试环境也要尽量构造足够多的AP进行扫描,测试负载
	if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num==0) )
	{
		send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
		GOTOERR;
	}
	//预期的结果数量不确定,将结果逐个显示,并提示测试人员分析显示的结果是否正确:结果显示的异常包括担不限于SSID乱码,SSID错误,认证模式错误,信号不在合理范围内等
	if (list_scan.num > 0) 
	{	
		memset(ssidout,0,sizeof(ssidout));
		send_result("共扫描到%d个AP的信息如下(请检查获取的AP信息是否正确):", list_scan.num); 
		for (i=0; i<list_scan.num; i++)
		{
			if(!strcmp(ap[i].ssid,""))//如果是隐藏ap  直接传给输出buf
				strcpy(ssidout[i],ap[i].ssid);
			else
			{
				if((ret=NDK_Utf8ToGbk((uchar *)ap[i].ssid,strlen(ap[i].ssid),(uchar *)ssidout[i],&ssidoutlen))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					GOTOERR;
				}
			}
			send_result("AP[%d]:%s,模式:%s,信道:%d,频率:%s,信号:%d,mac:%02x:%02x:%02x:%02x:%02x:%02x", i, ssidout[i], displaywifimode(ap[i].auth_mode),
			ap[i].channel, displaywififrequency(ap[i].frequency), ap[i].rssi, ap[i].mac.octet[0], ap[i].mac.octet[1], ap[i].mac.octet[2], ap[i].mac.octet[3], ap[i].mac.octet[4], ap[i].mac.octet[5]);
		}
	}
	if(auto_flag==0||auto_flag==2)
	{
		if(cls_show_msg("请检查显示的AP信息是否正常,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}

	//case6:隐藏ssid的AP不应该被扫描到
	if(auto_flag==0||auto_flag==2)
	{
		cls_show_msg("请隐藏某台AP,其不应再被扫描到,隐藏15秒左右后按任意键继续...");
		memset(ap,0,sizeof(ap));
		if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num==0) )
		{
			send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
			GOTOERR;
		}
		if (list_scan.num > 0)//预期的结果数量不确定,将结果逐个显示,并提示测试人员检查扫描到的AP中应该不再有被隐藏的SSID
		{
			memset(ssidout,0,sizeof(ssidout));
			send_result("隐藏某台AP后共扫描到%d个AP的信息如下:", list_scan.num);
			for (i=0; i<list_scan.num; i++)
			{
				if(!strcmp(ap[i].ssid,""))//如果是隐藏ap  直接传给输出buf
					strcpy(ssidout[i],ap[i].ssid);
				else
				{
					if((ret=NDK_Utf8ToGbk((uchar *)ap[i].ssid,strlen(ap[i].ssid),(uchar *)ssidout[i],&ssidoutlen))!=NDK_OK)
					{
						send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
						GOTOERR;
					}
				}
				send_result("AP[%d]:%s,mac:%02x:%02x:%02x:%02x:%02x:%02x", i, ssidout[i], ap[i].mac.octet[0], ap[i].mac.octet[1], ap[i].mac.octet[2], ap[i].mac.octet[3], ap[i].mac.octet[4], ap[i].mac.octet[5]);
			}
		}
		if(cls_show_msg("请检查之前隐藏的AP是否不再被扫描到,是按[ENTER],否按[其它]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}


	//case7:关闭的AP不应该再被扫描到
	if(auto_flag==0||auto_flag==2)
	{
		cls_show_msg("请关闭某台AP,其不应再被扫描到,关闭15秒左右后按任意键继续...");
		memset(ap,0,sizeof(ap));
		if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num==0) )
		{
			send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
			GOTOERR;
		}
		if (list_scan.num > 0)//预期的结果数量不确定,将结果逐个显示,并提示测试人员检查扫描到的AP中应该不再有被关闭的SSID
		{
			memset(ssidout,0,sizeof(ssidout));
			send_result("关闭某台AP后共扫描到%d个AP的信息如下:", list_scan.num);
			for (i=0; i<list_scan.num; i++)
			{
				if(!strcmp(ap[i].ssid,""))//如果是隐藏ap  直接传给输出buf
					strcpy(ssidout[i],ap[i].ssid);
				else
				{
					if((ret=NDK_Utf8ToGbk((uchar *)ap[i].ssid,strlen(ap[i].ssid),(uchar *)ssidout[i],&ssidoutlen))!=NDK_OK)
					{
						send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
						GOTOERR;
					}
				}
				send_result("AP[%d]:%s,mac:%02x:%02x:%02x:%02x:%02x:%02x", i, ssidout[i], ap[i].mac.octet[0], ap[i].mac.octet[1], ap[i].mac.octet[2], ap[i].mac.octet[3], ap[i].mac.octet[4], ap[i].mac.octet[5]);
			}	
		}
		if(cls_show_msg("请检查之前关闭的AP是否不再被扫描到,是按[ENTER],否按[其它]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	
	//case8:返回芯片mac,判断是否正确
	cls_printf("验证获取WIFI的MAC信息...");
	if((ret = NDK_WifiGetFunc(WIFI_GET_FUNC_MAC,&mac)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	send_result("获取到wifi的mac为:%02x:%02x:%02x:%02x:%02x:%02x,请验证MAC地址是否正确",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]);
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,status=%d)", __LINE__, TESTAPI, ret, status.status);
		RETURN;
	}
	
	//case9:设置一个ap,密码出错,那么应该连接不上,获取已连接的AP信息函数应该返回失败
	memset(&st_ap,0,sizeof(st_ap));
	strcpy(list_s.stored_ap->ssid,test_ap); 
	strcpy(list_s.stored_ap->pwd,"0000000000");
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET ;
	list_s.stored_ap->priority = 0;
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
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || ((status.status != WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED) && (status.status != WIFI_WLAN_STATUS_FAIL)))
	{
		send_result("line %d: %s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status.status);
		GOTOERR;
	}
	memset(&succ_ap,0,sizeof(succ_ap));
	if( (ret=NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&succ_ap))==NDK_OK )
	{
		send_result("line %d: %s测试失败(ret=%d),连接上错误的AP:%s", __LINE__, TESTAPI, ret, succ_ap.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case10:测试返回已连接ap信息:保存一条正确的AP信息,进行成功连接后会返回正确的ap信息
	cls_printf("验证获取已连接的ap信息...");
	memset(&st_ap,0,sizeof(st_ap));
	strcpy(list_s.stored_ap->ssid,test_ap); 
	strcpy(list_s.stored_ap->pwd,pwd);
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET ;
	list_s.stored_ap->priority = 0;
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
	if( ((ret = lib_getwifistatus(&status)) != NDK_OK) || (status.status != WIFI_WLAN_STATUS_SUCC) )
	{
		send_result("line %d: %s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status.status);
		GOTOERR;
	}
	memset(&succ_ap,0,sizeof(succ_ap));
	if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&succ_ap))!=NDK_OK) || strcmp(succ_ap.ssid,test_ap) ||  memcmp(succ_ap.mac.octet, ap_mac, sizeof(ap_mac)) )
	{
		send_result("line %d: %s测试失败(ret=%d),连接上错误的AP:%s,MAC:%02x:%02x:%02x:%02x:%02x:%02x", __LINE__, TESTAPI, ret, succ_ap.ssid, succ_ap.mac.octet[0], succ_ap.mac.octet[1], succ_ap.mac.octet[2], succ_ap.mac.octet[3], succ_ap.mac.octet[4], succ_ap.mac.octet[5]);
		goto ERR;
	}
	send_result("请检查已连接的AP的模式:%s,信道:%d,频率:%s,信号:%d是否正确", displaywifimode(succ_ap.auth_mode), succ_ap.channel, displaywififrequency(succ_ap.frequency), succ_ap.rssi);

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WifiStop();
	return;
}

