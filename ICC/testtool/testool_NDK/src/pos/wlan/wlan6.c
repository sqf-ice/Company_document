/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线Wifi 模块
* file name		: wlan6.c
* Author 		: chensj
* version		: 
* DATE			: 20130513
* directory 		: 
* description		: 测试NDK_WiFiGetNetInfo接口功能
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
#define TESTAPI	"NDK_WiFiGetNetInfo"	
#define APNUM 5
#define APNUM1 64

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlan6(void)
* functional description 	: 获取扫描到的AP的全部信息
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  					chensj   201305013	created
*****************************************************************/
void wlan6(void)
{

	/*private & local definition*/
	int ret = 0, i = 0, j = 0, numList = 0;
	ST_WIFI_APINFO ESSIDlist[64];
	
	memset(ESSIDlist,0,sizeof(ESSIDlist));
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:设备未初始化调用SDK_GetWiFiNetList函数应该返回失败
	if ((ret=NDK_WiFiGetNetInfo(ESSIDlist,APNUM,&numList)) != NDK_ERR_WIFI_DEVICE_UNAVAILABLE)
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
	if ((ret=NDK_WiFiGetNetInfo(NULL,APNUM, &numList)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiGetNetInfo(ESSIDlist,APNUM, NULL)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if ((ret=NDK_WiFiGetNetInfo(ESSIDlist,0, &numList)) != NDK_ERR_WIFI_INVDATA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:正常测试:正常调用应该能够获取到所有的AP信息，并在屏幕上显示
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist,APNUM, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("Line=%d\nAP[%d]->Essid:%s\nsKeyModeStr:%s\nnFrequency:%d\nsSignal:%s", __LINE__,i, ESSIDlist[i].sEssid,ESSIDlist[i].sKeyModeStr,ESSIDlist[i].nFrequency,ESSIDlist[i].sSignal);
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
	//当APNUM小于获取到的numList时,按照APNUM个数输出,这里增加一个判断 20141028 add jiangym
	if(cls_show_msg("显示的AP数量是否为5个,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//case3.1 设定nMaxNum 大于扫描到的AP时候,同时把 所希望的最大数量AP都显示出来 
	memset(ESSIDlist,0,sizeof(ESSIDlist));
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist,APNUM1, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("Line=%d\nAP[%d]->Essid:%s\nsKeyModeStr:%s\nnFrequency:%d\nsSignal:%s", __LINE__,i, ESSIDlist[i].sEssid,ESSIDlist[i].sKeyModeStr,ESSIDlist[i].nFrequency,ESSIDlist[i].sSignal);
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
	//当APNUM大于获取到的numList时,按照numList个数(将会是扫描到的所有的AP个数)输出,这里增加一个判断 20141028 add jiangym
	if(cls_show_msg("显示的AP数量是否为所有能扫描到的AP个数,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	

	//case3: AP发生变化,scan可以发现这一变化(还需要人工进一步对AP逐个确认)
	cls_show_msg("请关闭某台AP,其不应再被扫描到,关闭15秒左右后按任意键继续...");
	memset(ESSIDlist,0,sizeof(ESSIDlist));
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist,APNUM1, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("AP[%d]:%s", i, ESSIDlist[i].sEssid);
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
		send_result("%s测试失败", TESTAPI);
	else
		send_result("%s测试通过", TESTAPI);

	//测试后置:WIFI设备挂断
ERR:
	NDK_WiFiShutdown();

	return;
}

