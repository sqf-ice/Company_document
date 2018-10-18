/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 27.c
* Author 			: linwl
* version			: 
* DATE			: 20141208
* directory 		: 
* description		: 测试NDK_ScrImgDecode能否将图片解码数据保存到指定缓存,NDK_ScrImgDestroy函数能否释放指定的图片缓存
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#define 	TESTAPI		"NDK_ScrImgDecode,NDK_ScrImgDestroy"
#define SHUTDOWNPIC "shutdown.jpg"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513
#define BIGPIC "big.png"
#define BMPPIC "desktop.bmp"
#define LOGPIC "log.bmp"
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
*****************************************************************/
void scr27(void)
{
	/*private & local definition*/
	int ret = 0;
	uint ViewWidth = 0, ViewHeight = 0;
	char *imgbuf1 = NULL, *imgbuf2 = NULL, *imgbuf3 = NULL;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	//测试前置
	if(NDK_FsExist(SHUTDOWNPIC)!=NDK_OK&&NDK_FsExist(BIGPIC)!=NDK_OK&&NDK_FsExist(BMPPIC)!=NDK_OK&&NDK_FsExist(LOGPIC)!=NDK_OK)
	{
		send_result("%s测试图片不存在,请在POS的测试程序路径目录下载测试图片重新测试", TESTAPI);
		return;
	}
	
	//case1:图片路径不存在,图片缓存未解析成功时,
	if((ret=NDK_ScrImgDecode(ERRPIC,&imgbuf1))!=NDK_ERR_PATH)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrImgDestroy(imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:图片路径和缓存参数为NULL
	if((ret=NDK_ScrImgDecode(NULL,&imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrImgDestroy(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:正常测试jpg格式图片解码应该成功,并能够正常显示
	if((ret=NDK_ScrImgDecode(SHUTDOWNPIC,&imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);	
	if(ENTER != cls_show_msg1(MAXWAITTIME,"已显示shutdown图片,按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case4:正常测试png格式图片解码应该成功,并能够正常显示
	if((ret=NDK_ScrImgDecode(BIGPIC,&imgbuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);	
	if(ENTER != cls_show_msg1(MAXWAITTIME,"已显示big图片,按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR2;
	}
	
	//case7:正常测试bmp格式图片解码应该成功,并能够正常显示
	if((ret=NDK_ScrImgDecode(LOGPIC,&imgbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR3;
	}
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR3;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);	
	if(ENTER != cls_show_msg1(MAXWAITTIME,"已显示LOG图片,按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR3;
	}

	//case5:正常测试bmp格式图片解码应该成功,并能够正常显示
	if((ret=NDK_ScrImgDecode(BMPPIC,&imgbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR3;
	}
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR3;
	}
	NDK_ScrRefresh();
	NDK_SysDelay(30);	
	if(ENTER != cls_show_msg1(MAXWAITTIME,"已显示bmp图片,按确认测试通过，其它键失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR3;
	}
	
	//case6:测试释放指定的图片缓存应该成功,且调用显示应该失败
	if((ret=NDK_ScrImgDestroy(imgbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR3;
	}
	imgbuf3=NULL;//释放资源后需将指针指NULL,否则可能跑飞
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf3))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=NDK_ScrImgDestroy(imgbuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	imgbuf2=NULL;
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrImgDestroy(imgbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	imgbuf1=NULL;
	if((ret=NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,imgbuf1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	send_result("%s测试通过", TESTAPI);
	return;

	goto ERR3;
ERR3:
	NDK_ScrImgDestroy(imgbuf3);
	imgbuf3=NULL;
	goto ERR2;
ERR2:
	NDK_ScrImgDestroy(imgbuf2);
	imgbuf2=NULL;
	goto ERR;
ERR:
	NDK_ScrImgDestroy(imgbuf1);
	imgbuf1=NULL;
	return;
}

