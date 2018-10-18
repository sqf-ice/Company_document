/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 29.c
* Author 			: linwl
* version			: 
* DATE			: 20141208
* directory 		: 
* description		: 测试NDK_ScrGetFontBuf能否根据输入字符串取出系统当前字体相应的点阵缓冲
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrGetFontBuf"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/
typedef struct {
	unsigned int width;  			/**< 图像宽度（像素） */
	unsigned int height; 			/**< 图像高度（像素）*/
	unsigned int bytes_per_pixel; /**< 每个像素多少字节 */
	void * image_buf;		/**< 图像色彩数据 */
	void * noused;
}disp_test;

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
void scr29(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0;
	uint ViewWidth = 0, ViewHeight = 0, unBuf_x = 0, unBuf_y = 0;
	char str[]="欢迎使用:1234590!@#()*&";
	unsigned short img[320*240*2] = {0};
	disp_test * dispbuf = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	//测试前置:初始化测试缓冲
	memset(img,0xff,ViewWidth*ViewHeight*2);
	
	//case1:缓冲区参数为NULL情况下，应该返回参数错误
	if((ret=NDK_ScrGetFontBuf(NULL, img, unBuf_x, unBuf_y, ViewWidth, ViewHeight, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetFontBuf(str, NULL, unBuf_x, unBuf_y, ViewWidth, ViewHeight, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//X>=W，Y>=H的情况应该返回NDK_ERR_OVERFLOW
	if((ret=NDK_ScrGetFontBuf(str, img, ViewWidth, unBuf_y, ViewWidth, ViewHeight, 0))!=NDK_ERR_OVERFLOW)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetFontBuf(str, img, unBuf_x, ViewHeight, ViewWidth, ViewHeight, 0))!=NDK_ERR_OVERFLOW)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//W,H为0情况下应该返回参数错误
	if((ret=NDK_ScrGetFontBuf(str, img, unBuf_x, unBuf_y, 0, ViewHeight, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetFontBuf(str, img, unBuf_x, unBuf_y, ViewWidth, 0, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:图片解析缓存为NULL,调用显示函数应该失败
	dispbuf = calloc(1,sizeof(disp_test));
	if(dispbuf==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	lib_kbflush();	//清除按键缓冲区
	//为实现自动化测试，启动计数器
	lib_StartStopwatch();
	while ((nKeyin=lib_kbhit())==0)
	{	
		
		unBuf_x = rand()%ViewWidth;
		unBuf_y = rand()%ViewHeight;
		cls_show_msg1(2, "将在(%d, %d)显示%s.测试正常,可按ENTER;有异常,请按其它键", unBuf_x, unBuf_y, str);
		memset(img,0xff,ViewWidth*ViewHeight*2);//把空间初始化成全白
		if((ret = NDK_ScrGetFontBuf(str,img,unBuf_x,unBuf_y,ViewWidth,ViewHeight,0))!=NDK_OK)//将获取到字符点阵放到全屏缓冲中
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			free(dispbuf);
			return;
		}
		memset(dispbuf, 0, sizeof(dispbuf));
		dispbuf->width = ViewWidth;
		dispbuf->height = ViewHeight;
		dispbuf->bytes_per_pixel = 2;
		dispbuf->image_buf = img;
		NDK_ScrDispImg(0,0,ViewWidth,ViewHeight,dispbuf);
		NDK_ScrRefresh();
		NDK_SysDelay(20);	
		if (MAXWAITTIME<lib_ReadStopwatch())
			break;
	}
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过" ,TESTAPI);
	else
		send_result("%s测试失败" ,TESTAPI);
	free(dispbuf);
	return;
}

