/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name			: 5.c
* Author 			:
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_KbGetPad获取像素坐标点的触屏状态
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10
#define  TESTAPI		"NDK_KbGetPad"
#define  WUCHASEC 0.03//0.03 //这个值待定
#define	 MAXTIMEOUT	10000  //10000ms=10s

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: kb5
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        jiangym		20131128	created
*****************************************************************/
void kb5(void)
{
	/*private & local definition*/
#include "kbtable.h"

	int cnt = 0,ret = 0;
	ST_PADDATA pad_data;
	float tmp = 0;
#if K21_ENABLE
	char str[32] = {0};
#endif
#if (defined ME50||defined SP610)	
		time_t oldtime = 0;
#else
	uint x = 0, y = 0;
	char *type[]={"弹起","按下","按键"};
	int laststatus=-1;
#endif

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:不合理的参数:pstPaddata为NULL
	if ((ret=NDK_KbGetPad(NULL,MAXTIMEOUT))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if !(defined ME50||defined SP610)
	//case2:测试按下弹起的动作timeout为0情况:随意按下屏幕上的点
	//(测试最好覆盖屏幕上，下，左，右，中，左上，右上，左下，右下，九个位置),返回状态值
	cls_printf("请在屏幕上,下,左,右,中,左上,右上,左下,右下九个位置随意按下弹起或按键应可以获取到相应的状态,取消键结束测试.");
	NDK_ScrGetxy(&x,&y);
	while(1)
	{
		memset(&pad_data,0,sizeof(pad_data));
		if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
		if(laststatus!=pad_data.emPadState)
		{
			laststatus = pad_data.emPadState;
			NDK_ScrGotoxy(x, y);
			NDK_ScrPrintf("状态:%s",type[pad_data.emPadState]);
			NDK_ScrRefresh();
		}
		if (pad_data.emPadState == PADSTATE_KEY  && pad_data.unKeycode == ESC)
			break;
	}
	if(cls_show_msg("获取的状态如预期,[Enter]通过,[其他]失败")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif

	//case3:使用循环进行判断按下弹起的动作timeout=0 ,同时使用点的轨迹方式对坐标值判断
	// 5810x平台没有按键\按下\弹起状态获取,只有获取坐标,故要改使用方式
#if  (defined ME50||defined SP610)
	cls_printf("请在屏幕上划点的轨迹,30s后结束测试");
	oldtime = time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>30)
			break;
		memset(&pad_data,0,sizeof(pad_data));
		if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
		NDK_ScrPutPixel(pad_data.unPadX,pad_data.unPadY, BLACK);
		NDK_ScrRefresh();
	}
#else
	cls_printf("请在屏幕上划点的轨迹,取消键结束测试");
	while(1)
	{
		while(1)
		{
			memset(&pad_data,0,sizeof(pad_data));
			if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				return;
			}
			if(pad_data.emPadState == PADSTATE_DOWN) 
			{
				NDK_ScrPutPixel(pad_data.unPadX,pad_data.unPadY, BLACK);
				NDK_ScrRefresh();
			}
			if(pad_data.emPadState ==PADSTATE_UP  || pad_data.emPadState == PADSTATE_KEY)
				break;
		
		}
		if (pad_data.unKeycode == ESC)
			break;
	}
#endif
	if(cls_show_msg("点画的轨迹即手指移动的轨迹,[Enter]通过,[其他]失败")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

#if !(defined ME50||defined SP610)
	//case4:按下按键时，返回按键值timeout=30s
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在10s内按下%s键", table[cnt].keyname);
	memset(&pad_data,0,sizeof(pad_data));
	if ((ret=NDK_KbGetPad(&pad_data,MAXTIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断状态是否正确,坐标值无意义
	if(pad_data.emPadState != PADSTATE_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.emPadState);
		RETURN;
	}
	if (pad_data.unKeycode!=table[cnt].keycode)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.unKeycode);
		RETURN;
	}

	//case5:timeout=0 按下按键返回键值
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请按下%s键", table[cnt].keyname);
	memset(&pad_data,0,sizeof(pad_data));
	if ((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断状态是否正确,坐标值无意义
	if(pad_data.emPadState != PADSTATE_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.emPadState);
		RETURN;
	}
	if (pad_data.unKeycode!=table[cnt].keycode)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.unKeycode);
		RETURN;
	}
#if !defined ME50S //ME50S 没有F1 F2键
	//case8:timeout=0按下按键返回键值
	cls_printf("请按下F1键");
	memset(&pad_data,0,sizeof(pad_data));
	if ((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pad_data.emPadState != PADSTATE_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.emPadState);
		RETURN;
	}
	if (pad_data.unKeycode!=KEY_F1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.unKeycode);
		RETURN;
	}
	
	//case9:timeout=0按下按键返回键值
	cls_printf("请按下F2键");
	memset(&pad_data,0,sizeof(pad_data));
	if ((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pad_data.emPadState != PADSTATE_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.emPadState);
		RETURN;
	}
	if (pad_data.unKeycode!=KEY_F2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pad_data.unKeycode);
		RETURN;
	}
#endif
#endif
	//case6:超时测试
	cls_printf("请在10s内在屏幕、键盘上不做任何动作,请耐心等待...");
	lib_StartStopwatch();
	ret=NDK_KbGetPad(&pad_data,MAXTIMEOUT);//超时返回NDK_ERR_TIMEOUT
	if (ret!=NDK_ERR_TIMEOUT||(tmp=fabs(lib_ReadStopwatch()-MAXWAITTIME))>WUCHASEC)
	{
#if !K21_ENABLE
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, tmp);
#else
		memset(str,0,sizeof(str));
		ftos(tmp, str);
		send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI, ret, str);
#endif
		RETURN;
	}

#if !(defined ME50||defined SP610)
	//case7:测试case6超时异常后，触屏是否能够正常工作 4.1版本有发现case6执行后case7会出现死循环
	//(测试最好覆盖屏幕上，下，左，右，中，左上，右上，左下，右下，九个位置),返回状态值
	cls_printf("请在屏幕上,下,左,右,中,左上,右上,左下,右下九个位置随意按下弹起或按键应可以获取到相应的状态,取消键结束测试.");
	NDK_ScrGetxy(&x,&y);
	while(1)
	{
		memset(&pad_data,0,sizeof(pad_data));
		if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
		NDK_ScrGotoxy(x, y);
		NDK_ScrPrintf("状态:%s",type[pad_data.emPadState]);
		NDK_ScrRefresh();
			
		if (pad_data.emPadState == PADSTATE_KEY  && pad_data.unKeycode == ESC)
			break;
	}
	if(cls_show_msg("获取的状态如预期,[Enter]通过,[其他]失败")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif

	send_result("%s测试通过", TESTAPI);
	return;
}

