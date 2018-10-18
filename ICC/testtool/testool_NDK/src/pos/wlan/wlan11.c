/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WIFIAP模块
* file name			: wifiap1
* Author 			: jiangym
* version			: 
* DATE				: 20141209
* directory 			: 
* description			: 测试NDK_WiFiAPOpen和NDK_WiFiAPClose是否能正常开启关闭wifi热点功能
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"NDK_WiFiAPOpen,NDK_WiFiAPClose"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: wifiap1
* functional description 	:测试NDK_WiFiAPOpen和NDK_WiFiAPClose
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	       jiangym			20141209		created
*****************************************************************/
void wlan11()
{
	/*private & local definition*/
	int ret = -1, i = 0;
	ST_WIFIAP_PARAM apparam = {0};
	char *secmodestr[] = {"NONE","WEP_OPEN","WEP_SHARED","WPA","WPA2"};
	char *errkey[] = 
	{
		"1234567",//各种加密模式长度不正确的密码
		"wifi12mima",//WIFI_NET_SEC_WEP_OPEN:10个十六进制字符（0-9和A-F）:非合理字符密码
		"wifi12mima1234567890wifi12",//WIFI_NET_SEC_WEP_ SHARED: 26个十六进制字符（0-9和A-F）:非合理字符密码
		"01234567890123456789012345678901234567890123456789012345678901234",//WIFI_NET_SEC_WPA：65个任意字符，长度非法
		"01234567890123456789012345678901234567890123456789012345678901234",//WIFI_NET_SEC_WPA2：65个任意字符，长度非法		
	};
    EM_WIFI_NET_SEC secmode[6] = 
	{
		WIFI_NET_SEC_WEP_OPEN,
		WIFI_NET_SEC_WEP_OPEN,
		WIFI_NET_SEC_WEP_SHARED,
		WIFI_NET_SEC_WEP_SHARED,
		WIFI_NET_SEC_WPA,
		WIFI_NET_SEC_NONE,
	};
	char *normalkey[] = 
	{
		"12345",//WIFI_NET_SEC_WEP_OPEN模式设置5位ASCII密码
		"12345678ab",//WIFI_NET_SEC_WEP_OPEN模式设置10位16进制密码
		"0123456abc#%*",//WIFI_NET_SEC_WEP_SHARED模式设置13位ASCII密码
		"0123456789abcdef0123456789",//WIFI_NET_SEC_WEP_SHARED模式设置26位16进制密码
		"12345678ab",//WIFI_NET_SEC_WPA模式设置10位16进制密码
		"12ab34#$%56wifi12mima01234567890wifi12",//WIFI_NET_SEC_NONE模式,不判断密码长度和格式
	};
	char *ssid[] = 
	{
		"wifi热点",
		"`~!\"@$#[^%]*() -",
		"_=+|\\&{}:;?,/><.",
	};
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置:设置共享网络
	if(conf_conn_AP()!=SUCC)
		return;
	//使用系统默认的AP参数进行初始化
	memset(&apparam, 0,sizeof(apparam));
	apparam.emShareDev = WifiApShareDev;
	apparam.emSecMode = WifiApSecMode;
	strcpy(apparam.szAPIP, netsetting.WifiApIp.Str);
	strcpy(apparam.szKey,WifiApKey);
	strcpy(apparam.szSsid , WifiApSsid);
	apparam.uchidden = WifiApHidden;

	if(cls_show_msg("是否为开机第一次运行本用例:是[ENTER],否[其它]")==ENTER)
	{
		//case1:共享网络未连接打开情况下open失败，该用例只能开机第一次测试，后续网络设备打开后会一直是打开状态
		if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_SHAREDEV_NOOPENED)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}

	//连接网络
	NetUp();
	
	//case2:异常参数测试
	//case2.1:接入的网络不在枚举值范围内
	apparam.emShareDev = WIFIAP_SHARE_DEV_ETH-1;
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_NOSHAREDEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	apparam.emShareDev = WIFIAP_SHARE_DEV_PPP+1;
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_NOSHAREDEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	apparam.emShareDev = WifiApShareDev;
	
	//case2.2:加密方式不在枚举值范围内
	apparam.emSecMode = WIFI_NET_SEC_NONE-1;
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_SECMODE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	apparam.emSecMode = WIFI_NET_SEC_WPA2+1;
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_SECMODE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	apparam.emSecMode = WifiApSecMode;
	
	//case2.3:设置的wifi热点名称为空
	memset(apparam.szSsid,0,sizeof(apparam.szSsid));
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_NOSSID)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	strcpy(apparam.szSsid , WifiApSsid);
	
	//case2.4:设置的wifi热点名称长度超过32位
	strcpy(apparam.szSsid,"aaaaaaaaaabbbbbbbbbbccccccccccddd");//33位
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_SSIDLEN)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}	
	strcpy(apparam.szSsid , WifiApSsid);

	//case2.5:测试各类加密模式密码长度不正确或字符不符合要求情况下，打开AP应该失败
	//WIFI_NET_SEC_WEP_OPEN: 5个ASCII字符或者10个十六进制字符（0-9和A-F）（密钥长度为10时，模块会自动认为是十六进制字符）
	//WIFI_NET_SEC_WEP_ SHARED: 13个ASCII字符或者26个十六进制字符（0-9和A-F）（密钥长度为26时，模块会自动认为是十六进制字符）
	//WIFI_NET_SEC_WPA或者WIFI_NET_SEC_WPA2：可以输入8-64个任意字符。
	for(i=WIFI_NET_SEC_WEP_OPEN;i<=WIFI_NET_SEC_WPA2;i++)
	{
		apparam.emSecMode = i;
		memset(apparam.szKey,0,sizeof(apparam.szKey));
		strcpy(apparam.szKey,errkey[0]);
		if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_KEYLEN)
		{
			send_result("line %d:%s测试失败(i=%d)(%d)", __LINE__, TESTAPI, i, ret);
			GOTOERR;
		}
		memset(apparam.szKey,0,sizeof(apparam.szKey));
		strcpy(apparam.szKey,errkey[i]);
		if(i==1||i==2)
		{
			if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_KEYTYPE)
			{
				send_result("line %d:%s测试失败(i=%d)(%d)", __LINE__, TESTAPI, i, ret);
				GOTOERR;
			}
		}
		else
		{
			if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_KEYLEN)
			{
				send_result("line %d:%s测试失败(i=%d)(%d)", __LINE__, TESTAPI, i, ret);
				GOTOERR;
			}
		}
	}
	apparam.emSecMode = WifiApSecMode;
	strcpy(apparam.szKey,WifiApKey);

	//case2.6:设置错误的wifi热点IP
	strcpy(apparam.szAPIP,"192.168.1");
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_IPTYPE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}
	strcpy(apparam.szAPIP, netsetting.WifiApIp.Str);

	//case3:如果开启的是以太网，参数设置为PPP应该返回失败;如果开启的是PPP,设置以太网应该失败
	if(WifiApShareDev==WIFIAP_SHARE_DEV_ETH)
		apparam.emShareDev = WIFIAP_SHARE_DEV_PPP;	
	else
		apparam.emShareDev = WIFIAP_SHARE_DEV_ETH;	
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_ERR_WIFIAP_SHAREDEV_NOOPENED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	apparam.emShareDev = WifiApShareDev;

	//case4:正常测试:设置不同的加密模式进行测试各种合理密码情况下的测试，预期应该返回成功
	//case4.1:WIFI_NET_SEC_WEP_OPEN模式设置5位ASCII密码
	//case4.2:WIFI_NET_SEC_WEP_OPEN模式设置10位16进制密码
	//case4.3:WIFI_NET_SEC_WEP_SHARED模式设置13位ASCII密码
	//case4.4:WIFI_NET_SEC_WEP_SHARED模式设置26位16进制密码
	//case4.5:WIFI_NET_SEC_WPA模式设置10位16进制密码
	//case4.6:WIFI_NET_SEC_NONE模式,不判断密码长度和格式
	memset(&apparam, 0,sizeof(apparam));
	apparam.emShareDev = WifiApShareDev;
	strcpy(apparam.szAPIP, netsetting.WifiApIp.Str);
	strcpy(apparam.szSsid , WifiApSsid);
	apparam.uchidden = WifiApHidden;
	
	for(i=0;i<SZ_ARRAY(normalkey);i++)
	{
		apparam.emSecMode = secmode[i];
		memset(apparam.szKey,0,sizeof(apparam.szKey));
		strcpy(apparam.szKey,normalkey[i]);
		//cls_show_msg("SSID:%s\n加密模式:%s\n密码:%s\n",apparam.szSsid,secmodestr[apparam.emSecMode],apparam.szKey);
		if((ret=NDK_WiFiAPOpen(apparam))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)(i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		if(apparam.emSecMode==WIFI_NET_SEC_NONE)
			cls_show_msg("使用手机搜索连接热点:%s,加密模式:%s,密码:任意输入",apparam.szSsid,secmodestr[apparam.emSecMode]);
		else
			cls_show_msg("使用手机搜索连接热点:%s,加密模式:%s,密码:%s",apparam.szSsid,secmodestr[apparam.emSecMode],apparam.szKey);
		//判断是否能够扫描连接上
		if(cls_show_msg("手机应该能够成功连接热点并上网,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败(i=%d)", __LINE__, TESTAPI, i);
			GOTOERR;
		}
	}
	apparam.emSecMode = WifiApSecMode;
	strcpy(apparam.szKey,WifiApKey);

	//case5:关闭ap后扫描不到,也连接不上
	if((ret=NDK_WiFiAPClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("使用手机搜索热点:%s,应该搜索不到",apparam.szSsid);
	//判断是否能够扫描连接上
	if(cls_show_msg("手机搜索不到热点:%s,正确[Enter],错误[其他]",apparam.szSsid)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	

	//case6:关闭后再调用关闭 
	if((ret=NDK_WiFiAPClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:设置隐藏的wifi热点使用手机应搜索不到，但是能够添加网络成功
	apparam.uchidden = 1;
	if((ret=NDK_WiFiAPOpen(apparam))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//判断是否能够搜索到
	if(cls_show_msg("使用手机搜索wifi热点:%s,应该搜索不到,正确[Enter],错误[其他]",apparam.szSsid)!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("使用手机添加wifi热点:%s,加密模式:%s,密码:%s",apparam.szSsid,secmodestr[apparam.emSecMode],apparam.szKey);
	//判断是否能够添加成功
	if(cls_show_msg("使用手机能够成功连接热点并上网,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}		
	apparam.uchidden = WifiApHidden;

	//case8:wifi热点的名称设置为中文名,应该能成功
	//case9:wifi热点的名称设置为特殊字符,应该能成功
	for(i = 0;i<SZ_ARRAY(ssid);i++)
	{
		strcpy(apparam.szSsid,ssid[i]);
		if((ret=NDK_WiFiAPOpen(apparam))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg("使用手机搜索连接热点:%s,加密模式:%s,密码:%s",apparam.szSsid,secmodestr[apparam.emSecMode],apparam.szKey);
		//判断是否能够扫描连接上
		if(cls_show_msg("手机应该能够成功连接热点并上网,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败(i=%d)", __LINE__, TESTAPI, i);
			GOTOERR;
		}
	}
	strcpy(apparam.szSsid , WifiApSsid);

	send_result("%s测试通过", TESTAPI);
ERR:
	//恢复全局变量
	NDK_WiFiAPClose();
	NetDown();
	return;
}

