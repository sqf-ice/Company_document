/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan2.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiSignalCover接口功能
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
#define TESTAPI	"NDK_WiFiSignalCover"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan2(void)
* functional description 	: Wifi 获取信号值 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/

void wlan2(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	int signal = 0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];

	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_WiFiSignalCover函数应该返回失败
	if ((ret=NDK_WiFiSignalCover("XXX", &signal)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
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

	//扫描AP
	for (j=0; j<3; j++) 
	{
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList)) == NDK_OK) 
		{
			if (numList > 0) 
			{
				for (i=0; i<numList; i++)
					cls_show_msg("共扫描到%d个AP,AP[%d]=%s", numList, i, ESSIDlist1[i]);
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
	if ((ret=NDK_WiFiSignalCover(NULL, &signal)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiSignalCover(ESSIDlist1[0], NULL)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	
	//case4:正常测试:AP的名字不正确时,获取信号应该失败
	if ((ret=NDK_WiFiSignalCover("XXX", &signal)) != NDK_ERR_WIFI_DEVICE_NOTOPEN)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:正常测试:正常调用应该能够获取到扫描到的AP的信号，并在屏幕上显示
	for(i=0;i<numList;i++)
	{
		//本用例有可能失败：因为如果之前扫描到的AP中某个AP信号不强，获取信号时会获取不到
		if ((ret=NDK_WiFiSignalCover(ESSIDlist1[i], &signal)) != NDK_OK)
		{
			send_result("line %d:%s测试失败AP[%d]=%s,ret=%d", __LINE__, TESTAPI, i, ESSIDlist1[i], ret);
			GOTOERR;
		}
		if(cls_show_msg1(30, "AP[%d]:%s的信号为:%d,确认键通过,其它失败", i, ESSIDlist1[i], signal)!=ENTER)
		{
			send_result("line %d:%s测试失败AP[%d]=%s,signal=%d", __LINE__, TESTAPI, i, ESSIDlist1[i], signal);
			GOTOERR;
		}
	}

	send_result("%s测试通过", TESTAPI);
	goto ERR;
ERR:
	//测试后置:WIFI设备挂断
	NDK_WiFiShutdown();
	return;
}


