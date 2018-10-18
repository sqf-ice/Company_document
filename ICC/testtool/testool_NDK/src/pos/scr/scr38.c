/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 38.c
* Author 			:
* version			: 
* DATE			: 
* directory 		: 
* description		: 测试NDK_ScrDispMode是否可以正确翻转屏幕
* related document	: NDK.chm
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  HEIGHT_HLJNX 51
#define MAXWAITTIME  10
#define	TESTAPI		 "NDK_ScrDispMode"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr38
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	       sull            20180112  	    created
*
*****************************************************************/
void scr38(void)
{
	/*private & local definition*/
	int ret = -1, nKeyin = ENTER;
	// 黑龙江农信广告图(128*51)
	char sCUPBMP51x48[] =
	{
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X20,0X04,0X00,0X00,0X00,0X00,
	0X60,0X06,0X00,0X00,0X00,0X00,0XC7,0XE3,0X00,0X00,0X00,0X03,0X8F,0XF3,0X80,0X00,
	0X00,0X07,0X1F,0XF9,0XC0,0X00,0X00,0X0E,0X1F,0XF8,0XE0,0X00,0X00,0X1E,0X1E,0X00,
	0X70,0X00,0X00,0X3C,0X3C,0X00,0X78,0X00,0X00,0X38,0X3C,0X38,0X3C,0X00,0X00,0X78,
	0X3E,0X78,0X3E,0X00,0X00,0XF8,0X1F,0XF8,0X3E,0X00,0X00,0XF8,0X1F,0XF8,0X3F,0X00,
	0X01,0XFC,0X0F,0XF0,0X7F,0X80,0X03,0XFE,0X07,0XC0,0XFF,0X80,0X07,0XFF,0X00,0X01,
	0XFF,0XC0,0X07,0XFF,0X80,0X03,0XFF,0XE0,0X07,0XFF,0XC0,0X07,0XFF,0XE0,0X07,0XFF,
	0XE0,0X0F,0XFF,0XC0,0X03,0XFF,0XF0,0X1F,0XFF,0X80,0X01,0XFF,0XF8,0X3F,0XFF,0X00,
	0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X3F,0XFE,0XFF,
	0XF8,0X00,0X00,0X1F,0XFC,0X7F,0XF0,0X00,0X00,0X0F,0XFC,0X7F,0XF0,0X00,0X00,0X0F,
	0XF8,0X3F,0XF0,0X00,0X00,0X0F,0XF8,0X1F,0XF0,0X00,0X00,0X0F,0XF8,0X1F,0XF0,0X00,
	0X00,0X1F,0XF0,0X0F,0XF8,0X00,0X00,0X3F,0XE0,0X07,0XFC,0X00,0X00,0XFF,0XC0,0X07,
	0XFE,0X00,0X00,0XFF,0X80,0X03,0XFF,0X00,0X00,0XFF,0X00,0X01,0XFF,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X07,0XFE,0X21,0X00,0X40,
	0X04,0X04,0X02,0X3F,0XE0,0XA0,0X7F,0XC5,0XFA,0X20,0X01,0X10,0X44,0X44,0X42,0X4F,
	0XC2,0X08,0X44,0X45,0XFA,0X40,0X0D,0XF6,0X44,0X44,0X42,0XCF,0XC0,0X00,0X7F,0XC4,
	0X52,0X40,0X00,0X00,0X04,0X04,0X4A,0X4F,0XC3,0XF8,0X04,0X07,0XFE,0X48,0X42,0X08,
	0X04,0X04,0X02,0X4F,0XC2,0X08,0X04,0X07,0XFE,0X48,0X43,0XF8,0X00,0X00,0X00,0X00,
	0X00,0X00, 
	};
	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//case1:参数非法
	if((ret = NDK_ScrDispMode(0)) != NDK_ERR_PARA)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_ScrDispMode(-1)) != NDK_ERR_PARA)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:屏幕显示文字设置为正常显示和反转显示
	cls_show_msg1(3,"接下来的文字将先翻转180度，再正常显示");
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode(LCD_ROLLINGOVER)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("国国国国国国国国国国国国国国国国国国国国国国国国国国国国");
	NDK_ScrRefresh();
	sleep(2);
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode( LCD_ROLLINGBACK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("国国国国国国国国国国国国国国国国国国国国国国国国国国国国");
	NDK_ScrRefresh();
	sleep(2);
	NDK_ScrClrs();
	nKeyin = cls_show_msg1(MAXWAITTIME, "文字先翻转180度，然后正常显示?\n是[ENTER],否则其它");
	if(nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:屏幕显示图片设置为正常显示和反转显示
	cls_show_msg1(3,"接下来的图片将先翻转180度，再正常显示");
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode(LCD_ROLLINGOVER)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDrawBitmap(0,7,48,HEIGHT_HLJNX,sCUPBMP51x48))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(2);
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode(LCD_ROLLINGBACK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDrawBitmap(0,7,48,HEIGHT_HLJNX,sCUPBMP51x48))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(2);
	NDK_ScrClrs();
	nKeyin = cls_show_msg1(MAXWAITTIME, "屏幕和图片先翻转180度，然后正常显示?\n是[ENTER],否则其它");
	if(nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	//case4:连续两次调用反转，最后也应为反转显示
	cls_show_msg1(3,"接下来的文字将一直都是反转状态");
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode(LCD_ROLLINGOVER)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("国国国国国国国国国国国国国国国国国国国国国国国国国国国国");
	NDK_ScrRefresh();
	sleep(2);
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode( LCD_ROLLINGOVER)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrPrintf("国国国国国国国国国国国国国国国国国国国国国国国国国国国国");
	NDK_ScrRefresh();
	sleep(2);
	NDK_ScrClrs();
	if((ret = NDK_ScrDispMode( LCD_ROLLINGBACK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "文字一直都是反转状态?\n是[ENTER],否则其它");
	if(nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScrDispMode(LCD_ROLLINGBACK);
	return;	
}

