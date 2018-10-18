/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 13.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrGetLcdSize能否获取液晶屏尺寸,NDK_ScrGetColorDepth能否获取液晶屏色深
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrGetLcdSize,NDK_ScrGetColorDepth"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr13
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr13(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punWidth = 0, punHeight = 0;
#if !K21_ENABLE
	int nKeyin = ENTER;
	uint puncd = 0;
#endif
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常:punWidth,punHeight,puncd分别为NULL
	if((ret=NDK_ScrGetLcdSize(NULL,&punHeight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetLcdSize(&punWidth,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if !K21_ENABLE //K21 不支持NDK_ScrGetColorDepth函数
	if((ret=NDK_ScrGetColorDepth(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	//case2:获取液晶屏尺寸
	if((ret=NDK_ScrGetLcdSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("该机屏幕大小为宽:%d,高:%d,请人工判断是否测试通过", punWidth, punHeight);

#if !K21_ENABLE
	//case3:获取液晶屏色深,人工判断	
	if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(puncd!=1&&puncd!=4&&puncd!=16)//返回液晶屏色深：1----黑白两色, 16----16位色，彩屏  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, puncd);
		RETURN;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "该机为%s屏,按[确认]测试通过,[其它]失败",(puncd!=16?"黑白":"彩"));
	if (nKeyin==ENTER || nKeyin==0)
		;
	else
		send_result("%s测试失败", TESTAPI);
#endif	
	send_result("%s测试通过", TESTAPI);
	return;
}

