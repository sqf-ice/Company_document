/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 9.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrLine是否能在指定的两点间画线段
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrLine"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr9
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr9(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	int i = 0, j = 0; 
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);

	//case1:参数异常:unStartX,unStartY,unEndX,unEndY分别为-1,>屏幕
	if((ret=NDK_ScrLine(-1, 10, GUI_GetLCDWidth()-1, GUI_GetLCDHeight()-1, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLine(10, -1, GUI_GetLCDWidth()-1,GUI_GetLCDHeight()-1, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLine(10, 10, -1, GUI_GetLCDHeight()-1, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLine(10, 10, GUI_GetLCDWidth()-1, -1, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLine(10, 10, GUI_GetLCDWidth(), GUI_GetLCDHeight()-1, BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLine(10, 10, GUI_GetLCDWidth()-1, GUI_GetLCDHeight(), BLACK))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	
	//case2:任意画线
	cls_printf("随机画直线.按确认测试通过，其它键失败"); //ME50NX不支持画斜线 20171113
	lib_kbflush();		//清除按键缓冲区
	//为实现自动化测试，启动计数器
	lib_StartStopwatch();
	while ((nKeyin=lib_kbhit())==0)
	{
		x0 = rand()%ViewWidth;
		y0 = rand()%ViewHeight;
		x1 = rand()%ViewWidth;
		y1 = rand()%ViewHeight;
		NDK_ScrLine(x0, y0, x1, y1, BLACK);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		NDK_ScrLine(x0, y0, x1, y1, WHITE);
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(10);
		if (MAXWAITTIME*6<lib_ReadStopwatch())
			break;
	}
	if (nKeyin==ENTER || nKeyin==0)
		;//send_result("%s测试通过" ,TESTAPI);
	else
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:从左到右画直线,然后从右到左画直线
	cls_show_msg1(5, "从左到右画直线,然后从右到左画直线");
	NDK_ScrClrs();
	for (i=0; i<ViewWidth; i+=2)
	{
		for (j=0; j<ViewHeight; j++)
			NDK_ScrLine(i, j, i, j, BLACK);
		NDK_ScrRefresh();//每行刷新现实一次
	}	
	NDK_SysDelay(3);
	NDK_ScrClrs();
	NDK_ScrRectangle(0, 0, ViewWidth, ViewHeight, RECT_PATTERNS_SOLID_FILL, BLACK);//填充成黑色的
	NDK_ScrRefresh();
	NDK_SysDelay(3);	
	for (i=ViewWidth-1;i>=0;i-=2)
	{
		for (j=0; j<ViewHeight; j++)
			NDK_ScrLine(i, j, i, j, WHITE);
		NDK_ScrRefresh();//每行刷新现实一次
	}
	NDK_SysDelay(20);

	//case4:画对角线
	cls_show_msg1(5, "按任意键画对角线");
	NDK_ScrClrs();
	NDK_ScrLine(0,0,ViewWidth-1,ViewHeight-1,BLACK);
	NDK_ScrLine(ViewWidth-1,0,0,ViewHeight-1,BLACK);
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	if(cls_show_msg1(MAXWAITTIME, "若屏幕出现对角线,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)  //ME50NX不支持画斜线 20171113
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示 	

	//case5:越界参数
	NDK_ScrLine((ViewWidth-1)/2, 0, (ViewWidth-1)/2, ViewHeight+2, BLACK);	 //参数超出取值范围
	NDK_SysDelay(20);// 2s
	if(cls_show_msg1(MAXWAITTIME, "若屏幕不出现中垂线,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示 	

	//test over
	nKeyin = cls_show_msg1(MAXWAITTIME, "ENTER测试通过,其它键不通过");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	
	return;
}

