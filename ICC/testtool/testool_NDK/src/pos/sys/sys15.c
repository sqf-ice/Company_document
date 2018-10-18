/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys15.c
* Author 			: linw
* version			: 
* DATE				: 20130109
* directory 		: 
* description		: 本例测试NDK_LedStatus接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !(defined ME15CHX||defined ME15B||defined ME66)
#define TESTAPI	"NDK_LedStatus"	
#else
#define TESTAPI	"NDK_LedOpen,NDK_LedOn,NDK_LedOff,NDK_LedClose"	
#endif
#define	MAXWAITTIME 30
#define WUCHASEC  1 //这个值待定

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	      linw          20130109	created
*****************************************************************/
#if !(defined ME15CHX||defined ME15B||defined ME66)
void sys15(void)
{
	//以下为局部变量
	int ret = -1;
	unsigned int unLen=0;
	char szBuf[128]={0};
	float setledtime = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	NDK_SysGetPosInfo (SYS_HWINFO_GET_HARDWARE_INFO, &unLen, szBuf);
	if (szBuf[12] != 0x01)//szBuf[12]:是否有射频LED灯 0xFF :否 0x01 :是 
	{
		//如果没有LED灯会通过开启状态栏来模拟LED灯20171011 modify 根据开发说明
#if STABAR_ENABLE		
		if(cls_show_msg("是否进行使用状态栏LCD模拟LED灯测试,是按[Enter],否按[其他]")==ENTER)
		{
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_LED|STATUSBAR_POSITION_TOP))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			cls_show_msg("接下来的测试请查看状态栏上模拟显示的LED灯来进行判断,任意键继续");
		}
		else
		{
			send_result("%s,请确认是否可以使用状态栏LCD进行模拟LED测试",TESTAPI);
			return;
		}
#else	
#if 0  //有反馈SP610无led灯返回值不同,故直接改成无LED灯就直接提示无LED灯无需再判断 20180320 modify
		lib_StartStopwatch();
		if(((ret=NDK_LedStatus(LED_RFID_RED_ON | LED_RFID_YELLOW_ON | LED_RFID_GREEN_ON | LED_RFID_BLUE_ON | LED_COM_ON | LED_ONL_ON)) != NDK_ERR_NO_DEVICES)||((setledtime=lib_ReadStopwatch())>WUCHASEC))
		{
			send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, setledtime);
			GOTOERR;
		}
#endif
		send_result("%s测试通过(本设备无LED灯设备)", TESTAPI);
		return;
#endif
	}
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	
	//case10:异常测试,错误参数,最大为LED_ONL_FLICK = 0xc00
#if !K21_ENABLE
	if((ret=NDK_LedStatus(0xf000)) != NDK_ERR_PARA)
