/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		:6.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrClrLine是否可以清除若干行,把光标移到(0,unStartY)处
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10
#define 	TESTAPI		"NDK_ScrClrLine"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr6
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr6(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin = 0;
	uint getx = 0, gety = 0, ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(&getx, &gety, &ViewWidth, &ViewHeight);
	NDK_SysDelay(10);
	
	//测试前置:屏幕满字
	if((ret=NDK_ScrRectangle(0,0,ViewWidth,ViewHeight,RECT_PATTERNS_SOLID_FILL,BLACK))!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	NDK_ScrRefresh();
	
	//case1:测试输入参数非法时无效,返回错误信息:unEndY<unStartY,unStartY为-1,unStartY>屏幕像素
	if((ret=NDK_ScrClrLine(10,5))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_ScrClrLine(-1,5))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
	if((ret=NDK_ScrClrLine(ViewHeight,5))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//unEndY为-1,unEndY>屏幕像素,作为=屏幕像素处理,开发需要修改相关NDK文档
	if((ret=NDK_ScrClrLine(ViewHeight/3*2,-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	nKeyin=show_msg1(MAXWAITTIME,"保留%d行点,正确按[ENTER],其它键失败\n",ViewHeight/3*2);
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}	
	//清除1行unEndY=unStartY
	if((ret=NDK_ScrClrLine(10,10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	if((ret=NDK_ScrClrLine(ViewHeight/3,ViewHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	nKeyin=show_msg1(MAXWAITTIME,"保留%d行点,第11行被清行,正确按[ENTER],其它键失败\n",ViewHeight/3);
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case2:清除多行//合法参数测试unEndY>unStartY
	if((ret=NDK_ScrClrLine(2,ViewHeight-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	nKeyin=show_msg1(MAXWAITTIME,"只保留两行点.正确按[ENTER].其它键失败\n");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	NDK_ScrRefresh();
	
	//case3:清除2行unEndY=unStartY+1
	if((ret=NDK_ScrClrLine(0,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrClrLine(2,ViewHeight-1))!=NDK_OK)//本句是为为了清屏幕
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	nKeyin=show_msg1(MAXWAITTIME, "第一二行点行被删除.正确按[ENTER].其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
#if STABAR_ENABLE
	//case4:测试状态栏关闭状态下，NDK_ScrClrLine功能是否正常 20130703chensj
	cls_show_msg1(MAXWAITTIME, "按任意键将在底部显示状态栏,3秒后关闭,再3秒清屏");
	if((ret=NDK_ScrRectangle(0,ViewHeight/2,ViewWidth,ViewHeight/2,RECT_PATTERNS_SOLID_FILL,BLACK))!=NDK_OK)
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI,ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_ScrStatusbar(STATUSBAR_DISP_ALL|STATUSBAR_POSITION_BOTTOM);
	NDK_ScrRefresh();
	NDK_KbGetCode(3,NULL);
	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
	NDK_ScrRefresh();
	NDK_KbGetCode(3,NULL);
	if((ret=NDK_ScrClrLine(0,ViewHeight-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=show_msg1(MAXWAITTIME, "屏幕被清屏.正确按[ENTER].其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
#endif

	send_result("%s测试通过", TESTAPI);
#if STABAR_ENABLE
ERR:
	if(gety!=0)
		NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
	else
		NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

