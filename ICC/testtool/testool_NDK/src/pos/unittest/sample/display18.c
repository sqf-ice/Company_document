/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 18.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20070423
* directory 		: .\SRC\display\
* description		: 测试TurnOnLCD/TurnOffLCD能否打开/关闭LCD背光
* related document	: 
*
************************************************************************
* log			: 
* Revision 1.0  20070423 11:10 zhangnw
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  UCID  18
#define  MAXWAITTIME (30)
#define 	TESTAPI		"TurnOnLCD/TurnOffLCD"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: display18
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20070423 	 	created
*****************************************************************/
void display18(void)
{
	/*private & local definition*/
	int nKeyin = 0;
		
	/*process body*/
	//cls_printf("TurnOnLCD/TurnOffLCD测试中...请看效果!");

	cls_show_msg1(MAXWAITTIME, "%d秒后将打开背光,请看效果!", MAXWAITTIME);
	TurnOnLCD();

	cls_show_msg1(MAXWAITTIME, "%d秒后将关闭背光,请看效果!", MAXWAITTIME);
	TurnOffLCD();
	
	nKeyin = cls_show_msg1(MAXWAITTIME, "根据效果,按确认通过,其它键则不通过");
	if(nKeyin==ENTER||nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
}

