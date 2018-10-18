/************************************************************************
* copyright(c)2005-2016 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name			: 32.c
* Author 			: zhengry
* version			: 
* DATE				: 20160511
* directory 			: 
* description			: NDK_LedSetBright数码管设置亮度
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_LedSetBright"
#define MAXWAITTIME  15

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scr32
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					    zhengry		    20160511		created
*****************************************************************/
void scr32(void)
{
	/*private & local definition*/
	int ret=0, i=0, nKeyin=0;
	char buf[7]="888888";
	
	/*process body*/
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_LedClr();

	//case1:异常参数测试:rate=-1/8时
	if((ret=NDK_LedSetBright(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LedSetBright(8)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:正常测试:依次设置亮度为0~7的时候，亮度应该依次变亮
	for(i=0; i<8; i++)
	{
		if((ret=NDK_LedSetBright(i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		if((ret=NDK_LedDisp(0, buf)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysDelay(10);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管依次变亮.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_LedSetBright(3);
	NDK_LedClr();
	return;
}

