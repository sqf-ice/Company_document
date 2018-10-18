/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	触屏/休眠交叉
* file name			: systest56.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 触屏/休眠交叉测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20140220  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"触屏/休眠交叉"

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
* history 				: author			date			remarks
					 chensj			20140220	created
*****************************************************************/	
int CreatePoint(void)
{
	int start_x1_point = 0, start_y1_point = 0, ret = 0, wide = 20, height = 20;
	static int prev_x1_point , prev_y1_point = 0; //记录上一次的黑块位置，方便清除黑块(为了不完全清屏)
	ST_PADDATA pad_data;
	uint unWidth = 0, unHeight = 0;

	NDK_ScrGetViewPort(NULL, NULL, &unWidth, &unHeight);
	//unWidth = GUI_GetLCDWidth();
	//unHeight = GUI_GetLCDHeight();
	
	while(1)
	{
		memset(&pad_data,0,sizeof(pad_data));
		if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
		{
			cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
			return FAIL;
		}

		if(prev_x1_point!=0||prev_y1_point!=0) //首次进入不运行
		{
			if((ret = NDK_ScrRectangle(prev_x1_point, prev_y1_point ,wide,height,RECT_PATTERNS_SOLID_FILL,0xFFFF))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
				return FAIL;
			}
		}
		
		if (pad_data.emPadState == PADSTATE_KEY  && pad_data.unKeycode == ESC)
		{
			return ESC;
		}

		//使用确认键会被识别为长按键,故改成退格键
		if (pad_data.emPadState == PADSTATE_KEY  && pad_data.unKeycode == BACKSPACE) 
		{
			return BACKSPACE;
		}

		if(pad_data.emPadState == PADSTATE_UP)
		{
			/*对触摸点进行修正,防止太边缘显示的方框不正常*/
			start_x1_point = pad_data.unPadX;
			start_y1_point = pad_data.unPadY;
			
			if(unWidth - pad_data.unPadX < 10)
				start_x1_point = unWidth - 10-1;

			if(pad_data.unPadX < 10)
				start_x1_point = 10;

			if(unHeight - pad_data.unPadY < 10)
				start_y1_point = unHeight - 10-1;

			if(pad_data.unPadY < 10)
				start_y1_point = 10;
			
			if((ret = NDK_ScrRectangle(start_x1_point-10, start_y1_point-10 ,wide,height,RECT_PATTERNS_SOLID_FILL,0))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
				return FAIL;
			}
			prev_x1_point = start_x1_point-10;
			prev_y1_point = start_y1_point-10;
			NDK_ScrRefresh();
			break;
		}
	}
	return SUCC;
}

void systest56(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	int ret = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	//测试前置:测试前屏幕不应漂移
	if(systest_touch()!=SUCC)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
		return;
	}

	cls_printf("1.任意按触摸屏\n2.[退格]进休眠\n3.按键唤醒\n4.[取消]退出");
	while(1)
	{
		if((ret = CreatePoint())==ESC)
		{
			break;
		}
		else if(ret == FAIL)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
			return;
		}	
		if(ret == BACKSPACE)//使用确认键会被识别为长按键,故改成退格键
		{
#if ANDIROD_ENABLE
			if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
			if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
			{
				cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
				return;
			}
		}
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if(cls_show_msg("休眠前和休眠唤醒后[黑块]的位置是否一致\n[确认]是 [其他]否")!=ENTER)
	{
		cls_show_msg1(g_keeptime,"%s 测试失败,休眠后触屏依然在工作",TESTITEM);
		return;
	}
	
	//休眠唤醒后屏幕不应漂移
	if(systest_touch()!=SUCC)
	{
		cls_show_msg1(g_keeptime,"%s line: %d测试失败", TESTITEM, __LINE__);
		return;
	}
	
	cls_show_msg("%s 测试通过", TESTITEM );
	return;
}

