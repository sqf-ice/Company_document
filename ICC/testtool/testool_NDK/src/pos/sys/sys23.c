/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys23.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 本例测试NDK_LedSetFlickParam 接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_LedSetFlickParam"	

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
*			  	  	    jiangym			20140422		created
*****************************************************************/
void sys23(void)
{
	//以下为局部变量
	int ret = -1;
	unsigned int unLen=0;
	char szBuf[128]={0};
	ST_LED_FLICK flick_param;
	flick_param.unFlickOn = 5; //0.5s
	flick_param.unFlickOff = 5;//0.5s	
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置

	NDK_SysGetPosInfo (SYS_HWINFO_GET_HARDWARE_INFO, &unLen, szBuf);
	if (szBuf[12] != 0x01)//szBuf[12]:是否有射频LED灯 0xFF :否 0x01 :是 
	{
		//如果没有LED灯会通过开启状态栏来模拟LED灯20180409 modify 根据开发说明
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
		send_result("line %d:%s测试该机型没有LED灯", __LINE__,TESTAPI);
		return;
#endif
		}	
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	//case1:异常测试,错误参数,最大为LED_ONL_FLICK = 0xc00
	if((ret=NDK_LedSetFlickParam(0xf000,flick_param)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//参数为0,返回ok
	if((ret=NDK_LedSetFlickParam(0x00,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2: 不是flick的参数无效不起作用,返回ok,设置的时间间隔5s应该无效,红灯按默认的100ms进行闪烁
	cls_show_msg("红灯闪烁,亮100ms灭100ms,任意键继续...");
	flick_param.unFlickOn = 50; //5s
	flick_param.unFlickOff = 50;//5s	
	if((ret=NDK_LedSetFlickParam(LED_RFID_RED_ON,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("红灯闪烁,亮灭间隔约为100ms,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3: 红灯闪烁,亮0.5s灭0.5s
	cls_show_msg("红灯闪烁,亮0.5s灭0.5s,任意键继续...");
	flick_param.unFlickOn = 5; //0.5s
	flick_param.unFlickOff = 5;//0.5s	
	if((ret=NDK_LedSetFlickParam(LED_RFID_RED_FLICK,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("红灯闪烁,亮0.5s灭0.5s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4: 黄灯闪烁,亮1s灭1s
	cls_show_msg("黄灯闪烁,亮1s灭1s,任意键继续...");
	flick_param.unFlickOn = 10; //1s
	flick_param.unFlickOff = 10;//1s
	if((ret=NDK_LedSetFlickParam(LED_RFID_YELLOW_FLICK,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_YELLOW_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("黄灯闪烁,亮1s灭1s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_YELLOW_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5: 绿灯闪烁,亮0.5s灭0.5s
	cls_show_msg("绿灯闪烁,亮0.5s灭0.5s,任意键继续...");
	flick_param.unFlickOn = 5; //0.5s
	flick_param.unFlickOff = 5;//0.5s
	if((ret=NDK_LedSetFlickParam(LED_RFID_GREEN_FLICK,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_GREEN_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("绿灯闪烁,亮0.5s灭0.5s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_GREEN_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6: 蓝灯闪烁,亮1s灭1s
	cls_show_msg("蓝灯闪烁,亮1s灭1s,任意键继续...");
	flick_param.unFlickOn = 10; //1s
	flick_param.unFlickOff = 10;//1s
	if((ret=NDK_LedSetFlickParam(LED_RFID_BLUE_FLICK,flick_param)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_BLUE_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("蓝灯闪烁,亮1s灭1s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_BLUE_OFF)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7: 通讯灯闪烁,亮1s灭1s
    if(cls_show_msg("该设备是否支持通讯灯?支持[确认],跳过[其它]") == ENTER)
	{
	    cls_show_msg("通讯灯闪烁,亮1s灭1s,任意键继续...");
	    flick_param.unFlickOn = 10; //1s
	    flick_param.unFlickOff = 10;//1s
	    if((ret=NDK_LedSetFlickParam(LED_COM_FLICK,flick_param)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	    if((ret=NDK_LedStatus(LED_COM_FLICK)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	     }
	    if(cls_show_msg("通讯灯闪烁,亮1s灭1s,[确认]通过,[其他]失败") != ENTER)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	    if((ret=NDK_LedStatus(LED_COM_OFF)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	}
	//case8: 联机灯闪烁,亮0.5s灭0.5s
    if(cls_show_msg("该设备是否支持联机灯?支持[确认],跳过[其它]") == ENTER)
	{
	    cls_show_msg("联机灯闪烁,亮0.5s灭0.5s,任意键继续...");
	    flick_param.unFlickOn = 5; //0.5s
	    flick_param.unFlickOff = 5;//0.5s
	    if((ret=NDK_LedSetFlickParam(LED_ONL_FLICK,flick_param)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	    if((ret=NDK_LedStatus(LED_ONL_FLICK)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	    if(cls_show_msg("联机灯闪烁,亮0.5s灭0.5s,[确认]通过,[其他]失败") != ENTER)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
	    if((ret=NDK_LedStatus(LED_ONL_OFF)) != NDK_OK)
	    {
		    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		    GOTOERR;
	    }
    }
	
	//case9:灯全闪烁,亮0.5s灭0.5s,可以同时设置多个等的闪烁时间间隔
	cls_show_msg("LED灯全闪烁,亮0.5s灭0.5s,任意键继续...");
	flick_param.unFlickOn = 5; //0.5s
	flick_param.unFlickOff = 5;//0.5s
	if((ret=NDK_LedSetFlickParam(LED_RFID_RED_FLICK | LED_RFID_YELLOW_FLICK | LED_RFID_GREEN_FLICK | LED_RFID_BLUE_FLICK/* | \
		LED_COM_FLICK | LED_ONL_FLICK*/,flick_param)) != NDK_OK)//现在多数的产品不具备通讯灯和联机灯 故屏蔽20180412
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK | LED_RFID_YELLOW_FLICK | LED_RFID_GREEN_FLICK | LED_RFID_BLUE_FLICK /* | \
		LED_COM_FLICK | LED_ONL_FLICK*/)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("LED灯全闪烁,亮0.5s灭0.5s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedStatus(LED_RFID_RED_OFF | LED_RFID_YELLOW_OFF | LED_RFID_GREEN_OFF | LED_RFID_BLUE_OFF /* | \
		LED_COM_FLICK | LED_ONL_FLICK*/)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:参数设置的有效性是多次的，关灯后再打开仍然按之前设置的时间进行闪烁
	cls_show_msg("测试参数设置的有效性,任意键继续...");
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK | LED_RFID_YELLOW_FLICK | LED_RFID_GREEN_FLICK | LED_RFID_BLUE_FLICK/* | \
		LED_COM_FLICK | LED_ONL_FLICK*/)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("LED灯全闪烁,跟上次一样亮0.5s灭0.5s,[确认]通过,[其他]失败") != ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_LedStatus(LED_RFID_RED_OFF | LED_RFID_YELLOW_OFF | LED_RFID_GREEN_OFF | LED_RFID_BLUE_OFF/* | \
		LED_COM_FLICK | LED_ONL_FLICK*/);//灯全灭
	flick_param.unFlickOn = 1; //100ms
	flick_param.unFlickOff = 1;//100ms  恢复原始数据
	NDK_LedSetFlickParam(LED_RFID_RED_FLICK | LED_RFID_YELLOW_FLICK | LED_RFID_GREEN_FLICK | LED_RFID_BLUE_FLICK /* | \
		LED_COM_FLICK | LED_ONL_FLICK*/,flick_param);
#if STABAR_ENABLE		
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

