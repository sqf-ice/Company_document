/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 28.c
* Author 			: linwl
* version			: 
* DATE			: 20141208
* directory 		: 
* description		: 测试NDK_ScrDispImg能否在屏幕上显示图片
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrDispImg"
#define BIGPIC "big.png"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513
#define ERRPIC "err.jpg"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr21
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        linwl         20141208  	   created
*
*****************************************************************/
void scr28(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint ViewWidth = 0, ViewHeight = 0;
	char *imgbuf1 = NULL;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	//测试前置
	if(NDK_FsExist(BIGPIC)!=NDK_OK)
	{
		send_result("%s测试图片不存在,请在POS的测试程序路径目录下载测试图片重新测试", TESTAPI);
		return;
	}
	
	//case1:图片未进行正确解析,调用显示函数应该失败
	if((ret=NDK_ScrImgDecode(ERRPIC,&imgbuf1))!=NDK_ERR_PATH)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImg(0,0,GUI_GetLCDWidth(),GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	
	//case2:图片解析缓存为NULL,调用显示函数应该失败
	if((ret=NDK_ScrDispImg(0,0,GUI_GetLCDWidth(),GUI_GetLCDHeight(),NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	
	//case3:参数异常：unX,unY,unWidth,unHeight分别为-1,>屏幕,unX+unWidth>屏幕,unY+unHeight>屏幕
	if((ret=NDK_ScrImgDecode(BIGPIC,&imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(-1,0,GUI_GetLCDWidth(),GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_ScrDispImg(0,-1,GUI_GetLCDWidth(),GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_ScrDispImg(0,0,-1,GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_ScrDispImg(0,0,GUI_GetLCDWidth(),-1,imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_ScrDispImg(0,0,GUI_GetLCDWidth()+1,GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(0,0,GUI_GetLCDWidth(),GUI_GetLCDHeight()+1,imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(10,0,GUI_GetLCDWidth()-9,GUI_GetLCDHeight(),imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(0,10,GUI_GetLCDWidth(),GUI_GetLCDHeight()-9,imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(10);
	NDK_ScrClrs();
	
	//case4:正常测试
	if((ret=NDK_ScrImgDecode(BIGPIC,&imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);
	nKeyin = cls_show_msg1(MAXWAITTIME,"已显示big图片,按确认测试通过，其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);

ERR:
	NDK_ScrImgDestroy(imgbuf1);
	return;
}

