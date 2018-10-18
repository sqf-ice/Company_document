/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name			: 8.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_PS2Start,NDK_PS2Stop
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_PS2Start,NDK_PS2Stop"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: kb8
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 		:
* history 		 			: author			date			remarks
*			  	  	       
*****************************************************************/
void kb8(void)
{
	/*private & local definition*/
	int ret = 0 ,cnt = 0, key = 0;
	int nKeyin = ENTER;

#include "kbtable.h"
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	cls_show_msg("请将POS机与PS2键盘相连，按任意键继续测试");
	
	//case1:正常测试start后可以读取外接键盘键值
	if ((ret=NDK_PS2Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	 cnt = rand()%(SZ_ARRAY(ps2_num));
	 cls_printf("请在20s内按下键盘左边的%s键,NDK_PS2GetCode(20)将捕获它\n", ps2_num[cnt].keyname);
	 if((ret = NDK_PS2GetCode(20,&key)) != NDK_OK)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d)", __LINE__, ret);
		 GOTOERR;
	 }
	if(key!= ps2_num[cnt].keycode)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(0x%02x)", __LINE__, key);
		 GOTOERR;
	}
	if ((ret=NDK_PS2Stop())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	
	//case2:多次start应该返回成功
	if ((ret=NDK_PS2Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	if ((ret=NDK_PS2Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	if ((ret=NDK_PS2Stop())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	
	//case3:stop后按键不应获取到键值
	cnt = rand()%(SZ_ARRAY(ps2_num));
	cls_printf("请在20s内按下键盘左边的%s键,应不该被捕获到\n", ps2_num[cnt].keyname);
	if ((ret=NDK_PS2GetCode(20,&key))!=NDK_OK || key!=0)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI,ret,key);
		RETURN;
	}

	//case4:多次stop预期返回成功
	if ((ret=NDK_PS2Stop())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	if ((ret=NDK_PS2Stop())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

