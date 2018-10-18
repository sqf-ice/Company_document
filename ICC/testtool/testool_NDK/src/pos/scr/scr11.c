/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 11.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrPutPixel是否能在LCD屏幕上设置一个点，测试NDK_ScrGetPixel能否取当前LCD屏幕上的一点(x,y)的颜色
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrPutPixel,NDK_ScrGetPixel"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr11
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr11(void)
{
	/*private & local definition*/
	uint x = 0, y = 0, i = 0, j = 0;
	int nKeyin = ENTER, ret = 0;
	color_t punColor = BLACK;
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	//case1:参数异常:x,y分别为-1,>屏幕,NDK_ScrPutPixel失败;punColor为NULL,NDK_ScrGetPixel失败
	if((ret=NDK_ScrPutPixel(ViewWidth, 0, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrPutPixel(0, ViewHeight, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrPutPixel(-1, 0, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrPutPixel(0, -1, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetPixel(ViewWidth, 0, &punColor))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetPixel(0, ViewHeight, &punColor))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetPixel(-1, 0, &punColor))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetPixel(0, -1, &punColor))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetPixel(0, 0, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:任意画一点	
	lib_kbflush();//清除按键缓冲区
	//为实现自动化测试，启动计数器
	lib_StartStopwatch();
	while ((nKeyin=lib_kbhit())==0)
	{
		x = rand()%ViewWidth;
		y = rand()%ViewHeight;
		
		cls_show_msg1(3, "将在(%d, %d)画一黑点,随后再画一白点.测试正常,可按ENTER;有异常,请按其它键", x, y);
		NDK_ScrClrs();
		NDK_ScrPutPixel(x, y, BLACK);
		NDK_ScrRefresh();
#if !(defined ME50NX ||defined ME68||CPU5810X_ENABLE)//ME50NX\CPU5810X_ENABLE平台产品不支持NDK_ScrGetPixel 20171113
		NDK_ScrGetPixel(x, y, &punColor);
		if (BLACK!=punColor)
		{
			send_result("line %d:getpixel应为BLACK", __LINE__);
			RETURN;
		}
#endif
		NDK_SysBeep();
		NDK_SysDelay(10);
		NDK_ScrPutPixel(x, y, WHITE);
		NDK_ScrRefresh();
#if !(defined ME50NX ||defined ME68||CPU5810X_ENABLE) //ME50NX\CPU5810X_ENABLE平台产品不支持NDK_ScrGetPixel 20171113
		NDK_ScrGetPixel(x, y, &punColor);
		if (WHITE!=punColor)
		{
			send_result("line %d:getpixel应为WHITE(实际:0x%4x)", __LINE__, punColor);
			RETURN;
		}
#endif
		NDK_SysBeep();
		NDK_SysDelay(10);
		if (MAXWAITTIME<lib_ReadStopwatch())
			break;
	}
	if (nKeyin==ENTER || nKeyin==0)//nKeyin==0说明超时，那么就默认测试通过
		;
	else
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		
	//case3:全屏幕画黑点
	cls_show_msg1(5, "请按任意键全屏幕画黑点,再全屏幕画白点");
	for (i=0; i<ViewWidth; i++)
	{
		for (j=0; j<ViewHeight; j++)
			NDK_ScrPutPixel(i, j, BLACK);
		NDK_ScrRefresh();//每行刷新现实一次
	}
	NDK_SysDelay(20);
	
	//case4:全屏幕画白点
	for (i=0; i<ViewWidth; i++)
	{
		for (j=0; j<ViewHeight; j++)
			NDK_ScrPutPixel(i, j, WHITE);
		NDK_ScrRefresh();//每行刷新现实一次
	}
	NDK_SysDelay(20);
	
	//case5:隔行隔列画黑点
	cls_show_msg1(5, "请按任意键隔行隔列画黑点");
	NDK_ScrClrs();
	for (i=0; i<ViewWidth; i+=2)
	{   
		for (j=0; j<ViewHeight; j+=2)
			NDK_ScrPutPixel(i, j, BLACK);			
		NDK_ScrRefresh();//每行刷新现实一次
	}	  	
	NDK_SysDelay(20);
	
	//case6:隔行隔列画白点
	cls_show_msg1(5, "请按任意键隔行隔列画白点");
	NDK_ScrRectangle(0,0,ViewWidth,ViewHeight,RECT_PATTERNS_SOLID_FILL,BLACK);//填充成黑色的
	for (i=0; i<ViewWidth; i+=2)
	{   
		for (j=0; j<ViewHeight; j+=2)
			NDK_ScrPutPixel(i, j, WHITE);	
		NDK_ScrRefresh();//每行刷新现实一次
	}
	NDK_SysDelay(20);
	nKeyin = cls_show_msg1(5, "ENTER测试通过,其它键不通过");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);

	return;
}

