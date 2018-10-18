/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 4.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_ScrSetbgPic是否设置屏幕背景图片，测试NDK_ScrClrbgPic是否可以取消背景图片
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 5
#define 	TESTAPI		"NDK_ScrSetbgPic,NDK_ScrClrbgPic"

#define HEIGHT_HLJNX	(51)
#define SHUTDOWNPIC "shutdown.png"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513
#define SMALLPIC "set.png"
#define ERRPIC "err.png"
#define BIGPIC "big.png"
#define TESTBGPIC "test_bg.png"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr4
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void scr4(void)
{
	/*private & local definition*/
	int  ret = 0, nKeyin = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置
	if(cls_show_msg1(MAXWAITTIME,"请确认已经在测试程序路径下载了shutdown,set,big等png图片,按任意键继续,ESC键退出")==ESC)
		return;
	
	//case1:背景图片路径为NULL
	if((ret=NDK_ScrSetbgPic(NULL))!=NDK_ERR_DECODE_IMAGE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case2:背景图片路径正确,图片不存在
	if((ret=NDK_ScrSetbgPic(ERRPIC))!=NDK_ERR_PATH)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrClrs();

	//case3:设置背景图片大小大于屏幕大小
	if((ret=NDK_ScrSetbgPic(BIGPIC)&&auto_flag!=1)!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已设置大屏背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();
	
	//case4:取消背景图片
	if((ret=NDK_ScrClrbgPic())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已取消大屏背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();

	//case5:设置背景图片大小与屏幕大小相同
	//先设置大屏幕背景,再设置较小屏背景,删除全屏背景,看是否是最近一次有效 20140603 added by jiangym
	if((ret=NDK_ScrSetbgPic(BIGPIC))!=NDK_OK&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已设置大屏背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();
	if((ret=NDK_ScrSetbgPic(SHUTDOWNPIC))!=NDK_OK&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已设置小背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();
	
	//case6:取消背景图片	
	if((ret=NDK_ScrClrbgPic())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	nKeyin=cls_show_msg1(MAXWAITTIME,"已取消小背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();

	//case7:设置背景图片的大小小于屏幕大小
	if((ret=NDK_ScrSetbgPic(SMALLPIC))!=NDK_OK&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已设置小背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrClrs();
	
	//case8:取消背景图片
	if((ret=NDK_ScrClrbgPic())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME,"已取消小背景图片,按确认测试通过，其它键失败");
	if(ENTER!=nKeyin&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	send_result("%s测试通过", TESTAPI);

ERR:
	NDK_ScrClrbgPic();
	if(cls_show_msg1(5, "测试入口是否有设置背景图片:是[确认],否[其它]")==ENTER)
		NDK_ScrSetbgPic(TESTBGPIC);
	return;
}

