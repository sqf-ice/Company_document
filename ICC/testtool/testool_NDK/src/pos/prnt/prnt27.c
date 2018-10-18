/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 27.c
* Author 			: zhengry
* version			: 
* DATE			: 20170503
* directory 		: 
* description		: NDK_PrnSetTrueTypeFontEncodingFormat设置输入格式、NDK_PrnTrueTypeFontText使用已加载的TTF字体显示数据
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PrnSetTrueTypeFontEncodingFormat、NDK_PrnTrueTypeFontText"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
#define FILENAME37  "angsa.ttf"
#define FILENAME2  "msgothic.ttc"
#define FILENAME3 "arial.ttf"
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: prnt27
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhengry         20170503  	   created
*****************************************************************/
void prnt27(void)
{
	/*private & local definition*/
	int ret=0;
	char asc_str[]="Hello NewLand123! Please Enter Any Key to Continue!";// 只能显示ttf字体中的文字
	char uni_str[]={0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x20, 0x00, 0x4E, 0x00, 0x65, 0x00, 
					0x77, 0x00, 0x4C, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 
					0x21, 0x00, 0x20, 0x00, 0x50, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x61, 0x00, 0x73, 0x00, 0x65, 0x00, 
					0x20, 0x00, 0x45, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x41, 0x00, 
					0x6E, 0x00, 0x79, 0x00, 0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x20, 0x00, 0x74, 0x00, 
					0x6F, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6E, 0x00, 
					0x75, 0x00, 0x65, 0x00, 0x21, 0x00};//是asc_str字符串的unicode格式
	char utf8_str[]={0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x4E, 0x65, 0x77, 0x4C, 0x61, 0x6E, 0x64, 0x31, 0x32, 0x33,
					 0x21, 0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x41,
					 0x6E, 0x79, 0x20, 0x4B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x43, 0x6F, 0x6E, 0x74, 0x69, 0x6E, 
					 0x75, 0x65, 0x21 };
	char asc_str1[]={0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0xD0, 0xC2, 0xB4, 0xF3, 0xC2, 0xBD};//"hello新大陆"ascii编码16进制内容
	char uni_str1[]={0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x20, 0x00, 0xB0, 0x65, 0x27, 0x59, 0x46, 0x96};
	char uni_angsa_str[]={0x06, 0x0e, 0x10, 0x0e, 0x29, 0x0e, 0x37, 0x0e, 0x55, 0x0e, 0xbb, 0x00, 0xc8, 0x00, 0xe5, 0x00, 0xf1, 0x00, 0x20, 0x20};	uint rand_x=0;
	char utf8_msg_str[]={0xE3, 0x81, 0x93, 0xE3, 0x82, 0x93, 0xE3, 0x81, 0xAB, 0xE3, 0x81, 0xA1, 0xE3, 0x81, 0xAF, 0xE3,
						 0x80, 0x81, 0xE3, 0x81, 0x93, 0xE3, 0x82, 0x8C, 0xE3, 0x81, 0xAF, 0xE3, 0x83, 0x86, 0xE3, 0x82, 
						 0xB9, 0xE3, 0x83, 0x88, 0xE3, 0x81, 0xAE, 0xE4, 0xBE, 0x8B, 0xE3, 0x81, 0xA7, 0xE3, 0x81, 0x99 };//日文版的:你好,这是测试用例

	char utf8_arial_str[]={0xD8, 0xB3, 0xD9, 0x84, 0xD8, 0xA7, 0xD9, 0x85, 0xD8, 0x8C, 0x20, 0xDA, 0x86,
		                  0xDB, 0x8C, 0xD9, 0x86}; //波斯语的:你好，中国

	char uni_arial_str[]={0x33, 0x06, 0x44, 0x06, 0x27, 0x06, 0x45, 0x06, 0x0C, 0x06, 0x20, 0x00, 0x86, 0x06,
		                   0xCC, 0x06, 0x46, 0x06};  //波斯语的:你好，中国
						 
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	cls_show_msg("请将%s,%s和%s下载到与测试程序同一个路径下,并保有纸后按任意键继续...", FILENAME37, FILENAME2,FILENAME3);
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto ERR;
	}

	//case1:未加载ttf字体，调用设置输入格式与显示字符串函数应该失败
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:未设置字体宽高,应无法打印字符串
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME37, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
#if 0 //需求变更为开机也有设置字体宽高默认值为24*24,可以显示字符串 20180307 sull modify
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应无内容打印,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
	
	//case2.1:异常参数：input为非DISP_TTF_INPUT_ASCII/UNICODE时
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(48,48))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UTF8+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.2:异常参数：X=-1,X为特大值?,str=NULL,strlen=-1
	if((ret=NDK_PrnTrueTypeFontText(-1, asc_str, strlen(asc_str)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(384, asc_str, strlen(asc_str)))!=NDK_ERR_PARA) //热敏打印最大宽度为384点(0~383点)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, NULL, strlen(asc_str)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:设置为UNICODE格式时，传入字符串长度必须是2的倍数，否则应返回NDK_ERR_PARA
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, 1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3：设置ascii格式的时候，显示unicode字符应该会有多出来0x00
	rand_x=rand()%384; 
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(rand_x, uni_str, sizeof(uni_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容从x=%d坐标开始为%s,应清晰无乱码,但每个字符之间会有空白框,是[ENTER],否[其他]", rand_x, asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case4:设置ascii格式的时候，打印utf8字符能正常打印(UTF-8兼容ascii格式)
	rand_x=rand()%384; 
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(rand_x, utf8_str, sizeof(utf8_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容从x=%d坐标开始为%s,应清晰无乱码,是[ENTER],否[其他]", rand_x, asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case5：设置unicode格式的时候，显示ascii码字符应该会乱码
	rand_x=rand()%384; 
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(rand_x, asc_str, strlen(asc_str)+1))!=NDK_OK)//asc_str为51个字符,在unicode格式下,字符串长度需为2的整数倍
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容应从x=%d坐标开始为白框框,是[ENTER],否[其他]", rand_x) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6:应无法显示非ttf字体文件中的字
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str1, sizeof(asc_str1)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容为hello+乱码,应能清晰打印,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case7:传入所需显示字符长度,应能正确显示相应长度
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_str1, 10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印内容为hello,应能清晰打印,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case8:正常测试,三种种输入格式应该能正常打印
	//case8.1:输入格式为ascii码,应能打印相应字符
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
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
	if( cls_show_msg("打印内容为%s,应能清晰打印,是[ENTER],否[其他]", asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case8.2:输入格式为unicode码,应能打印相应字符
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UNICODE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, uni_angsa_str, sizeof(uni_angsa_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应能清晰打印,且打印内容应与prn文件夹下的测试结果图片1一致,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case8.3:输入格式为utf-8码,应能打印相应字符
	if((ret=NDK_PrnDestroyTrueTypeFont())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
	if( cls_show_msg("应能清晰打印,且打印内容应与prn文件夹下的测试结果图片2一致,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case9:新增波斯语，ascii码只支持英文和数字，故不测	add by sull 20171101
	//case9.1:输入格式为utf-8码，应能正常打印波斯语	
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
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_UTF8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, utf8_arial_str, sizeof(utf8_arial_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应能清晰打印,且打印顺序应从右到左，字体大小:36*36，内容为波斯语的:你好，中国，是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case9.2:输入格式为unicode码，应能正常打印波斯语
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
	if( cls_show_msg("应能清晰打印,且打印顺序应从右到左，字体大小:72*72，内容为波斯语的:你好，中国，是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PrnInit(g_PrnSwitch);	
	NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII);
	NDK_PrnDestroyTrueTypeFont();
	return;
}
