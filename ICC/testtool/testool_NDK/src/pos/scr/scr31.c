/************************************************************************
* copyright(c)2005-2016 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name			: 31.c
* Author 			: zhengry
* version			: 
* DATE				: 20160511
* directory 			: 
* description			: 测试NDK_LedDisp数码管显示和NDK_LedClr数码管全灭
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_LedDisp,NDK_LedClr"
#define MAXWAITTIME  15

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct _LEDDISP_ALG
{
	uint justify;
	char *digits;
}LEDDISP_ALG;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scr31
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					    zhengry		    20160511		created
*****************************************************************/
void scr31(void)
{
	/*private & local definition*/
	int ret=0, i=0, nKeyin=0;
	LEDDISP_ALG abnormal[] ={
		{0, NULL},
		//{0, "12AB"}, //底层无法判断显示内容是否正常故不作为异常参数测试20180425 modify
		{-1, "123"},
		{2, "123"},
	};
	LEDDISP_ALG normal[] = {
		{0, " . . . . . "},
		{0, "_._._._._._"},
		{0, "-.-.-.-.-.-"},
		{0, "0.0.0.0.0.0"},
		{0, "1.1.1.1.1.1"},
		{0, "2.2.2.2.2.2"},
		{1, "3.3.3.3.3.3"},
		{1, "4.4.4.4.4.4"},
		{1, "5.5.5.5.5.5"},
		{1, "6.6.6.6.6.6"},
		{1, "7.7.7.7.7.7"},
		{1, "8.8.8.8.8.8"},
		{1, "9.9.9.9.9.9"},
	};
	
	/*process body*/
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_LedClr();
	if((ret=NDK_LedSetBright(3)) != NDK_OK)//设置正常亮度
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1:异常参数测试:justify=-1/2时
	for(i=0; i<SZ_ARRAY(abnormal); i++)
	{
		if((ret=NDK_LedDisp(abnormal[i].justify, abnormal[i].digits)) != NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysMsDelay(100);
	}	
	
	//case2:正常测试:justify=0 左对齐
	cls_printf("测试左对齐中...");
	if((ret=NDK_LedDisp(0, "1234")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管上按左对齐显示1234.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case3:正常测试:justify=1 右对齐
	cls_printf("测试右对齐中...");
	if((ret=NDK_LedDisp(1, "1234")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管上按右对齐显示1234.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case4:正常测试:能正常遍历
	for(i=0; i<SZ_ARRAY(normal); i++)
	{
		if((ret=NDK_LedDisp(normal[i].justify, normal[i].digits)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管上按显示:%s  是[ENTER],否则[其他].", normal[i].digits);
		if( nKeyin != ENTER &&	nKeyin != 0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
			GOTOERR;
		}
	}

	//case5:正常测试:大于6个数字，应能正常显示	
	cls_printf("测试字符串大于6个数字...");
	if((ret=NDK_LedDisp(0, "1234567")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管上显示123456  是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case6:正常测试:调用后,数码管应全部灭掉
	if((ret=NDK_LedClr())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "原先亮着的led数码管全部灭掉.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_LedClr();
	return;
}
