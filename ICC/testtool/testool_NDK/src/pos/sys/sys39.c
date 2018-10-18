/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 系统
* file name			: 39.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_SysBeepVoiceCtrl是否可以正常设置按键音大小(低端支持)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_SysBeepVoiceCtrl"   
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: voice2
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					      sull		      20180115		    created
*
*****************************************************************/
void sys39(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, nKeyin = 0;


	/*process body*/	
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数非法
	if((ret = NDK_SysBeepVoiceCtrl(0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SysBeepVoiceCtrl(11)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
		GOTOERR;
	}

	//case2:设置音量为1-10应该成功，且按键音量应逐渐增大
	for(i=1; i<=10; i++)
	{
		if((ret = NDK_SysBeepVoiceCtrl(i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		cls_show_msg1(MAXWAITTIME,"请按任意键，按键音量应为%d", i);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"按键的音量是否逐渐增大?是[ENTER],否则[其它]");
	if(nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//结束测试	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SysBeepVoiceCtrl(10);//默认值为10
	return;
}

