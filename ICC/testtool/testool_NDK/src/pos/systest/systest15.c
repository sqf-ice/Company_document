/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: systest15.c
* Author 			: chensj
* version			: V1.0
* DATE			: 2013/02/21
* directory 		: 
* description		: 磁卡/打印交叉
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2013/02/21 chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	UCID			(13)
#define	TESTITEM		"磁卡/打印交叉"

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static void systest15_Card(void)
{
	/*private & local definition*/

	/*process body*/
	MagcardReadTest(TK2_3, FALSE, 0);
}


/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					chensj		2013/02/21		created
*****************************************************************/
//编写用例注意：交叉测试要做到尽可能深度交叉!另外,也要有一定的强度。
void systest15(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}
	/*private & local definition*/
	
	/*process body*/
	cls_show_msg("%s测试,请在打印时连续刷卡,并注意打印效果", TESTITEM);
	prn_press(TESTITEM, systest15_Card);
	return;
}

