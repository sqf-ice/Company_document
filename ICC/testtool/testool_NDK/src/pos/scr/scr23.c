/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: scr23.c
* Author 			: jiangym
* version			: 
* DATE			: 20130218
* directory 		: 
* description		: 测试 NDK_ScrStatusbar能否正确控制屏幕底部显示状态栏
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScrStatusbar"
#if STABAR_LED_ENABLE		
#if OVERSEAS_ENABLE
#define	FILETEST		"typeAunMaster.NLD"
#define	TESTAppName		"typeAunMaster" 
#else
#define  FILETEST  "App_U_NonMaster.NLD" 
#define	TESTAppName		"App_U_NonMaster" 
#endif
#endif
#define     MAXWAITTIME  10//30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr23
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	     jiangym         20130224  	   created
*
*****************************************************************/
void scr23(void)
{
	/*private & local definition*/
	int ret = 0, k = 0;
	float oldtime = 0.0, diff = 0.0;
	EM_STATUSBAR displayposition = STATUSBAR_POSITION_TOP;
	uint getx = 0, gety = 0, ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
#if STABAR_LED_ENABLE		
	if(cls_show_msg1(30, "测试%s...,请确保%s存在,ESC退出", TESTAPI, FILETEST)==ESC)
		return;
#else
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
#endif
	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(&getx, &gety, &ViewWidth, &ViewHeight);
	 
	//case1:参数异常:unFlag为-1或不是其规定的参数内的数如3，应该返回失败
	if((ret=NDK_ScrStatusbar(-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrStatusbar(3))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	for(k = 0; k < 2 ; k++)
	{
		if(k==0)
			displayposition=STATUSBAR_POSITION_TOP;//状态栏显示在顶部
		else
			displayposition=STATUSBAR_POSITION_BOTTOM;//状态栏显示在底部
		
		//case2:正常测试:参数0,应该不显示状态栏
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "%s不显示状态栏,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case3:正常测试:参数2,状态栏应该只显示时间
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示时间,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case4:正常测试:参数4,状态栏应该只显示电量
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_BATTERY|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示电量,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case5:正常测试:参数8,状态栏应该只显示无线信号
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WLSIGNAL|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示无线信号,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case9:正常测试:参数0x10,状态栏应该只显示wifi信号
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WIFI|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		/* WIFI需要在连接情况下才能显示WIFI图标，故WIFI状态显示结果由WIFI单元模块进行验证
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示wifi信号,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}*/
#if !(defined ME50S||defined ME50NS)	
		//case10:正常测试:参数0x20,状态栏应该只显示MODEM状态
		//由于开机未操作MODEM时，不显示MODEM图标故这里可能将不显示图标
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_MODEM|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示MODEM状态(如果开机未操作过MDM则不显示),按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case11:正常测试:参数0x40,状态栏应该只显示ETH状态
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ETH|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s只显示ETH状态,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
#endif
		//case:正常测试:参数0x100,状态栏显示蓝牙图标,由于在蓝牙未连接时不显示蓝牙图标,所以具体显示效果在蓝牙模块进行验证,本处只要测试调用无异常返回成功即可
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_BLUETOOTH|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case14:参数0x80,状态栏应该显示日期,STATUSBAR_DISP_TIME未开启情况下无效
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_DATE|displayposition))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s没有显示日期,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case12:正常测试:参数0x80,状态栏应该显示日期,需要在STATUSBAR_DISP_TIME开启情况下有效
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_DATE|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s显示日期和时间,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case13:显示时间与状态图标时，时间靠左,图标靠右
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_ETH|STATUSBAR_DISP_WLSIGNAL|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s显示是否时间靠左,图标靠右,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case6:正常测试:STATUSBAR_DISP_ALL,状态栏应该显示无线信号、时间、电量,状态栏应该默认显示
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ALL|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s显示无线信号、时间、电量,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case15:测试显示所有图标
		//cls_show_msg("0x%8x",STATUSBAR_DISP_WLSIGNAL|STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_DATE|STATUSBAR_DISP_TIME|STATUSBAR_DISP_ETH|STATUSBAR_DISP_MODEM|STATUSBAR_DISP_WIFI|displayposition);
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WLSIGNAL|STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_DATE|STATUSBAR_DISP_TIME|STATUSBAR_DISP_ETH|STATUSBAR_DISP_MODEM|STATUSBAR_DISP_WIFI|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//当显示状态图标与时间冲突时，隐藏时间，优先显示图标  (验证看看是否是这样)
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s是否优先显示状态图标,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case7:测试将光标移动到状态栏坐标范围内，应该不会在状态栏范围内显示信息
		cls_printf("状态栏范围不应该显示其它信息");
		oldtime = time(NULL);
		while((diff=time(NULL)-oldtime)<MAXWAITTIME && (lib_kbhit()==0))//在30秒时间范围内
		{	
			if(displayposition==STATUSBAR_POSITION_BOTTOM)//状态栏在底部
				NDK_ScrDispString(0,ViewHeight-GUI_GetFontHeight(),"本信息不应该显示在状态栏范围内",0);
			else
				NDK_ScrDispString(0,0,"本信息不应该显示在状态栏范围内",0);
			show_stopwatch(ENABLE, diff);//倒计时
		}
		show_stopwatch(DISABLE, 0);
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏不应该出现异常信息,按[确认]测试通过,[其它]失败")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case8:验证状态栏关闭后，原状态栏位置可以重新显示其他信息
		cls_show_msg1(30, "按任意键将关闭状态栏后全屏显示黑点，包括原状态栏位置");
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_ScrRefresh();
		NDK_KbGetCode(1,NULL);
		if((ret=NDK_ScrRectangle(0,0,GUI_GetLCDWidth(),GUI_GetLCDHeight(),RECT_PATTERNS_SOLID_FILL,BLACK))!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d", __LINE__, TESTAPI,ret);
			GOTOERR;
		}
		NDK_ScrRefresh();
		NDK_KbGetCode(3,NULL);
		if(ENTER != cls_show_msg1(MAXWAITTIME, "全屏黑点无空白.正确按[ENTER].其它键失败")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		//case16:模拟led灯对状态栏图标的影响20180416 linying added
#if STABAR_LED_ENABLE		
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_LED|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s能正常显示模拟LED灯四个方块,是按[Enter],否按[其他]", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case16.1验证主应用在开启模拟LED的情况下执行子应用，子应用中不应该出现模拟led灯闪烁
		if(( ret = NDK_AppLoad((const char *)FILETEST,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(( ret = NDK_AppRun((const char *)TESTAppName))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "子应用中没有显示状态栏及模拟LED灯方块,是按[Enter],否按[其他]")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case16.2:模拟led灯和电量(不冲突图标)同时存在
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_LED|STATUSBAR_DISP_BATTERY|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s能正常显示模拟LED灯四个方块和电量图标,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		
		//case16.3:模拟led灯和时间图标(冲突图标)优先显示LED灯
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_LED|STATUSBAR_DISP_TIME|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s能正常显示模拟LED灯四个方块,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case16.4 关闭模拟led状态栏后，再开启状态栏显示时间和日期，应该能正常显示
		NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
		if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_DATE|displayposition))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(ENTER != cls_show_msg1(MAXWAITTIME, "状态栏在%s显示日期和时间,按[确认]测试通过,[其它]失败", (displayposition==0)?"顶部":"底部")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}	
#endif
	}
	send_result("%s测试通过", TESTAPI);
//恢复默认设置
	goto ERR;
ERR:
	if(gety!=0)//y坐标非0说明状态栏在顶部打开
		NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
	else
		NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
	NDK_ScrFullRefresh();
#if STABAR_LED_ENABLE			
	NDK_AppDel(TESTAppName);
#endif
	return;
}
