/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan7.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiGetMac接口功能
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
#define TESTAPI	"NDK_WiFiGetMac"	
#define APNUM 5

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan7(void)
* functional description 	: 获取Mac 地址
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/
void wlan7(void)
{

	/*private & local definition*/
	int ret = 0,j = 0;
	char szMac[64]={0};	

	/*process body*/
	if(auto_flag==2)
		return;
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_GetWiFiNetList函数应该返回失败
	if ((ret=NDK_WiFiGetMac(szMac)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE) //NDK_ERR_WIFI_DEVICE_UNAVAILABLE
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试前置:WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:初始后异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiGetMac(NULL)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:正常测试:获取mac值
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetMac(szMac) == NDK_OK)
		{
			send_result("Line=%d,szMac=%s", __LINE__,szMac);
			break;		
		}
		sleep(1);
	}
	if (j == 3)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	send_result("%s测试成功", TESTAPI);

	//测试后置:WIFI设备挂断
ERR:
	NDK_WiFiShutdown();
	return;
}