#else
    if((ret=NDK_LedStatus(0xf0000)) != NDK_OK)//枚举值最大为16位，传入时最大合法值为0xffff，大于0xffff的会变编译器截断成为低16位，所以这里会返回成功
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(cls_show_msg("该设备是否支持LED_X?支持[确认],跳过[其它]") == ENTER)
	{
		//case1: led灯全亮
		if((ret=NDK_LedStatus(LED_RED_X_ON | LED_YELLOW_X_ON | LED_GREEN_X_ON | LED_BLUE_X_ON )) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED_X灯全亮,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//case2: led灯全闪
		if((ret=NDK_LedStatus(LED_RED_X_FLICK | LED_YELLOW_X_FLICK | LED_GREEN_X_FLICK | LED_BLUE_X_FLICK )) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED_X灯全闪,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//case3: led灯全灭
		if((ret=NDK_LedStatus(LED_RED_X_OFF | LED_YELLOW_X_OFF | LED_GREEN_X_OFF | LED_BLUE_X_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED_X灯全灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	if(cls_show_msg("该设备是否支持LED射频灯?支持[确认],跳过[其它]") == ENTER)
	{
		//case1: led灯全亮
		if((ret=NDK_LedStatus(LED_RFID_RED_ON | LED_RFID_YELLOW_ON | LED_RFID_GREEN_ON | LED_RFID_BLUE_ON/* \
			| LED_COM_ON | LED_ONL_ON*/)) != NDK_OK)//后面有测通讯灯和联机灯 这里可不要也为了适应ME30MH
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED灯全亮,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case2: led灯全闪
		if((ret=NDK_LedStatus(LED_RFID_RED_FLICK | LED_RFID_YELLOW_FLICK | LED_RFID_GREEN_FLICK | LED_RFID_BLUE_FLICK /*| \
			LED_COM_FLICK | LED_ONL_FLICK*/)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED灯全闪,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case3: led灯全灭
		if((ret=NDK_LedStatus(LED_RFID_RED_OFF | LED_RFID_YELLOW_OFF | LED_RFID_GREEN_OFF | LED_RFID_BLUE_OFF/* \
			| LED_COM_OFF | LED_ONL_OFF*/)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("LED灯全灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case4: 红灯亮闪灭
	cls_show_msg("红灯亮闪灭,任意键继续...");
	if((ret=NDK_LedStatus(LED_RFID_RED_ON)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(2000);
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(4000);
	if((ret=NDK_LedStatus(LED_RFID_RED_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("红灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5: 黄灯亮闪灭
	if(cls_show_msg("该设备是否支持LED黄灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("黄灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_RFID_YELLOW_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_RFID_YELLOW_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_RFID_YELLOW_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("黄灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case6: 绿灯亮闪灭
	if(cls_show_msg("该设备是否支持LED绿灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("绿灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_RFID_GREEN_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_RFID_GREEN_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_RFID_GREEN_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("绿灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//case7: 蓝灯亮闪灭
	if(cls_show_msg("该设备是否支持LED蓝灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("蓝灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_RFID_BLUE_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_RFID_BLUE_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_RFID_BLUE_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(cls_show_msg("蓝灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//case8: 通讯灯亮闪灭
	if(cls_show_msg("该设备是否支持通讯灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("通讯灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_COM_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_COM_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_COM_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(cls_show_msg("通讯灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case9: 联机灯亮闪灭
	if(cls_show_msg("该设备是否支持联机灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("联机灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_ONL_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_ONL_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_ONL_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(cls_show_msg("联机灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case10:验钞灯亮闪灭
	if(cls_show_msg("该设备是否支持验钞灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("验钞灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_DETECTOR_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_DETECTOR_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_DETECTOR_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(cls_show_msg("验钞灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//case11: 磁卡灯亮闪灭
	if(cls_show_msg("该设备是否支持磁卡灯?支持[确认],跳过[其它]") == ENTER)
	{
		cls_show_msg("磁卡灯亮闪灭,任意键继续...");
		if((ret=NDK_LedStatus(LED_MAG_ON)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(2000);
		if((ret=NDK_LedStatus(LED_MAG_FLICK)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysMsDelay(4000);
		if((ret=NDK_LedStatus(LED_MAG_OFF)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(cls_show_msg("磁卡灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	send_result("%s测试通过", TESTAPI);
	goto ERR;
ERR:
	NDK_LedStatus(LED_RFID_RED_OFF | LED_RFID_YELLOW_OFF | LED_RFID_GREEN_OFF | LED_RFID_BLUE_OFF \
		| LED_COM_OFF | LED_ONL_OFF);//灯全灭
#if K21_ENABLE
	NDK_LedStatus(0x2000 | 0x8000);//灯全灭
#endif
#if defined ME11||defined ME15||defined ME15C||defined ME20
	NDK_LedStatus(LED_RFID_BLUE_FLICK);//增加LED打开操作,帮助无液晶的设备确认程序运行成功
#endif
#if STABAR_ENABLE		
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}
#else
void sys15(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int handle = 0, handle2 = 0, ret = -1, i = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	//测试前置,先关闭所有的灯
	NDK_LedOff(0);
	NDK_LedClose(0);
	NDK_LedOff(1);
	NDK_LedClose(1);

	//case1: 红灯亮闪灭
	cls_show_msg("红灯亮闪灭,任意键继续...");
	handle = NDK_LedOpen("red");
	if((ret=NDK_LedOn(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(2000);
	for(i=0;i<2;i++)
	{
		if((ret=NDK_LedOff(handle)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
		if((ret=NDK_LedOn(handle)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
	}
	if((ret=NDK_LedOff(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("红灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2: 蓝灯亮闪灭
#if defined ME66
	cls_show_msg("绿灯亮闪灭,任意键继续...");
	handle2 = NDK_LedOpen("green");
#else
	cls_show_msg("蓝灯亮闪灭,任意键继续...");
	handle2 = NDK_LedOpen("blue");
#endif
	if((ret=NDK_LedOn(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(2000);
	for(i=0;i<2;i++)
	{
		if((ret=NDK_LedOff(handle2)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
		if((ret=NDK_LedOn(handle2)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
	}
	if((ret=NDK_LedOff(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if defined ME66
	if(cls_show_msg("绿灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
#else
	if(cls_show_msg("红灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if defined ME66
	//摄像灯亮闪灭
	cls_show_msg("摄像灯亮闪灭,任意键继续...");
	handle = NDK_LedOpen("cam");
	if((ret=NDK_LedOn(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(2000);
	for(i=0;i<2;i++)
	{
		if((ret=NDK_LedOff(handle)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
		if((ret=NDK_LedOn(handle)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_SysMsDelay(1000);
	}
	if((ret=NDK_LedOff(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("红灯亮闪灭,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case4:红灯亮,蓝灯亮,红灯灭,蓝灯灭
#if defined ME66
	cls_show_msg("红灯亮,绿灯亮,红灯灭,绿灯灭,任意键继续...");
	handle2 = NDK_LedOpen("green");
#else
	cls_show_msg("红灯亮,蓝灯亮,红灯灭,蓝灯灭,任意键继续...");
	handle2 = NDK_LedOpen("blue");
#endif
	handle = NDK_LedOpen("red");
	if((ret=NDK_LedOn(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(1000);
	if((ret=NDK_LedOn(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(1000);
	if((ret=NDK_LedOff(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(1000);
	if((ret=NDK_LedOff(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(1000);
	if((ret=NDK_LedClose(handle)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(handle2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:异常测试,打开的灯名字为red,blue以外的值或者空串或者NULL
#define MAXHANDLE 5	
	if((ret=NDK_LedOpen(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedOpen("")) != NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedOpen("white")) != NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if defined ME66
	if((ret=NDK_LedOpen("blue")) != NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case4:异常测试,不跑飞即可
	if((ret=NDK_LedOn(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedOn(MAXHANDLE)) != NDK_OK)//0
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedOff(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedOff(MAXHANDLE)) != NDK_OK)//0
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedClose(MAXHANDLE)) != NDK_OK)//0
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	//测试后置关闭灯和释放句柄
	//打开红灯
	NDK_LedOff(handle);
	NDK_LedClose(handle);
	NDK_LedOff(handle2);
	NDK_LedClose(handle2);
#if defined ME66
	handle = NDK_LedOpen("green");
	NDK_LedOn(handle);
	handle2 = NDK_LedOpen("cam");
	NDK_LedOn(handle2);
#else
	handle = NDK_LedOpen("blue");
	NDK_LedOn(handle);
#endif
	return;
}
#endif

