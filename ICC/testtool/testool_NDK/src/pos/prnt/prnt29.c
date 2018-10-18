/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 打印
* file name		: 29.c
* Author 			: 
* version			: 
* DATE			: 
* directory 		: 
* description		: 测试NDK_PrnSetMixMode能否正常设置混排模式
* related document	: NDK.chm
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define ABCBANKBMP "abclogo.bmp"
#define FILENAME3  "arial.ttf"
#define	TESTAPI		"NDK_PrnSetMixMode"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: prnt29
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	       sull           20180118  	    created
*
*****************************************************************/
void prnt29(void)
{
	/*private & local definition*/
	int ret = -1;
	char uni_arial_str[]={0x33, 0x06, 0x44, 0x06, 0x27, 0x06, 0x45, 0x06, 0x0C, 0x06, 0x20, 0x00, 0x86, 0x06,
		                 0xCC, 0x06, 0x46, 0x06};  //波斯语的:你好，中国

	
	/*process body*/
	//测试前置
	cls_show_msg("请将%s和%s下载到与测试程序同一个路径下,并确保有纸后按任意键继续...", ABCBANKBMP,FILENAME3);
	
	//case1:获取开机默认值(打印出来的应该为非混排模式)
	NDK_PrnStr("NewLand新大陆00045");
	NDK_PrnImage(128, 64, 0, CCB_BMP);
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否不在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case2:设置为0，为非混排模式
	if((ret = NDK_PrnSetMixMode(0)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_PrnStr("工行Logo");
	if((ret = NDK_PrnPicture(0,ABCBANKBMP)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否不在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case3:设置为1，为混排模式
	if((ret = NDK_PrnSetMixMode(1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStr("----------------NewLand新大陆00045");
	NDK_PrnImage(128, 64, 0, CCB_BMP);
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:设置为-1，为混排模式
	if((ret = NDK_PrnSetMixMode(-1)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_PrnStr("------工行Logo");
	if((ret = NDK_PrnPicture(0,ABCBANKBMP)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case5:打印init后会自动恢复默认值(不混排模式)
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		GOTOERR;
	}
	NDK_PrnStr("NewLand新大陆00045");
	NDK_PrnImage(128, 64, 0, CCB_BMP);
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否不在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6:设置为混排，ttf字体和图片在同一行
	if((ret = NDK_PrnSetMixMode(1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME3, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(36,36))!=NDK_OK)
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
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if((ret = NDK_PrnPicture(0,ABCBANKBMP)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否在同一行?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case7:设置为非混排，ttf字体和图片不在同一行
	if((ret = NDK_PrnSetMixMode(0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME3, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(36,36))!=NDK_OK)
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
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	NDK_PrnImage(128, 64, 0, CCB_BMP);
	NDK_PrnStart();
	if(cls_show_msg("打印的文字和图片是否不在同一行?是是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
ERR:
	NDK_PrnSetMixMode(0);
	NDK_PrnInit(g_PrnSwitch);	
	NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII);
	NDK_PrnDestroyTrueTypeFont();
	return;
}

