/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 19.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrDispString是否在指定区域内显示字符串
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrDispString"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr19
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr19(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint  x = 0, y = 0;
	const char pszS1[] = "NEWLAND\r\nPOS";	
	const char pszS2[] = "新大陆单人旁月:(仴)(仡)\r\nPOS";//20130925 新增GB2312字库生僻字：仡 和 GB18030字库生僻字：仴 的显示测试
#if !K21_ENABLE
	uint  puncd = 0;
	const char str[] = "newland电脑";
	char stroverbuf[8192] = {0};
#else
	char stroverbuf[4096] = {0};//K21底层堆栈较小
#endif
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	//case1:参数异常:x,y为-1;unMode为-1,2
	if((ret=NDK_ScrDispString(-1,y,pszS1,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	NDK_ScrRefresh();
	NDK_SysDelay(10);
	if((ret=NDK_ScrDispString(x,-1,pszS1,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(10);
	if((ret=NDK_ScrDispString(x,y,pszS1,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(10);
#if !K21_ENABLE //K21不支持unMode设置为1，所以应该返回参数错误
	if((ret=NDK_ScrDispString(x,y,pszS1,2))!=NDK_ERR_PARA)
#else
	if((ret=NDK_ScrDispString(x,y,pszS1,1))!=NDK_ERR_PARA)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(10);

	//case11:验证输入字符串大于底层缓冲情况下,程序不应该跑飞,显示缓冲部分,超出部分丢弃 20131128 linwl
	memset(stroverbuf,'A',sizeof(stroverbuf)-1);
	if((ret=NDK_ScrDispString(0,0,stroverbuf,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	if(cls_show_msg1(MAXWAITTIME, "全屏幕显示字符A,按确认;否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case2:添加显示字符串为NULL
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(x,y,NULL,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);//等待以确认是否有屏幕输出
	if(cls_show_msg1(MAXWAITTIME, "若屏幕没有出现字符串,按确认;否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:添加显示坐标越界的测试
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(ViewWidth,ViewHeight,pszS1,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);
	if(cls_show_msg1(MAXWAITTIME, "若屏幕没有出现\"%s\",按[确认];否则按其它键.", pszS1)!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示
	
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(ViewWidth,ViewHeight,pszS2,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);//sleep(2);
	if(cls_show_msg1(MAXWAITTIME, "若屏幕没有出现\"%s\",按[确认];否则按其它键.", pszS2)!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示

#if !K21_ENABLE		//K21不支持unMode设置为1的情况，不做正常测试
	//case4:unMode设置为1
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(x,y,pszS1,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);//等待以确认是否有屏幕输出
	if(cls_show_msg1(MAXWAITTIME, "若屏幕出现\"%s\",按[确认];否则按其它键.", pszS1)!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示

	//case5:不能输出汉字
	if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(x,y,str,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(20);//等待以确认是否有屏幕输出

	if(puncd==1||puncd==4) //普屏模式为1的时候不能输出汉字
	{	
		if(cls_show_msg1(MAXWAITTIME, "若屏幕没有出现完整\"电脑\",按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
	}
	else //彩屏输出汉字
	{	
		if(cls_show_msg1(MAXWAITTIME, "若屏幕出现%s,按[确认];否则按其它键.",str)!=ENTER&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示
#endif

	//不应走到状态栏(730追加测试)
#if 0//STATUSBAR_ENABLE
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(0,GUI_GetLCDHeight(),pszS1,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(20);//sleep(2);
	if(cls_show_msg1(MAXWAITTIME, "若状态栏出现\"%s\",按确认;否则按其它键.", str)==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}	
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示
#endif
	
	//case6:unMode设置为0
	NDK_ScrClrs(); 
	if((ret=NDK_ScrDispString(x,y,pszS2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(10);//等待以确认是否有屏幕输出
	if(cls_show_msg1(MAXWAITTIME, "若屏幕出现\"%s\",按[确认];否则按其它键.", pszS2)!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case7:POS上的\r是回车效果,\n是回车并换行的效果
	NDK_ScrClrs();
	if((ret=NDK_ScrDispString(ViewWidth/2,0,"\r",0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);//等待以确认是否有屏幕输出
	if(show_msg1(MAXWAITTIME, "若本行在第一行顶格显示,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	NDK_ScrClrs();
	if((ret=NDK_ScrDispString(ViewWidth/2,0,"\n",0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);//等待以确认是否有屏幕输出
	if(show_msg1(MAXWAITTIME, "若本行在第二行顶格显示,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示 

	//case8:汉字在边界位置不应该显示半个汉字
	NDK_ScrClrs();
	NDK_ScrDispString(ViewWidth-(GUI_GetFontWidth()/2),0,"国",0);
	NDK_ScrDispString(0,ViewHeight-(GUI_GetFontHeight()/2),"国",0);
	NDK_ScrRefresh();
	NDK_SysDelay(10);//等待以确认是否有屏幕输出
	if(cls_show_msg1(MAXWAITTIME, "屏幕不出现半个国字,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示 
	
	//case 9:汉字在非双字符位置显示应正常
	NDK_ScrClrs();
	NDK_ScrDispString(8,0,"本信息起始位置(8,0)开始显示",0);
	if(show_msg1(MAXWAITTIME, ",按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");
	
	//case10:任意输出字符
	lib_kbflush();		//清除按键缓冲区
	lib_StartStopwatch();//为实现自动化测试，启动计数器
	while ((nKeyin=lib_kbhit())==0)
	{
		x = rand()%(ViewWidth-(GUI_GetFontWidth()/2));
		y = rand()%(ViewHeight-GUI_GetFontHeight());
		cls_show_msg1(1, "将在(%d, %d)显示\"%s\".测试正常,可按ENTER;有异常,请按其它键", x, y, pszS2);
		NDK_ScrClrs(); 
		if((ret=NDK_ScrDispString(x,y,pszS2,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(20);
		if (30<lib_ReadStopwatch())
			break;
	}
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);

	return;
}

