/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: scr25.c
* Author 			: chensj
* version			: 
* DATE			: 20130722
* directory 		: 
* description		: 测试 NDK_ScrPrintf接口功能
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		 "NDK_ScrPrintf"
#define	MAXWAITTIME	10//30.0

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr25
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	   chensj          20130722  	     created
*
*****************************************************************/
void scr25(void)
{
	/*private & local definition*/
	int ret = 0, i = 999;
	char c = 'a';
	char temp[4] = {0};
	char str1[] = {
		"①自动流程②老化"
	   "③整机④检测记录"
	   "⑤出货整理⑥硬件"
	   "⑦版本⑧蓝牙测试"
	};
#if defined ME68 //ME68的大小限制不能超过511字节，否则会溢出 20180416 sull modify
	char BigStr[511+1] = {0};
#else
	char BigStr[512+1] = {0};
#endif
	float f = 0.0;
	char *p=NULL;
	uint ViewWidth=0 ,ViewHeight=0;
#if K21_ENABLE
	char str[32] = {0};
#endif		
	/*process body*/
	cls_show_msg1(2,"测试%s...", TESTAPI); 

	//case 1:格式化输出%s但不给值,不应该显示随机数据
	if((ret = NDK_ScrPrintf("\n不给值的%s"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	sleep(1);
	if((ret = NDK_ScrPrintf("\n给空指针%s",p))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	sleep(1);
	if((ret = NDK_ScrPrintf("\n直接赋值空%s",NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	sleep(1);
	
	//case 2:异常参数测试
	if((ret = NDK_ScrPrintf(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case 3:汉字在边界位置不应该显示半个汉字
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	NDK_ScrClrs();
	NDK_ScrGotoxy(ViewWidth-(GUI_GetFontWidth()/2),0);
	NDK_ScrPrintf("国");
	NDK_ScrGotoxy(0,ViewHeight-(GUI_GetFontHeight()/2));
	NDK_ScrPrintf("国");
	NDK_ScrRefresh();
	NDK_SysDelay(10);//等待以确认是否有屏幕输出
	if(cls_show_msg1(MAXWAITTIME, "屏幕不出现半个国字,按[确认];否则按其它键.")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示 

	//case 4:mNDK文档上说明,针对ME30字符串不能超过511字节,故增加此用例超过511字节不应跑飞 20140219新增,其他产品也不应该跑飞
	NDK_ScrClrs();
	memset(BigStr,'a',sizeof(BigStr)-1);
	if((ret = NDK_ScrPrintf("%s", BigStr))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	sleep(3);
	if(cls_show_msg1(MAXWAITTIME, "屏幕显示小a? \n[确认]是  [其他]否")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case 6:测试字符(不含换行符)多次显示后超过液晶宽度是否会换行//根据工程部反馈20180521 linying added
	NDK_ScrClrs();
	for (i=0; i<strlen(str1); i += 2) {
		temp[0] = str1[i];
		temp[1] = str1[i + 1];
		NDK_ScrPrintf("%s", temp);
	}
	NDK_ScrRefresh();
	sleep(3);
	if(cls_show_msg1(MAXWAITTIME, "是否正常显示四行菜单项? \n[确认]是  [其他]否")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case7:测试多行一次性显示时，是否会换行//20180521 linying added
	NDK_ScrClrs();
	NDK_ScrPrintf("%s", str1);
	NDK_ScrRefresh();
	sleep(3);
	if(cls_show_msg1(MAXWAITTIME, "是否正常显示四行菜单项? \n[确认]是  [其他]否")!=ENTER&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case 5:正常参数测试
	NDK_ScrClrs();
#if !K21_ENABLE
	if((ret = NDK_ScrPrintf("正常测试字符串:%s;整型:%d;字符:%c;浮点:%f\n", "AB串", i, c, f))!=NDK_OK)
#else
	memset(str,0,sizeof(str));
	ftos(f,str);
	if((ret = NDK_ScrPrintf("正常测试字符串:%s;整型:%d;字符:%c;浮点:%s\n", "AB串", i, c, str))!=NDK_OK)
#endif
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//测试结束
	ret = show_msg1(MAXWAITTIME, "[ENTER]通过,[其它]不通过");
	if (ret==ENTER || ret==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

