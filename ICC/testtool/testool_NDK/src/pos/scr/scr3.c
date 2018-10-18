/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 3.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrPush是否可以保存当前屏幕显示状态,测试NDK_ScrPop是能能恢复利用中后一个pushscreen保存的显示状态
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrPush,NDK_ScrPop"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr3
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr3(void)
{
	/*private & local definition*/
	uint PosX = 0, PosY = 0;
	int nKeyin = ENTER, i = 0, ret = 0;
	EM_TEXT_ATTRIBUTE iOldAttr = {0};
	uint ViewWidth = 0, ViewHeight = 0;

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	NDK_ScrClrs();
	GUI_DispStringEx(0, 0, "具备休眠功能的机具,请确认已开启休眠!ENTER键继续...", TEXT_ATTRIBUTE_REVERSE);
	if(ENTER!=lib_getkeycode(MAXWAITTIME)&&auto_flag!=1)
	{
		send_result("line %d:%s未设置修改，测试中止", __LINE__, TESTAPI);
		return;
	}
	
	//case1:下面push两次，看看是否确实只保留最后一次的screen信息
	while(i<2)
	{
		NDK_ScrClrs();
		PosX = rand()%(ViewWidth-(GUI_GetFontWidth()/2));
		PosY = rand()%(ViewHeight-GUI_GetFontHeight());		//为了正常显示提示信息,进行-英文字体的宽和高的操作
		NDK_ScrGotoxy(PosX, PosY);
		NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE,&iOldAttr);
		NDK_ScrPrintf("测试NDK_ScrPush...");
		NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL,&iOldAttr);
		show_msg1(MAXWAITTIME, "按任意键NDK_ScrPush 第%d次", ++i);	
		if((ret=NDK_ScrPush())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}

	//case2:恢复保留的最后的一次的screen信息
	cls_show_msg1(MAXWAITTIME,"按任意键恢复[NDK_ScrPush 第2次].若设置了休眠,唤醒后,仍在本界面!");//屏幕信息已改变
	if((ret=NDK_ScrPop())!=NDK_OK)//屏幕改变为"pushscreen 第2次"的信息
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);//留出一定的观察时间

	//case3:验证该函数不能够嵌套使用,第2次POP仍然只能恢复第2次保存的界面
	cls_show_msg1(MAXWAITTIME,"按任意键不应恢复[NDK_ScrPush 第1次].");
	if((ret=NDK_ScrPop())!=NDK_OK)//屏幕改变为"pushscreen 第2次"的信息
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);//留出一定的观察时间
	
	nKeyin = cls_show_msg1(MAXWAITTIME, "按确认测试通过,其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	
	return;
}

