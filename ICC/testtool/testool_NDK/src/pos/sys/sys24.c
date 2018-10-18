/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys24.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_SysSetShutdownDelay接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysSetShutdownDelay"

#if defined ME50C
#define CONFIG_RTC_AUTO_SHUTDOWN (2*128)
#elif defined ME50N||defined SP610
#define CONFIG_RTC_AUTO_SHUTDOWN (5*60)
#else
#define CONFIG_RTC_AUTO_SHUTDOWN (128)
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		    	  jiangym		20140514   created
*****************************************************************/
#if 1//defined ME11||defined ME15||defined ME20||defined ME15C
void sys24(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	
	//以下为局部变量
	int nRet = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 
	
#if defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B	//打开蓝牙
	NDK_PortOpen(PORT_NUM_BT, NULL);
	NDK_BTEnterCommand();
	NDK_BTSetLocalName("ME15");
	NDK_BTExitCommand();
#endif 

#if !(defined SP610||defined ME15B||defined ME66)	//SP610默认休眠不关机
	if(cls_show_msg("是否进行:默认休眠5分钟后关机用例测试，是按确认键，否则按其它键")==ENTER)
	{
		if(cls_show_msg("本用例需开机首次进入此用例进行测试,[Enter]是,[其他]否")==ENTER)
		{
			//case1:未设置休眠后关机时间默认是5分钟
			NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
#if defined ME65//与开发确认，ME65休眠后LED灯会灭掉          		20180725    wangk     modify		
			cls_show_msg("此时蓝灯长亮(无灯设备请忽略灯),按任意键进入休眠后,蓝灯灭掉,拔掉外电,休眠5分钟后应该自动关机");
#else
			cls_show_msg("蓝灯长亮(无灯设备请忽略灯),按任意键进入休眠,拔掉外电,休眠5分钟后应该自动关机");
#endif
			if((nRet=NDK_SysGoSuspend()) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				RETURN;
			}
			sleep(1);
		}
	}
#endif

	if(cls_show_msg("是否进行:休眠不关机用例测试，是按确认键，否则按其它键")==ENTER)
	{
		//case2:设置0不会关机，红灯应该长亮5分钟以上
		NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
#if defined ME65//与开发确认，ME65休眠后LED灯会灭掉          		20180725    wangk     modify
		cls_show_msg("按任意键进入休眠,蓝灯灭掉(无灯设备请忽略灯),休眠5分钟后唤醒(唤醒方法根据产品特性选择例如:按键唤醒,蓝牙唤醒等)");
#else
		cls_show_msg("按任意键进入休眠,蓝灯长亮5分钟以上(无灯设备请忽略灯),休眠5分钟后唤醒(唤醒方法根据产品特性选择例如:按键唤醒,蓝牙唤醒等)，唤醒后，蓝灯将闪烁");
#endif
#if !defined SP610	//SP610默认不设置休眠自动关机,应用不设置情况下测试
		if((nRet = NDK_SysSetShutdownDelay(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
#endif		
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(1);
#if !(defined ME15CHX||defined ME15B||defined ME66)
		cls_show_msg1(2,"唤醒成功,2秒后蓝灯闪烁(无灯设备请忽略灯)");
		NDK_LedStatus(LED_RFID_BLUE_FLICK);
#else
		cls_show_msg1(2,"唤醒成功,2秒后蓝灯亮起(无灯设备请忽略灯)");
		nRet = NDK_LedOpen("blue");
		NDK_LedOn(nRet);
#endif
	}
#if CPU5810X_ENABLE||defined ME50NX||defined ME68 //5810平台上，NDK_SysSetShutdownDelay需要128S的整数倍,zhengry add
	if(cls_show_msg("是否进行:休眠%d秒后进充电模式用例测试，是按确认键，否则按其它键",CONFIG_RTC_AUTO_SHUTDOWN)==ENTER)
	{
		//case3:测试插着USB线 设置时间后能否自动关机
		NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
		cls_show_msg("蓝灯长亮(无灯设备请忽略灯),确保插USB线,按任意键%d秒后会红灯长亮或闪烁进入充电状态",CONFIG_RTC_AUTO_SHUTDOWN);	//进入充电状态后如果拔掉外线设备就关机了
		if((nRet = NDK_SysSetShutdownDelay(CONFIG_RTC_AUTO_SHUTDOWN/*128*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		//NDK_LedStatus(LED_RFID_RED_OFF);//通过指示灯判断是否关机
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(1);
	}

	if(cls_show_msg("是否进行:休眠%d秒后关机用例测试，是按确认键，否则按其它键",CONFIG_RTC_AUTO_SHUTDOWN)==ENTER)
	{
		//case4:正常调用不插USB线
		NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
		cls_show_msg("蓝灯长亮(无灯设备请忽略灯),请按任意键后马上拔掉USB线,%d秒钟后蓝灯将灭掉关机",CONFIG_RTC_AUTO_SHUTDOWN);
		if((nRet = NDK_SysSetShutdownDelay(CONFIG_RTC_AUTO_SHUTDOWN/*128*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN; 
		}
		sleep(1);
	}
#else

#if !(defined ME15CHX||defined ME15B||defined ME66)//经过开发确认ME15CHX不支持进入充电模式
	if(cls_show_msg("是否进行:休眠10秒后进充电模式用例测试，是按确认键，否则按其它键")==ENTER)
	{
		//case3:测试插着USB线 设置时间后能否自动关机
		NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
#if defined ME65//与开发确认，ME65休眠后LED灯会灭掉          		20180725    wangk     modify
		cls_show_msg("此时蓝灯长亮(无灯设备请忽略灯),确保插USB线,按任意键10秒后会红灯长亮进入充电状态");	//进入充电状态后如果拔掉外线设备就关机
#else
		cls_show_msg("蓝灯长亮(无灯设备请忽略灯),确保插USB线,按任意键10秒后会红灯长亮或闪烁进入充电状态");	//进入充电状态后如果拔掉外线设备就关机了
#endif
		if((nRet = NDK_SysSetShutdownDelay(10))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		//NDK_LedStatus(LED_RFID_RED_OFF);//通过指示灯判断是否关机
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(1);
	}
#endif	
	if(cls_show_msg("是否进行:休眠5秒后关机用例测试，是按确认键，否则按其它键")==ENTER)
	{
		//case4:正常调用不插USB线		
#if defined ME15CHX||defined ME15B||defined ME66	//ME15CHX不拔线也会关机,如果拔线会唤醒休眠,所以要再调一次休眠,后续才会关机
		nRet = NDK_LedOpen("blue");
		NDK_LedOn(nRet);
		cls_show_msg("蓝灯长亮(无灯设备请忽略灯),请按任意键后等待5秒,5秒钟后蓝灯将灭掉关机");
#else
		NDK_LedStatus(LED_RFID_BLUE_ON);//通过指示灯判断是否关机
#if defined ME65//与开发确认，ME65休眠后LED灯会灭掉          		20180725    wangk     modify
		cls_show_msg("此时蓝灯长亮(无灯设备请忽略灯),按任意键后,蓝灯灭掉,马上拔掉USB线,5秒钟后关机");
#else
		cls_show_msg("蓝灯长亮(无灯设备请忽略灯),请按任意键后马上拔掉USB线,5秒钟后红灯将灭掉关机");
#endif
#endif
		if((nRet = NDK_SysSetShutdownDelay(5))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(1);
#if defined ME15CHX||defined ME15B	||defined ME66		
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
#endif		
	}
#endif
}
#else
void sys24(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int nRet = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

#if defined ME30||defined ME30MH||defined ME32||defined ME50||defined ME50N||defined SP610
	//case1:未设置休眠后关机时间默认是5分钟
	cls_show_msg("确保未插USB线,任意键继续");
	if(cls_show_msg("本用例需开机首次进入此用例进行测试,[Enter]是,[其他]否")==ENTER)
	{
		cls_printf("休眠5分钟后将自动关机(机器不接USB线)");
		if((nRet=NDK_SysGoSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(1);
	}
#endif

	//case2:设置0不会关机
	cls_printf("将测试休眠5分钟后不会自动关机,休眠5分钟后按键唤醒");
	if((nRet = NDK_SysSetShutdownDelay(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("等待5分钟后未自动关机,[Enter]正确,[其他]失败")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:测试插着USB线 设置时间后能否自动关机
	cls_show_msg("确保插USB线,任意键继续");
	if((nRet = NDK_SysSetShutdownDelay(10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	cls_printf("第一次测试请等待10s将提示正在充电界面,第二次测试请在10s内按键唤醒");//本用例有两种情况：10秒时间到后进入充电界面(设计如此)，或者在10秒内唤醒回到正常操作 20140812 zengjj 说明该该功能
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	
	//case4:正常调用不插USB线
	cls_show_msg("确保未插USB线,任意键继续");
	if((nRet = NDK_SysSetShutdownDelay(5))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	cls_printf("休眠5s后将自动关机");
	if((nRet=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	NDK_SysDelay(5);
	//测试结束
	//send_result("%s测试通过", TESTAPI);
	//return;
}
#endif
