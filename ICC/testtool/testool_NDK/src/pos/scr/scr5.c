/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 5.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrClrs是否可以清除屏幕,并把光标移到(0,0),并将显示属性恢复正常
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrClrs"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr5
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr5(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	EM_TEXT_ATTRIBUTE OldAttr = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//case1:测试NDK_ScrClrs
	NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE,&OldAttr);
	NDK_ScrPrintf("清屏前\n");
	NDK_ScrRefresh();
	show_msg1(MAXWAITTIME, "按任意键清屏后,下条信息从(0,0)处正常显示");
	if((ret=NDK_ScrClrs())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	nKeyin = show_msg1(MAXWAITTIME, "(0,0)处正常显示,按确认测试通过,其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	
	return;
}

