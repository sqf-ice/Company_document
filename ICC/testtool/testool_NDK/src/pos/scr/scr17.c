/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 17.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrSetSpace能否设置行间距和字间距
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrSetSpace"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr17
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr17(void)
{
	/*private & local definition*/
	int  ret = 0, i = 0, nKeyin = ENTER;	
	uint unWspace = 0, unHpace = 0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置:保存系统设置

	//case1:参数异常:unWspace,unHpace分别为-1
	//函数定义参数为uint所以-1估计可以成功，应该会变成最大间距，改用例只为增加测试覆盖度
	if((ret=NDK_ScrSetSpace(-1,unHpace))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetSpace(unWspace,-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1:测试字间距	
	//subcase1:测试了wspace大于当前使用的英文字体像素宽度的2倍,预期:字间距为英文字体像素宽度的2倍
	NDK_ScrClrs();
	if((ret=NDK_ScrSetSpace(GUI_GetFontWidth()+10,unHpace))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",GUI_GetFontWidth(),unHpace);
	for(i=0; i<((GUI_GetLCDWidth()/GUI_GetFontWidth())/2-1); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();	
	NDK_SysDelay(20);
	
	//subcase2:测试了wspace等于当前使用的英文字体像素宽度的2倍,字间距为英文字体像素宽度的2倍
	if((ret=NDK_ScrSetSpace(GUI_GetFontWidth(),unHpace))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",GUI_GetFontWidth(),unHpace);
	for(i=0; i<((GUI_GetLCDWidth()/GUI_GetFontWidth())/2-1); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();	
	NDK_SysDelay(20);

	NDK_ScrClrs(); //小屏需要清屏
	//subcase3:测试了wspace小于当前使用的英文字体像素宽度的2倍,预期:字间距为wspace
	if((ret=NDK_ScrSetSpace(GUI_GetFontWidth(),unHpace))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",GUI_GetFontWidth(),unHpace);
	for(i=0; i<((GUI_GetLCDWidth()/GUI_GetFontWidth())/2-1); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(40);
	
	if((ret=NDK_ScrSetSpace(GUI_GetFontWidth()-1,unHpace))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",GUI_GetFontWidth()-1,unHpace);
	for(i=0; i<((GUI_GetLCDWidth()/GUI_GetFontWidth())/2-1); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(40);
	if(ENTER != cls_show_msg1(MAXWAITTIME,"观看效果,按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s设置行间距测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case2:测试行间距	
	//subcase1:hspace大于当前使用的英文字体像素高度时，预期:行间距为英文字体像素高度
	NDK_ScrClrs();
	if((ret=NDK_ScrSetSpace(unWspace,GUI_GetFontHeight()+10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",unWspace,GUI_GetFontHeight());
	for(i=0; i<(GUI_GetLCDWidth()/GUI_GetFontWidth()); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	
	//subcase2:hspace等于当前使用的英文字体像素高度时，预期:行间距为英文字体像素高度
	if((ret=NDK_ScrSetSpace(unWspace,GUI_GetFontHeight()))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",unWspace,GUI_GetFontHeight());
	for(i=0; i<(GUI_GetLCDWidth()/GUI_GetFontWidth()); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	
	//subcase3:hspace小于当前使用的英文字体像素高度时，行间距为hspace
	if((ret=NDK_ScrSetSpace(unWspace,GUI_GetFontHeight()-1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
	NDK_ScrPrintf("预期:字间距(%d),行间距(%d)",unWspace,GUI_GetFontHeight()-1);
	for(i=0; i<(GUI_GetLCDWidth()/GUI_GetFontWidth()); i++)
		NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(40);
	
	nKeyin = cls_show_msg1(MAXWAITTIME, "观看效果,按确认测试通过，其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);	
	
	//恢复默认设置
ERR:
	//需要跟开发确认默认值
	NDK_ScrSetSpace(0,0);
	return;
}

