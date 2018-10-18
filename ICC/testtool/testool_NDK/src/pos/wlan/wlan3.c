/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan3.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiGetSec接口功能
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
#define TESTAPI	"NDK_WiFiGetSec"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan3(void)
* functional description 	: 获取指定AP的安全模式 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/
void wlan3(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	EM_WIFI_NET_SEC sec;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];

	char *sectypestr[] = {"WIFI_NET_SEC_NONE","WIFI_NET_SEC_WEP_OPEN","WIFI_NET_SEC_WEP_SHARED","WIFI_NET_SEC_WPA","WIFI_NET_SEC_WPA2"};
	EM_WIFI_NET_SEC secresult[5]={WIFI_NET_SEC_NONE, WIFI_NET_SEC_WEP_OPEN, WIFI_NET_SEC_WEP_OPEN, WIFI_NET_SEC_WPA, WIFI_NET_SEC_WPA2}; //	WIFI_NET_SEC_WEP_OPEN 跟WIFI_NET_SEC_WEP_SHARED模式不区别	
	
	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_GetWiFiSec函数应该返回失败
	if ((ret=NDK_WiFiGetSec("XXX", &sec)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试前置:WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//扫描AP,连续扫描3轮
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg("共扫描到%d个AP,Line=%d,AP[%d]=%s", numList, __LINE__,i, ESSIDlist1[i]);
				break;
			} 
		}
		sleep(1);
	}
	if (j == 3) 
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case2:异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiGetSec(WlanEssid, NULL)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiGetSec(NULL, &sec)) != NDK_ERR_WIFI_INVDATA )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:测试能够识别所有的加密模式类型
	for(i=0;i<SZ_ARRAY(sectypestr);i++)
	{
		if (cls_show_msg("请将%s的加密模式修改为%s后按[确认]测试,[其它]退出", WlanEssid,sectypestr[i]) == ENTER)
		{
			if ((ret=NDK_WiFiGetNetList(ESSIDlist, &numList)) != NDK_OK) //底层修改NDK_WiFiGetSec时不再自动扫描,所以这里需要增加扫描AP的动作
			{
				send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if ((ret=NDK_WiFiGetSec(WlanEssid, &sec)) != NDK_OK)//这里的testwifi必须是测试用WIFI的名字
			{
				send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if(sec!=secresult[i])//验证获取到的加密模式跟设置的一致
			{
				send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, sec);
				GOTOERR;
			}
		}
	}

	//case5:正常测试:AP的名字不正确时,获取加密模式应该失败
	if ((ret=NDK_WiFiGetSec("XXX", &sec)) != NDK_ERR_WIFI_DEVICE_NOTOPEN )
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:正常测试:正常调用应该能够获取到扫描到的AP的加密模式,并在屏幕上显示
	for(i=0;i<numList;i++)
	{	
		//本用例有可能失败：因为如果之前扫描到的AP中某个AP信号不强，获取加密模式时会获取不到
		if ((ret=NDK_WiFiGetSec(ESSIDlist1[i], &sec)) != NDK_OK)
		{
			send_result("line %d:%s测试失败AP[%d]:%s,ret=%d", __LINE__, TESTAPI, i, ESSIDlist1[i], ret);
			GOTOERR;
		}
		if(cls_show_msg1(30, "Line=%d,AP[%d]=%s的加密模式为:%s,sec=%d,确认键通过,其它失败",__LINE__, i, ESSIDlist1[i], sectypestr[sec],sec)!=ENTER)
		{
			send_result("line %d:%s测试失败AP[%d]:%s,sec=%d", __LINE__, TESTAPI, i, ESSIDlist1[i], sec);
			GOTOERR;
		}
	}

	send_result("%s测试通过,请将路由器参数修改成POS设置的参数后继续其它测试", TESTAPI);
	goto ERR;
ERR:
	//测试后置:WIFI设备挂断
	NDK_WiFiShutdown();
	return;
}


