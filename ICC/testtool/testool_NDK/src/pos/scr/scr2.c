/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 2.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrSetAttr是否可以正确取原来的显示属性，并设置新的显示属性
* related document	: NDK.chm
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	MAXWAITTIME 10//30
#define	TESTAPI		"NDK_ScrSetAttr"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr2
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr2(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	EM_TEXT_ATTRIBUTE OldAttr[3] = {0};
	
	/*process body*/
	nKeyin = cls_show_msg1(MAXWAITTIME,"测试%s.请选择显示字体:1.正常,2.扩展", TESTAPI);//屏幕提示当前测试所在模块与用例

	switch(nKeyin)
	{
		case '1':
			NDK_ScrSetFontType(DISPFONT_CUSTOM);
			break;
		case '2':
			NDK_ScrSetFontType(DISPFONT_EXTRA);
			break;
		default:
			NDK_ScrSetFontType(DISPFONT_CUSTOM);
			break;
	}
	
	//测试前置：设置正常显示模式，并获取原来的显示模式应该成功
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL,OldAttr))!=NDK_OK)//预置为正显
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME,"文本显示属性为正常模式,按确认测试通过，其它键失败\n");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case1:测试设置合法显示模式,会返回旧的显示模式(3种模式)
	//反显模式
	NDK_ScrClrs();
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE,OldAttr+1))!=NDK_OK)//预置为反显
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = show_msg1(MAXWAITTIME,"文本显示属性为反显模式,按确认测试通过，其它键失败\n");
	if((OldAttr[1]!=TEXT_ATTRIBUTE_NORMAL)||(ENTER!=nKeyin&&auto_flag!=1))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, OldAttr[1]);
		GOTOERR;
	}
	
	//下划线模式
#if !K21_ENABLE //K21平台不支持下划线功能
	NDK_ScrClrs();
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE,OldAttr+1))!=NDK_OK)//预置为反显
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_UNDERLINE,OldAttr+1))!=NDK_OK)//预置为字体加下划线
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = show_msg1(MAXWAITTIME,"文本显示属性为反显带下划线模式,按确认测试通过，其它键失败\n");
	if((OldAttr[1]!=TEXT_ATTRIBUTE_REVERSE)||(ENTER!=nKeyin&&auto_flag!=1))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, OldAttr[1]);
		GOTOERR;
	}
#endif
	//正显模式
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL,OldAttr+1))!=NDK_OK)//预置为正显
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = show_msg1(MAXWAITTIME,"文本显示属性为正常模式,按确认测试通过，其它键失败\n");
#if !K21_ENABLE
	if((OldAttr[1]!=(TEXT_ATTRIBUTE_UNDERLINE|TEXT_ATTRIBUTE_REVERSE))||(ENTER!=nKeyin&&auto_flag!=1))
#else
	if((OldAttr[1]!=(TEXT_ATTRIBUTE_REVERSE))||(ENTER!=nKeyin&&auto_flag!=1))
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, OldAttr[1]);
		GOTOERR;
	}
	
	//case2:测试原模式为NULL时，应不返回值
#if !K21_ENABLE
	NDK_ScrClrs();
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_UNDERLINE,NULL))!=NDK_OK)//原模式为NULL
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = show_msg1(MAXWAITTIME,"文本显示属性为下划线模式,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
	//case3:测试非法参数-1,应返回失败
	if((ret=NDK_ScrSetAttr(-1,OldAttr+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:测试NDK_ScrClrs后恢复TEXT_ATTRIBUTE_NORMAL模式
	NDK_ScrClrs();//注意:NDK_ScrClrs后，会恢复TEXT_ATTRIBUTE_NORMAL，且把光标移到(0,0)，所以必须要放在set之前
	if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE,OldAttr+2))!=NDK_OK)//预置为正显
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(OldAttr[2]!=TEXT_ATTRIBUTE_NORMAL)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, OldAttr[2]);
		goto ERR;
	}
	nKeyin = show_msg1(MAXWAITTIME,"文本显示属性为反显模式,按确认测试通过，其它键失败\n");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	NDK_ScrSetAttr(OldAttr[2],OldAttr+1);
	send_result("%s测试通过", TESTAPI);
	//恢复默认值
ERR:
	NDK_ScrSetFontType(DISPFONT_CUSTOM);
	NDK_ScrSetAttr(OldAttr[0],OldAttr+1);
	return;
}

