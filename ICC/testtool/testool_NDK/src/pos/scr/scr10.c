/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 10.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrRectangle是否在指定区域内画矩型
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrRectangle"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr10
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr10(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0, i = 0;
	uint x0 = 0, y0 = 0, width = 0, height = 0;
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	//case1:参数异常:unStartX,unStartY,unWidth,unHeight,填充方式分别为-1,>屏幕
	if((ret=NDK_ScrRectangle(-1, 0, GUI_GetLCDWidth(),GUI_GetLCDHeight(),RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrRectangle(0, -1, GUI_GetLCDWidth(),GUI_GetLCDHeight(),RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrRectangle(0, 0, -1,GUI_GetLCDHeight(),RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrRectangle(0, 0, GUI_GetLCDWidth(),-1,RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//参数填充方式为-1，应该返回失败//底层将按照枚举类型中一个合理值进行操作，返回成功
	if((ret=NDK_ScrRectangle(0, 0, GUI_GetLCDWidth(),GUI_GetLCDHeight(),-1, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrRectangle(0, 0, GUI_GetLCDWidth()+1,GUI_GetLCDHeight(),RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrRectangle(0, 0, GUI_GetLCDWidth(),GUI_GetLCDHeight()+1,RECT_PATTERNS_NO_FILL, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	NDK_ScrClrs();

	//case2:任意画矩形
	lib_kbflush();		//清除按键缓冲区
	//为实现自动化测试，启动计数器
	lib_StartStopwatch();
	while ((nKeyin=lib_kbhit())==0)
	{
		x0 = rand()%(ViewWidth-1);
		y0 = rand()%(ViewHeight-1);
		width = rand()%(ViewWidth-x0);
		height = rand()%(ViewHeight-y0);
		NDK_ScrRectangle(x0, y0, width, height, RECT_PATTERNS_SOLID_FILL, BLACK);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		NDK_ScrRectangle(x0, y0, width, height, RECT_PATTERNS_SOLID_FILL, WHITE);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		NDK_ScrRectangle(x0, y0, width, height, RECT_PATTERNS_NO_FILL, BLACK);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		NDK_ScrRectangle(x0, y0, width, height, RECT_PATTERNS_NO_FILL, WHITE);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		cls_printf("测试%s...按确认测试通过,其它键失败", TESTAPI);
		NDK_SysDelay(30);
		if (MAXWAITTIME<lib_ReadStopwatch())
			break;
	}

	if (nKeyin==ENTER || nKeyin==0)
		;
	else
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:画嵌套的矩型
	cls_show_msg1(5, "按任意键画嵌套的矩型");
	NDK_ScrClrs();
	for (i=0; i<=(ViewHeight/2); i++)
	{
		NDK_ScrRectangle(i*2, i, ViewWidth-i*2*2, ViewHeight-i*2, RECT_PATTERNS_NO_FILL, BLACK);
		NDK_ScrRefresh();
	}
	NDK_SysDelay(20);

	//case4:画矩型边框(起点越界)
	cls_show_msg1(5, "按任意键画矩型边框(起点越界),"
			"不显示矩形.");
	NDK_ScrClrs(); 
	NDK_ScrRectangle(ViewWidth, 0, 10, 10, RECT_PATTERNS_NO_FILL, BLACK);   //参数超出取值范围
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	NDK_ScrClrs(); 
	NDK_ScrRectangle(0, ViewHeight, 10, 10, RECT_PATTERNS_SOLID_FILL, BLACK);   //参数超出取值范围,并且填充模式为SOLID_FILL，提高语句覆盖率
	NDK_ScrRefresh();
	NDK_SysDelay(20);	
	nKeyin = cls_show_msg1(5, "ENTER测试通过,其它键不通过");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

