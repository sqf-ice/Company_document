/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name		: 2.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_KbSwitch,NDK_KbGetSwitch
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_KbSwitch,NDK_KbGetSwitch"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: kb2
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*****************************************************************/
void kb2(void)
{
	/*private & local definition*/
	int ret = 0 ,cnt = 0;
	int nMode = 0, nMode2 = 0;
	int pnState = 0;	
	int pnState2 = 0;
	int nKeyin = ENTER;

#include "kbtable.h"
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常:nSelect=-1,2;nMode=-1,2;pnState=NULL
	if ((ret=NDK_KbSwitch(-1,0,&pnState))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_KbSwitch(1,-1,&pnState))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if ((ret=NDK_KbSwitch(1,0,NULL))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if ((ret=NDK_KbGetSwitch(-1,&pnState2))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN; 
	}
	if ((ret=NDK_KbGetSwitch(nMode,NULL))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}


	//case2:测试NDK_KbSwitch的nSelect为0,默认关闭可以通过长按键的返回键码正常键码来验证	
	if ((ret=NDK_KbSwitch(0,0,&pnState))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//subcase1:测试了NDK_KbHit
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键(1秒以上),NDK_KbHit将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = wait_anykey(MAXWAITTIME);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}
	//subcase2:测试了NDK_KbGetCode(30)
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键(1秒以上),NDK_KbGetCode(30)将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = lib_getkeycode(MAXWAITTIME);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}
	
	//subcase3:测试了NDK_KbGetCode(0)
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键(1秒以上),NDK_KbGetCode(0)将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = lib_getkeycode(0);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case3:测试NDK_KbGetSwitch,并验证
	if ((ret=NDK_KbGetSwitch(nMode2,&pnState2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (pnState2!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pnState2);
		RETURN;
	}

	//case4:测试NDK_KbSwitch的nSelect为1,并设置长按键打开
	if ((ret=NDK_KbSwitch(1,0,&pnState))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//subcase1:测试了NDK_KbHit可以获取到长按键
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键(1秒以上),NDK_KbHit将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = wait_anykey(MAXWAITTIME);
	if (nKeyin!=(table[cnt].keycode+0x4000) && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}
	
	//subcase2:测试了NDK_KbGetCode(30)可以获取到长按键	
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键(1秒以上),NDK_KbGetCode(30)将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = lib_getkeycode(MAXWAITTIME);
	if (nKeyin!=(table[cnt].keycode+0x4000) && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}
	//subcase3:测试了NDK_KbGetCode(0)可以获取到长按键
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请按下%s键(1秒以上),NDK_KbGetCode(0)将捕获它\n", table[cnt].keyname);	
	nKeyin = lib_getkeycode(0);
	if (nKeyin!=(table[cnt].keycode+0x4000) && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}
	cls_printf("请按下F1键(1秒以上),NDK_KbGetCode(0)将捕获它\n");	
	nKeyin = lib_getkeycode(0);
	if (nKeyin!=KEY_F1 && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}


	//case5:测试NDK_KbGetSwitch,并验证	
	if ((ret=NDK_KbGetSwitch(nMode2,&pnState2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (nMode2!=0||pnState2!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, nMode2, pnState);
		RETURN;
	}
	
	//case6::测试NDK_KbSwitch的nSelect为1,并设置组合键打开
	if ((ret=NDK_KbSwitch(0,1,&pnState))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//subcase1:测试了NDK_KbHit可以获取到组合键
	//subcase2:测试了NDK_KbGetCode(30)可以获取到组合键
	
	//subcase3:测试了NDK_KbGetCode(0)可以获取到组合键
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请按组合键F1+%s键\n", table[cnt].keyname);
	nKeyin = lib_getkeycode(0);
	if (nKeyin!=(table[cnt].keycode+0x8000) && nKeyin!=0)
	{
		send_result("line %d: %s测试失败(%04x)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case7:测试NDK_KbGetSwitch,并验证
	if ((ret=NDK_KbGetSwitch(nMode2,&pnState2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (nMode2!=1 || pnState2!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, nMode2, pnState);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
	
ERR:
	NDK_KbSwitch(1, 0, &pnState);
	return;
}

