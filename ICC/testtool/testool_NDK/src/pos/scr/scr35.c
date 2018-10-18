/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 35.c
* Author 			: zhengry
* version			: 
* DATE			: 20170425
* directory 		: 
* description		: NDK_ScrInitTrueTypeFont加载TTF字体和NDK_ScrDestroyTrueTypeFont字体卸载
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScrInitTrueTypeFont、NDK_ScrDestroyTrueTypeFont"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
#define FILENAME35  "angsa.ttf"
#define FILENAMEPATH1 "andlso.ttf"   //改为放在当前路径下  20180502 modify
#define FILENAME1	  "andlso.ttf"
#define FILENAME2  "msgothic.ttc"
#define FILEERRNAME  "errfont.ttf"
#define FILEERRPATH   "/appfs/apps/angsa.ttf"
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr35
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhengry         20170425  	   created
*****************************************************************/
void scr35(void)
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
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	cls_show_msg1(15, "请将%s,%s和%s下载到与测试程序同一个路径下", FILENAME35, FILENAME2, FILENAME1);

	//case1:未加载ttf字体，调用卸载字体函数应该失败
	if((ret=NDK_ScrDestroyTrueTypeFont())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:异常参数:filepathname传NULL/filepathname文件名出错/filepathname文件路径出错
	if((ret=NDK_ScrInitTrueTypeFont(NULL, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrInitTrueTypeFont(FILEERRNAME, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrInitTrueTypeFont(FILEERRPATH, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:字体文件的字形索引index不为字体文件中的，应无法加载TTF字体 
	if((ret=NDK_ScrInitTrueTypeFont(FILENAME35, -1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:加载angsa字体并显示字体,加载andlso字体并显示字体,应能显示对应字体;开机未设置输入格式,默认为ascii码显示
	if(cls_show_msg("是否开机首次运行ttf字体加载显示测试用例,是[ENTER],否[其他]") != ENTER)
	{
		NDK_ScrClrs();	
		if((ret=NDK_ScrInitTrueTypeFont(FILENAME35, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScrDispTrueTypeFontText(0, 0, asc_str, strlen(asc_str)))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_ScrRefresh();
		NDK_KbGetCode(0,0);
		if( cls_show_msg("显示内容为%s应该清晰无乱码，并请在%s字体文件中检测字体大小为24的字符样式是否与其一致,是[ENTER],否[其他]", asc_str, FILENAME35) != ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	
	NDK_ScrClrs();
	if((ret=NDK_ScrInitTrueTypeFont(FILENAMEPATH1, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispTrueTypeFontText(0,0, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_KbGetCode(0,0);
	if( cls_show_msg("显示内容为%s应该清晰无乱码，并请在%s字体文件中检测字体大小为24的字符样式是否与其一致,是[ENTER],否[其他]", asc_str, FILENAME1) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_ScrDestroyTrueTypeFont())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:卸载字体后,再次安装新字体应该能成功;在不止一个字体中的字体文件中，在调用正确index下，应能正常加载TTF字体
	NDK_ScrClrs();
	if((ret=NDK_ScrInitTrueTypeFont(FILENAME2, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispTrueTypeFontText(0, 0, uni_str1, sizeof(uni_str1)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_KbGetCode(0,0);
	
	NDK_ScrClrs();
	if((ret=NDK_ScrInitTrueTypeFont(FILENAME2, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24,24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispTrueTypeFontText(0, 20, uni_str1, sizeof(uni_str1)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_KbGetCode(0,0);
	if( cls_show_msg("显示内容为%s应该清晰无乱码，且两种显示的字体不一样,是[ENTER],否[其他]", asc_str1) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_ASCII);
	
	if((ret=NDK_ScrDestroyTrueTypeFont())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6: 调用卸载字体函数后，调用设置输入格式应该失败
	if((ret=NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_ASCII))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7: 调用卸载字体函数后，调用设置字体大小应该失败
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24,24))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8: 调用卸载字体函数后，调用获取字体大小应该失败
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case9: 调用卸载字体函数后，调用显示字体应该失败
	if((ret=NDK_ScrDispTrueTypeFontText(0, 0, asc_str, strlen(asc_str)))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScrDestroyTrueTypeFont();
	return;
}
