/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 40.c
* Author 			:
* version			: 
* DATE			: 
* directory 		: 
* description		: 测试NDK_ScrClrsClors是否可以按颜色清除屏幕
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScrClrsColor"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr40
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	    
*
*****************************************************************/
void scr40(void)
{
	/*private & local definition*/
	int i = 0, ret = 0;
	color_t color_tbl[] = {0x0000/*黑*/, 0xFFFF/*白*/, 0xF800/*红*/, 0x07E0/*绿*/, 0x001F/*蓝*/, 0x07FF/*青*/};
	char *color[] = {"黑", "白", "红", "绿", "蓝", "青"};

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//case1:测试NDK_ScrClrsColor
	for(i=0; i<SZ_ARRAY(color_tbl); i++)
	{
		cls_show_msg("按任意键全屏幕显示%s色，且屏幕上除了颜色不应有其它内容",color[i]);
		if((ret = NDK_ScrClsColor(color_tbl[i])) != NDK_OK) 
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			RETURN;
		}
		NDK_SysMsDelay(1*1000); //延时
	}
	if(cls_show_msg("此条信息在屏幕[0,0]处正常显示?是[ENTER[,否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}



