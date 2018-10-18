/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 12.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		:  测试NDK_ScrAutoUpdate能否设置屏幕刷新模式，测试NDK_ScrFullRefresh能否设置全屏幕刷新模式,测试NDK_ScrRefresh能否刷新屏幕
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrAutoUpdate,NDK_ScrFullRefresh,NDK_ScrRefresh"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr12
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr12(void)
{
	/*private & local definition*/
	int  ret = 0;
	int pnOldauto = 0, pnOldauto_set = 0; 
	uint punX = 0, punY = 0, punWidth = 0, punHeight = 0;

	/*process body*/
	cls_show_msg1(2, "测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置：获取当前程序设置好的显示区域和刷新模式
	NDK_ScrGetViewPort(&punX,&punY,&punWidth,&punHeight);
	if((ret=NDK_ScrAutoUpdate(0,&pnOldauto_set))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	NDK_ScrClrs();//界面显示需要插入清屏
	//case1:非0值的测试，应该保持每条屏幕API都会即时显示
	if((ret=NDK_ScrAutoUpdate(-1,&pnOldauto))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("屏幕直接显示本信息,按确认测试通过,其它键失败");
	if(lib_getkeycode(5)!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();

	if(auto_flag==0||auto_flag==2)
	{
		//case2:0值的测试，不自动刷新，只有调用NDK_ScrRefresh()才显示显存中的数据
		NDK_ScrPrintf("按任意键后屏幕将会显示下一条信息");//自动刷新显示本信息后修改成不自动刷新
		if((ret=NDK_ScrAutoUpdate(0,&pnOldauto))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_ScrClrs();
		NDK_ScrPrintf("按任意键后屏幕显示本信息,");
		lib_getkeycode(0);//按键后刷新
		NDK_ScrRefresh();
		if(ENTER != show_msg1(MAXWAITTIME,"按确认测试通过,其它键失败")&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		NDK_ScrClrs();
	}
	
	//case3:测试NDK_ScrAutoUpdate设置成功后返回原来的模式
	if((ret=NDK_ScrAutoUpdate(0,&pnOldauto))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrAutoUpdate(1,&pnOldauto))!=NDK_OK || pnOldauto!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnOldauto);
		GOTOERR;
	}
	if((ret=NDK_ScrAutoUpdate(0,&pnOldauto))!=NDK_OK || pnOldauto!=1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnOldauto);
		GOTOERR;
	}

	//case4:设置成0后，要调用update_viewport函数才对显示内容进行更新
	NDK_ScrAutoUpdate(0,&pnOldauto);
	cls_show_msg1(5, "3秒后将黑屏,6秒后出现白色矩形");
	NDK_ScrRectangle(0,0,punWidth,punHeight,RECT_PATTERNS_SOLID_FILL,BLACK);	
	if((ret=NDK_ScrSetViewPort(10+punX,10+punY,punWidth-20,punHeight-20))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(30);
	NDK_ScrFullRefresh();
	NDK_ScrRectangle(0,0,punWidth-20,punHeight-20,RECT_PATTERNS_SOLID_FILL,WHITE);
	NDK_SysDelay(30);
	NDK_ScrRefresh();
	NDK_SysDelay(30);
	if(ENTER != cls_show_msg1(MAXWAITTIME,"按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	send_result("%s测试通过", TESTAPI);

	//恢复默认设置
ERR:
	NDK_ScrSetViewPort(punX,punY,punWidth,punHeight);
	NDK_ScrAutoUpdate(pnOldauto_set,&pnOldauto);
	return;
}

