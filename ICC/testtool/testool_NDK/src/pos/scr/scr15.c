/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 15.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrSetContrast能否设置屏幕对比度
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrSetContrast"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr15
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
#if OVERSEAS_OS70_ENABLE  //海外产品7.0版本开始彩屏支持设置对比度,且不需要在root权限下  20180425 modify
void scr15(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0, i = 0;
	uint puncd = 0;
		
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置:判断液晶屏类型
	if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//测试前置:保存系统设置
	//这里要保存默认的对比度，如果无法用函数获取默认对比度，至少我们要找开发确认对比度是多少，在退出程序的时候设置成这个确认的默认值
	if(puncd == 16)
	{
		//case1:参数异常:unContrast为0,21
		if((ret=NDK_ScrSetContrast(0))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		if((ret=NDK_ScrSetContrast(21))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case2:测试NDK_ScrSetContrast(分别设置1-20),观看效果,人工判断
		cls_show_msg1(5,"耐心等待10秒看屏幕变化,设置屏幕对比度由浅变深,最后在设置成正常,请观看效果");
		for(i=1;i<=20;i++)
		{
			if((ret=NDK_ScrSetContrast(i))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
				goto ERR;
			}		
			NDK_SysDelay(5);
		}	
		NDK_ScrSetContrast(12);
	}
	else if(puncd ==1 || puncd == 4)
	{
		//case1:参数异常:unContrast为-1,64
		if((ret=NDK_ScrSetContrast(-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		if((ret=NDK_ScrSetContrast(64))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case2:测试NDK_ScrSetContrast(分别设置0-63),观看效果,人工判断
		cls_show_msg1(5,"耐心等待30秒看屏幕变化,设置屏幕对比度由浅变深,最后在设置成正常,请观看效果");
		for(i=0;i<=63;i++)
		{
			if((ret=NDK_ScrSetContrast(i))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}		
			NDK_SysDelay(5);
		}	
		NDK_ScrSetContrast(32);
	}
	else
	{
		send_result("line %d:%s测试失败:屏幕非彩屏或黑白屏(puncd = %d)", __LINE__, TESTAPI, puncd);
		GOTOERR;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "[ENTER]通过,[其它]不通过");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
			send_result("%s测试失败", TESTAPI);
	goto ERR;

	//恢复默认设置
ERR:
	if(puncd == 16)	
		NDK_ScrSetContrast(12);
	else
		NDK_ScrSetContrast(32);
	return;
}
#else
void scr15(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint i = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	if(cls_show_msg1(5, "此用例需要在root权限下进行,[Enter]继续,其他返回" )!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}

	//测试前置:保存系统设置
	//这里要保存默认的对比度，如果无法用函数获取默认对比度，至少我们要找开发确认对比度是多少，在退出程序的时候设置成这个确认的默认值
	
	//case1:参数异常:unContrast为-1,64
	if((ret=NDK_ScrSetContrast(-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_ScrSetContrast(64))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:测试NDK_ScrSetContrast(分别设置0-63),观看效果,人工判断
	cls_show_msg1(5,"耐心等待30秒看屏幕变化,设置屏幕对比度由浅变深,最后在设置成正常,请观看效果");
#if defined ME30S  //林生欣说ME30S的有效参数是从1-63,不做修改,故这里加条件编译 20180517 linying modify
	i=1;
#else 
	i=0;
#endif
	for(i;i<=63;i++)
	{
		if((ret=NDK_ScrSetContrast(i))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			goto ERR;
		}		
		NDK_SysDelay(5);
	}	
	NDK_ScrSetContrast(32);
	nKeyin = cls_show_msg1(MAXWAITTIME, "[ENTER]通过,[其它]不通过");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);

	//恢复默认设置
ERR:
	//默认值为32
#if !(defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME30GX||defined ME32THM||defined ME31||defined SP10)
	NDK_ScrSetContrast(32);
#else
	NDK_ScrSetContrast(19);
#endif
	return;
}
#endif

