/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 7.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		:  测试NDK_ScrSetViewPort能否设置屏幕的显示区域, 测试NDK_ScrGetViewPort能否获取屏幕的显示区域
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrSetViewPort,NDK_ScrGetViewPort"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr7
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr7(void)
{
	/*private & local definition*/
	int  ret = 0;	
	uint punX = 0, punY = 0, punWidth = 0, punHeight = 0;
	uint punX1 = 0, punY1 = 0, punWidth1 = 0, punHeight1 = 0;

	/*process body*/
	cls_show_msg1(2, "测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	NDK_ScrClrs();
	NDK_ScrRefresh();
	//测试前置:获取当前的屏幕的显示区域//理论上应该等于测试程序初始化时设置可显示的区域大小或者未设置情况下显示区域为实际屏幕尺寸
	if((ret=NDK_ScrGetViewPort(&punX,&punY,&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//case1:NDK_ScrSetViewPort参数异常：unX,unY,unWidth,unHeight分别为-1,>屏幕,unX+unWidth>屏幕,unY+unHeight>屏幕
	if((ret=NDK_ScrSetViewPort(punX-1,0,100,100))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(0,punY-1,100,100))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(0,0,-1,100))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(0,0,100,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//理论边界值为GUI_GetLCDWidth()+1;gp730的实际边界值为GUI_GetLCDWidth()+5
	if((ret=NDK_ScrSetViewPort(0,0,GUI_GetLCDWidth()+5,100))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(0,0,100,GUI_GetLCDHeight()+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//理论边界值为GUI_GetLCDWidth()+1;gp730的实际边界值为GUI_GetLCDWidth()+5	
	if((ret=NDK_ScrSetViewPort(10+punX,10+punY,GUI_GetLCDWidth()-5,100))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(10+punX,10+punY,100,GUI_GetLCDHeight()-9))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case2:设置一个正常的显示区域，应该成功
	//先设置一个,再设置一个区域,看是否最后一次有效 20140603 added by jiangym
	if((ret=NDK_ScrSetViewPort( 15+punX,15+punY,punWidth-30,punHeight-30))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)(x=%d,y=%d,punWidth=%d,punHeight=%d)", __LINE__, TESTAPI, ret, 15+punX,15+punY,punWidth-30,punHeight-30);
		GOTOERR;
	}
	if((ret=NDK_ScrSetViewPort(10+punX,10+punY,punWidth-20,punHeight-20))!=NDK_OK)//此处设置
	{
		send_result("line %d:%s测试失败(%d)(x=%d,y=%d,punWidth=%d,punHeight=%d)", __LINE__, TESTAPI, ret, 10+punX,10+punY,punWidth-20,punHeight-20);
		GOTOERR;
	}

	//case3:验证显示操作只在该区域内有效，可以调用任意一个显示函数	
	if(ENTER != cls_show_msg1(MAXWAITTIME,"本信息是从屏幕像素坐标(10,10)开始显示的,ENTER测试通过;其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case4:清屏操作也只清除像素坐标，清屏后坐标会回到显示区域左上角	
	NDK_ScrClrs();
	if(ENTER != cls_show_msg1(MAXWAITTIME,"|光标将移动到(10,10),ENTER测试通过;其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case5:NDK_ScrGetViewPort为NULL,表示不用获取该项值，返回成功
	if((ret=NDK_ScrGetViewPort(NULL,&punY1,&punWidth1,&punHeight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetViewPort(&punX1,NULL,&punWidth1,&punHeight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetViewPort(&punX1,&punY1,NULL,&punHeight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetViewPort(&punX1,&punY1,&punWidth1,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:获取显示区域大小，应该和case2第2次设置的一致
	if((ret=NDK_ScrGetViewPort(&punX1,&punY1,&punWidth1,&punHeight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(punX1!=10+punX||punY1!=10+punY||punWidth1!=(punWidth-20)||punHeight1!=(punHeight-20))
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, punX1, punY1, punWidth1, punHeight1);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
	
ERR:
	//恢复默认配置
	NDK_ScrSetViewPort(punX,punY,punWidth,punHeight);
	return;
}

