/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 25.c
* Author 			: zhengry
* version			: 
* DATE			: 20170502
* directory 		: 
* description		: NDK_PrnInitTrueTypeFont加载TTF字体和NDK_PrnDestroyTrueTypeFont字体卸载
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PrnInitTrueTypeFont、NDK_PrnDestroyTrueTypeFont"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
#define FILENAME35  "angsa.ttf"
#define FILENAMEPATH1 "andlso.ttf"  //改为放到当前路径  20180502 modify
#define FILENAME1	  "andlso.ttf"
#define FILENAME2  "msgothic.ttc"
#define FILEERRNAME  "errfont.ttf"
#define FILEERRPATH   "/appfs/apps/angsa.ttf"
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: prn25
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhengry         20170502  	   created
*****************************************************************/
void prnt25(void)
{
	/*private & local definition*/
	int ret=0, getwidth=0, getheight=0;
	char asc_str[]="Innovation in China 0123456789!";//angsa和andlso不支持中文字体,是支持其他外文字体
	char asc_str1[]="Innovation in China 中国制造，惠及全球 0123456789!";//msgothic支持中文和日文
	char uni_str1[]={0x49, 0x00, 0x6E, 0x00, 0x6E, 0x00, 0x6F, 0x00, 0x76, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 
					 0x6F, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x43, 0x00, 0x68, 0x00, 
		        	 0x69, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x20, 0x00, 0x2D, 0x4E, 0xFD, 0x56, 0x36, 0x52, 0x20, 0x90,
		        	 0x0C, 0xFF, 0xE0, 0x60, 0xCA, 0x53, 0x68, 0x51, 0x03, 0x74, 0x20, 0x00, 0x30, 0x00, 0x31, 0x00,
		        	 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 
		        	 0x21, 0x00};// Innovation in China 中国制造，惠及全球 0123456789!的十六进制

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	cls_show_msg("请将%s,%s和%s下载到与测试程序同一个路径下,并保有纸后按任意键继续...", FILENAME35, FILENAME2, FILENAME1);
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto ERR;
	}
	/*这几个新函数不受打印中的设置函数影响*/
	
	//case1:未加载ttf字体，调用卸载字体函数应该失败
	if((ret=NDK_PrnDestroyTrueTypeFont())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:异常参数:filepathname传NULL/filepathname文件名出错/filepathname文件路径出错
	if((ret=NDK_PrnInitTrueTypeFont(NULL, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILEERRNAME, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILEERRPATH, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:字体文件的字形索引index不为字体文件中的，应无法加载TTF字体 
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME35, -1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:加载angsa字体并显示字体,加载andlso字体并打印字体,应能打印对应字体;开机未设置输入格式,默认为ascii码显示
	if(cls_show_msg("是否开机首次运行ttf字体加载打印测试用例,是[ENTER],否[其他]") != ENTER)
	{
		if((ret=NDK_PrnInitTrueTypeFont(FILENAME35, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_PrnStart();
		NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
		if( cls_show_msg("打印内容为%s应该清晰无乱码，并请在%s字体文件中检测字体大小为24的字符样式是否与其一致,是[ENTER],否[其他]", asc_str, FILENAME35) != ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	
	if((ret=NDK_PrnInitTrueTypeFont(FILENAMEPATH1, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容为%s应该清晰无乱码，并请在%s字体文件中检测字体大小为24的字符样式是否与其一致,是[ENTER],否[其他]", asc_str, FILENAME1) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case5:卸载字体后,再次安装新字体应该能成功;在不止一个字体中的字体文件中，在调用正确index下，应能正常加载TTF字体
	if((ret=NDK_PrnDestroyTrueTypeFont())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME2, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_str1, sizeof(uni_str1)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);

	if((ret=NDK_PrnInitTrueTypeFont(FILENAME2, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_str1, sizeof(uni_str1)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容为%s应该清晰无乱码，且打印的两种字体不一样,是[ENTER],否[其他]", asc_str1) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII);

	if((ret=NDK_PrnDestroyTrueTypeFont())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6: 调用卸载字体函数后，调用设置输入格式应该失败
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7: 调用卸载字体函数后，调用设置字体大小应该失败
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24,24))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8: 调用卸载字体函数后，调用获取字体大小应该失败
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case9: 调用卸载字体函数后，调用显示字体应该失败
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PrnInit(g_PrnSwitch);
	NDK_PrnDestroyTrueTypeFont();
	return;
}
