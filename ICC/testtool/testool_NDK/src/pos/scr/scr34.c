/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 34.c
* Author 			: zhengry
* version			: 
* DATE			: 20170220
* directory 		: 
* description		: 测试NDK_ScrDispImgWithFlash通过Flash绝对地址显示图片
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define		MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrDispImgWithFlash"
#define 	ADDR 		0x03788000
#define 	PICLEN 		139520
#define		PIC_XLEN	320
#define 	PIC_YLEN	218

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr10
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        linwl         20160720  	   created
*****************************************************************/
void scr34(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);

	//测试前置
	if((nKeyin=cls_show_msg1(MAXWAITTIME,"请在开发配合下下载好测试所需图片,且配置好需显示图片的绝对地址、图片大小及长宽,[是]按确认,否则按其他")!=ENTER) && nKeyin !=0)
		return;

	//case1:参数异常：unX,unY,unWidth,unHeight,offset,len分别为-1,>屏幕,unX+unWidth>屏幕,unY+unHeight>屏幕
	if((ret=NDK_ScrDispImgWithFlash(-1,0,PIC_XLEN,PIC_YLEN, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,-1,PIC_XLEN,PIC_YLEN, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,0,-1,PIC_YLEN, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_ScrDispImgWithFlash(0,0,PIC_XLEN,-1, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,0,GUI_GetLCDWidth()+1,PIC_YLEN, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,0,PIC_XLEN,GUI_GetLCDHeight()+1, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(10,0,GUI_GetLCDWidth()-9,PIC_YLEN, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,10,PIC_XLEN,GUI_GetLCDHeight()-9, ADDR, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithFlash(0,0,PIC_XLEN,PIC_YLEN, -1, PICLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;//会跑飞;绝对地址应该要有范围
	}
	NDK_ScrRefresh();
	if((ret=NDK_ScrDispImgWithFlash(0,0,PIC_XLEN,PIC_YLEN, ADDR, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);//图片一闪;图片大小应该要有限制
		//RETURN;ret=0
	}
	NDK_SysDelay(10);

	//case2:测试参数中图片宽和高与实际图片的宽高不一致的情况,预期不应该跑飞,或有其它任何死机等异常,允许显示乱码
	cls_show_msg1(3,"3秒后在10,20位置显示一张%d*%d乱码图片", PIC_XLEN/2, PIC_YLEN/2);
	NDK_ScrClrs();
	if((ret=NDK_ScrDispImgWithFlash(10, 20, PIC_XLEN/2, PIC_YLEN/2, ADDR, PICLEN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);
	if ((nKeyin=cls_show_msg1(MAXWAITTIME,"显示的图片是否乱码,按确认测试通过,其它键失败"))!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//cls_show_msg1(3,"3秒后在0,0位置开始显示一张全屏乱码图片");
	if((ret=NDK_ScrDispImgWithFlash(0, 0, ViewWidth, ViewHeight, ADDR, PICLEN))!=NDK_ERR_PARA/*NDK_OK*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);

	//case3:正常测试显示图片
	cls_show_msg1(3,"3秒后在0,0位置显示图片");
	if((ret=NDK_ScrDispImgWithFlash(0, 0, PIC_XLEN, PIC_YLEN, ADDR, PICLEN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);
	nKeyin = cls_show_msg1(MAXWAITTIME,"显示的图片是否正常,按确认测试通过,其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

