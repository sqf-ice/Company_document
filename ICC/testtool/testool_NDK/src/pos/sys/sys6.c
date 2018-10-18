/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys6.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysBeep接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysBeep"
#define	MAXWAITTIME 30

/*------------global variables definition-----------------------*/

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
*			  		        linwei	           20130121	 	created
*****************************************************************/
void sys6(void)
{
	if(auto_flag==1)
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTAPI);
	//以下为局部变量
	int i = 0;
	int nKeyin = ENTER;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置
#if !K21_ENABLE
	NDK_SysSetBeepVol(5);
#endif

	//case 1:连续响10声
	for (i=0; i<10; i++)
	{
		NDK_SysBeep();
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "听到响声,[确认]通过,[其它]失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

