/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan1.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiGetNetList,NDK_WiFiInit接口功能
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
#define TESTAPI	"NDK_WiFiGetNetList,NDK_WiFiInit"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan1(void)
* functional description 	: Wifi 获取扫描到的AP的SSID信息 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/
void wlan1(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];

	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_GetWiFiNetList函数应该返回失败
	if ((ret=NDK_WiFiGetNetList(ESSIDlist, &numList)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
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
	
	//case2:测试连续初始化，应该成功
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:初始后异常参数测试：输入输出参数为NULL,应该返回参数错误
	if ((ret=NDK_WiFiGetNetList(NULL, &numList)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiGetNetList(ESSIDlist, NULL)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:正常测试:正常调用应该能够获取到所有的AP信息，并在屏幕上显示
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetList(ESSIDlist, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg1(3,"共扫描到%d个AP,Line=%d,AP[%d]:%s", numList, __LINE__,i, ESSIDlist1[i]);
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

	if(auto_flag==0||auto_flag==2)
	{
		//case5: AP发生变化,scan可以发现这一变化(还需要人工进一步对AP逐个确认)
		cls_show_msg("请关闭某台AP,其不应再被扫描到,关闭15秒左右后按任意键继续...");
		for (j=0; j<3; j++)
		{
			if (NDK_WiFiGetNetList(ESSIDlist, &numList) == NDK_OK)
			{
				if (numList > 0)
				{
					for (i=0; i<numList; i++)
					    cls_show_msg("共扫描到%d个AP,AP[%d]:%s", numList, i, ESSIDlist1[i]);
					break;
				}
			}
			sleep(1);
		}
		if (j == 3)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
		
		if(ENTER!=cls_show_msg("关闭的AP不再被扫描到,通过按[ENTER],否则按[其它]", TESTAPI))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
	}
	else
		send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
	
	send_result("%s测试通过", TESTAPI);

	//测试后置:WIFI设备挂断
ERR:
	NDK_WiFiShutdown();
	return;
}

