/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: systest38.c
* Author 			: chensj
* version			: V1.0
* DATE			: 20130225
* directory 		: 
* description		: 打印/键盘交叉
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 1.0  20130225 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"打印/键盘交叉"

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					chensj		20130225       	created
*****************************************************************/
//编写用例注意：交叉测试要做到尽可能深度交叉!另外,也要有一定的强度。
void systest38(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	
	/*process body*/
	cls_show_msg("%s测试,请在打印时连续按键,并注意打印效果", TESTITEM);
	conf_prn();
	prn_press(TESTITEM, NULL);
	return;
}

