/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 打印
* file name		: 28.c
* Author 			: 
* version			: 
* DATE			: 
* directory 		: 
* description		: 测试NDK_PrnSetTrueTypeFontAttr是否可以正确取原来的显示属性，并设置新的显示属性
* related document	: NDK.chm
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	MAXWAITTIME 10//30
#define FILENAME2  "msgothic.ttc"
#define FILENAME3 "arial.ttf"
#define ABCBANKBMP "abclogo.bmp"
#define	TESTAPI		"NDK_PrnSetTrueTypeFontAttr"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: prnt28
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	       sull           20180112  	    created
*
*****************************************************************/
void prnt28(void)
{
	/*private & local definition*/
	int ret = -1;
	EM_PRN_TTF_ATTRIBUTE OldAttr = PRN_TTF_ATTRIBUTE_NORMAL;
	char utf8_msg_str[]={0xE3, 0x81, 0x93, 0xE3, 0x82, 0x93, 0xE3, 0x81, 0xAB, 0xE3, 0x81, 0xA1, 0xE3, 0x81, 0xAF, 0xE3,
						 0x80, 0x81, 0xE3, 0x81, 0x93, 0xE3, 0x82, 0x8C, 0xE3, 0x81, 0xAF, 0xE3, 0x83, 0x86, 0xE3, 0x82, 
						 0xB9, 0xE3, 0x83, 0x88, 0xE3, 0x81, 0xAE, 0xE4, 0xBE, 0x8B, 0xE3, 0x81, 0xA7, 0xE3, 0x81, 0x99 };//日文版的:你好,这是测试用例
	char uni_arial_str[]={0x33, 0x06, 0x44, 0x06, 0x27, 0x06, 0x45, 0x06, 0x0C, 0x06, 0x20, 0x00, 0x86, 0x06,
		                 0xCC, 0x06, 0x46, 0x06, 0x0D, 0x00};  //波斯语的:你好，中国
	
	/*process body*/
	//测试前置
	cls_show_msg("请将%s,%s和%s下载到与测试程序同一个路径下,并保有纸后按任意键继续...",ABCBANKBMP, FILENAME2,FILENAME3);
	//case1:参数非法
	if((ret = NDK_PrnSetTrueTypeFontAttr(0, &OldAttr)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}


	//case2:设置为反显模式，且能正常获取到开机默认值正显模式(获取的是设置之前的模式)
	if((ret = NDK_PrnSetTrueTypeFontAttr(PRN_TTF_ATTRIBUTE_REVERSE, &OldAttr)) != NDK_OK || OldAttr != PRN_TTF_ATTRIBUTE_NORMAL)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, OldAttr);
		GOTOERR;
	}

	//case3:调用NDK_PrnInit后会被自动重置为正显模式
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		GOTOERR;
	}	
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME2, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(48,48))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UTF8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, utf8_msg_str, sizeof(utf8_msg_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应能清晰打印,且打印内容应与prn文件夹下的测试结果图片2一致,打印属性为正常模式，是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:设置为反显模式，且能正常获取正显模式，打印出来的效果为反显模式
	if((ret = NDK_PrnSetTrueTypeFontAttr(PRN_TTF_ATTRIBUTE_REVERSE, &OldAttr)) != NDK_OK || OldAttr != PRN_TTF_ATTRIBUTE_NORMAL)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, OldAttr);
		GOTOERR;
	}
	if((ret = NDK_PrnDestroyTrueTypeFont()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME3, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(72,72))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_arial_str, sizeof(uni_arial_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应能清晰打印,且内容为波斯语的:你好，中国,打印属性为反显模式?是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3.1:反显模式对打印字符串和图片不应生效,只对ttf设置有效
	if((ret = NDK_PrnSetTrueTypeFontAttr(PRN_TTF_ATTRIBUTE_REVERSE, &OldAttr)) != NDK_OK || OldAttr != PRN_TTF_ATTRIBUTE_REVERSE)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, OldAttr);
		GOTOERR;
	}
	NDK_PrnStr("工行Logo");
	if((ret = NDK_PrnPicture(0,ABCBANKBMP)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 	
	NDK_PrnStart();
	if(cls_show_msg("应能清晰打印,且打印属性为正常模式,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:允许测试原模式参数为NULL，此时没有返回值
	if((ret = NDK_PrnSetTrueTypeFontAttr(PRN_TTF_ATTRIBUTE_NORMAL, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PrnDestroyTrueTypeFont()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME3, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(72,72))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_arial_str, sizeof(uni_arial_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if(cls_show_msg("应能清晰打印,且内容为波斯语的:你好，中国,打印属性为正常模式?是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);

ERR:
	NDK_PrnSetTrueTypeFontAttr(PRN_TTF_ATTRIBUTE_NORMAL, &OldAttr);
	NDK_PrnInit(g_PrnSwitch);	
	NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII);
	NDK_PrnDestroyTrueTypeFont();
	return;
}
