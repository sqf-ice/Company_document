/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 1.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrGotoxy 是否可以正确移动到第（y+1）行第（x+1）列处，测试NDK_ScrGetxy是否可以正确取光标位置的列号和行号
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define	 MAXTIMEOUT	10
#define 	TESTAPI		"NDK_ScrGotoxy,NDK_ScrGetxy"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr1
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr1(void)
{
	/*private & local definition*/
	uint PosX = 0, PosY = 0;
	int nKeyin = ENTER, ret = 0;
	uint getx = 0, gety = 0, ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	lib_kbflush();		//清除按键缓冲区
	//为实现自动化测试，启动计数器
	lib_StartStopwatch();
	while ((nKeyin=lib_kbhit())==0)
	{
		PosX = rand()%(ViewWidth-(GUI_GetFontWidth()/2));
		PosY = rand()%(ViewHeight-GUI_GetFontHeight());
		//NDK_ScrGotoxy :  0<= x <=(LCDCOL-1) ;0<= y <= (LCDROW-1) (单位：CHAR)
		cls_show_msg1(3, "光标将移动到(%d,%d),按任意键继续...", PosX, PosY);
		
		//case1:测试scrgotoxy正常移动
		NDK_ScrClrs();
		if((ret=NDK_ScrGotoxy(PosX, PosY))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}	
		
		//case2:测试scrgetxy获取到的值与设置的值一致
		if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if (getx!=PosX || gety!= PosY)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, getx, gety);
			RETURN;
		}
		NDK_ScrPrintf("|光标在这!");
		NDK_SysBeep();
		NDK_ScrRefresh();
		NDK_SysDelay(20);
		cls_printf("测试%s...按确认测试通过,其它键失败", TESTAPI);
		NDK_SysDelay(20);
		if (MAXWAITTIME<lib_ReadStopwatch())
			break;
	}

	if (nKeyin==ENTER || nKeyin==0)
		;
	else
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:不应走到状态栏(730追加测试)
#if  0 //STATUSBAR_ENABLE

	if((ret=NDK_ScrGotoxy(0, 18))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(18==gety)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	if((ret=NDK_ScrGotoxy(0, 19))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(19==gety)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif
	//后续用例的测试前置
	PosX = rand()%ViewWidth;
	PosY = rand()%ViewHeight;
	if((ret=NDK_ScrGotoxy(PosX, PosY))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:参数异常测试。文档说：如果输入参数非法，则光标保持当前位置不变。
	// 即,下面三句是不起作用的。
	//case4.1:x参数异常
	if((ret=NDK_ScrGotoxy(ViewWidth, 0))!=NDK_ERR_PARA)//!=错误返回值
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(ViewWidth==getx || getx!=PosX|| gety!= PosY)
	{
		send_result("line %d:%s测试失败%d,(%d,%d)(%d,%d)", __LINE__, TESTAPI, ViewWidth, getx, gety, PosX, PosY);
		RETURN;
	}

	//case4.2:y参数异常
	if((ret=NDK_ScrGotoxy(0, ViewHeight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(ViewHeight==gety || getx!=PosX || gety!= PosY)
	{
		send_result("line %d:%s测试失败%d,(%d,%d)(%d,%d)", __LINE__, TESTAPI, ViewHeight, getx, gety, PosX, PosY);
		RETURN;
	}
	
	//case4.3:x,y参数异常
	if((ret=NDK_ScrGotoxy(ViewWidth, ViewHeight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(ViewWidth==getx || ViewHeight==gety || getx!=PosX || gety!= PosY)
	{
		send_result("line %d:%s测试失败(%d,%d)(%d,%d)(%d,%d)", __LINE__, TESTAPI, ViewWidth, ViewHeight, getx, gety, PosX, PosY);
		RETURN;
	}

	//结束测试
	send_result("%s测试通过", TESTAPI);
	return;
}

