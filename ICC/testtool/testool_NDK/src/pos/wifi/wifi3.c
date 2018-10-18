/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi模块
* file name		: wifi3.c
* Author 		:  jiangym
* version		: 
* DATE			:  20160226
* directory 		: 
* description		: 测试NDK_WifiStatus是否能正确获取底层正在操作的AP名称和连接状态
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
#define TESTAPI	"NDK_WifiStatus"	

/*------------global variables definition-----------------------*/
#define MAXTIMEOUT 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wifi3(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			  jiangym	   20160226		created
*****************************************************************/
void wifi3(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, ret1 = 0;
	time_t oldtime;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T set_ap[32];
	list_s.stored_ap = set_ap;
	ST_WIFI_STATUS status;
	char *ssid[] = {
					"AP1@#fartest",
					"AP2&4same",
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
					"`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.",  //路由器设置为:`~!"@$#[^%]*() -_=+|\&{}:;?/><.
#else
					"`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.",
#endif
					};

	/*process body*/
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);
	//提供三个ap ssid与密码设置好 再测试
	
	//测试前置
	NDK_WifiStop();//避免因开机自动启动wifi引起的问题
	
	//case1:参数异常测试NULL
	if((ret = NDK_WifiStatus(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:wifi未连接去获取状态,应该返回失败
#if 0 //与陈镇江确认过，这边只要返回失败或获取到失败状态，都算通过  20180605 sull modify
	if((ret = NDK_WifiStatus(&status)) == NDK_OK/*!= NDK_ERR*/)//-5
//#else
	//if( (((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!= WIFI_WLAN_STATUS_FAIL)) && ((ret1 = NDK_WifiStatus(&status)) == NDK_OK))
#endif
	if((ret = NDK_WifiStatus(&status)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
	{
		if(((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL))
		{
			send_result("line %d: %s测试失败(%d,%d,status=%d)", __LINE__, TESTAPI, ret, ret1, status.status);
			GOTOERR;
		}
	}

	//case3:设置列表，1个ap,其密码出错,调用start后尝试连接返回连接失败WIFI_WLAN_STATUS_FAIL
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap->ssid,ssid[1]);
	strcpy(list_s.stored_ap->pwd,"9876543210");
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET;
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
	oldtime=time(NULL);
	while(1)
	{
		if((ret = NDK_WifiStatus(&status)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status.status);
			goto ERR;
		}
		//与陈镇江确认，模块本身无法准确识别密码错误的时候只能返回连接失败，故获取到FAIL状态也算通过
		if(status.status == WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED || status.status == WIFI_WLAN_STATUS_FAIL)
			break;
		if((time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d: %s测试失败,未获取到正确的状态,超时退出(status=%d)", __LINE__, TESTAPI, status.status);
			goto ERR;
		}
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:设置空列表,获取状态会返回WIFI_WLAN_STATUS_EMPTY
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
	if( ((ret=NDK_WifiStatus(&status))!=NDK_OK) || (status.status!= WIFI_WLAN_STATUS_EMPTY) )
	{
		send_result("line %d: %s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status.status);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//返回WIFI_WLAN_STATUS_INIT 不好构造,此状态不好捕捉到,除非是在设置了开启自动连接后,设置空列表后调用获取状态能够获取到
	
	//case5:列表:3个ap的密码是错误的那么在连接过程中会返回正在连接的状态WIFI_WLAN_STATUS_LINKING
	memset(set_ap,0,sizeof(set_ap));
	for(i=0;i<3;i++)
	{
		strcpy(list_s.stored_ap[i].ssid,ssid[i]);
		strcpy(list_s.stored_ap[i].pwd,"1234123412");
		list_s.stored_ap[i].flag = WIFI_AP_FLAG_DHCP_SET;
		list_s.stored_ap[i].priority = 0;
	}
	list_s.stored_ap[0].flag = WIFI_AP_FLAG_DHCP_SET | WIFI_AP_FLAG_HIDDEN_SET;
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
	oldtime=time(NULL);
	while(1)
	{
		if((ret = NDK_WifiStatus(&status)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(status.status == WIFI_WLAN_STATUS_LINKING)
			break;
		if((time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d: %s测试失败,未获取到正确的状态,超时退出(status=%d)", __LINE__, TESTAPI, status.status);
			goto ERR;
		}
	}
#if !WIFI_PRIORITY_ENABLE
	if( (ret=lib_getwifistatus(&status))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:列表:设置正确的ap,调用start后会返回成功的状态,ssid为预期的apWIFI_WLAN_STATUS_SUCC
	memset(set_ap,0,sizeof(set_ap));
	strcpy(list_s.stored_ap->ssid,ssid[2]);
	strcpy(list_s.stored_ap->pwd,"1234567890");
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET;
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
	oldtime=time(NULL);
	while(1)
	{
		if((ret = NDK_WifiStatus(&status)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(status.status == WIFI_WLAN_STATUS_SUCC)
			break;
		if((time(NULL)-oldtime)>MAXTIMEOUT)
		{
			send_result("line %d: %s测试失败,未获取到正确的状态,超时退出(status=%d)", __LINE__, TESTAPI, status.status);
			goto ERR;
		}
	}
	if(status.status!= WIFI_WLAN_STATUS_SUCC || strcmp(status.ssid,ssid[2]))
	{
		send_result("line %d: %s测试失败(status=%d,ssid:%s)", __LINE__, TESTAPI, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:调用stop后获取状态 应该返回NDK_ERR 所有WiFi模块返回值改为要返回-3004  
	//挂断后获取status预期是要返回-3004,若获取状态返回ok那也要确保获取的状态值为-1 20180622 modify
	if( ((ret=NDK_WifiStatus(&status))!=NDK_ERR_WIFI_DEVICE_UNAVAILABLE) )
	{
		if(((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL))
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			goto ERR;
		}
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WifiStop();
	return;
}

