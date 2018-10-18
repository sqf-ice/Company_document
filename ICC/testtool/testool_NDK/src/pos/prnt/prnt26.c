/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 26.c
* Author 			: zhengry
* version			: 
* DATE			: 20170502
* directory 		: 
* description		: NDK_PrnSetTrueTypeFontSizeInPixel设置TTF字体的宽度和高度和NDK_PrnGetTrueTypeFontSize获取字体宽高
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PrnSetTrueTypeFontSizeInPixel、NDK_PrnGetTrueTypeFontSize"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
#define FILENAME36  "angsa.ttf"
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: prn26
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhengry         20170502  	   created
*****************************************************************/
void prnt26(void)
{
	/*private & local definition*/
	int ret=0, getwidth=0, getheight=0, getwidth1=0, getheight1=0, getwidth2=0, getheight2=0;
	char asc_str[]="Innovation in China 0123456789!";
	char ch1= 'A', ch2= 'a', ch3= 'h';

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	cls_show_msg("请将%s下载到与测试程序同一个路径下,并保有纸后按任意键继续...", FILENAME36);
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto ERR;
	}

	//case1:未加载ttf字体，调用设置字体宽高函数与获取宽高函数应该失败
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(12, 12))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:在未设置宽高情况，获取字符串宽高应该为0
	if((ret=NDK_PrnInitTrueTypeFont(FILENAME36, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	#if 0 //需求变更为开机也有设置字体宽高默认值为24*24,获取的字符宽高不为0,不同字体获取的宽高不一致，不作判断 20180307 sull modify
	if( getwidth!=0 || getheight!=0 )
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, getwidth, getheight);
		GOTOERR;
	}
	#endif
	
	if((ret=NDK_PrnSetTrueTypeFontEncodingFormat(PRN_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case3.1:异常参数：width/height=-1
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(-1, 12))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(12, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.2:异常参数：str传NULL,strlen=-1
	if((ret=NDK_PrnGetTrueTypeFontSize(NULL, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, -1, &getwidth, &getheight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	
	//case4:设置宽高过大,将无法打印(时间会比较久)
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(60000, 60000))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if OVERSEAS_OS70_ENABLE  //海外产品7.0版本设置默认最大字体为1000(原来5000)，可以打印，字太长空间会不够  20180423 sull modify
	if((ret=NDK_PrnGetTrueTypeFontSize("Inno 0!", strlen("Inno 0!"), &getwidth, &getheight))!=NDK_OK)

	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, "Inno 0!", strlen("Inno 0!")))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("应以最大字体打印内容Inno 0!,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#else
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
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
	if( cls_show_msg("应无内容打印,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

	//case5:设置非8整数倍大小字体,应能正常打印
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(31, 45))!=NDK_OK)
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
	if( cls_show_msg("打印%s,字体大小应该是31*45,应该清晰无乱码,是[ENTER],否[其他]", asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6：设置两次宽高，同一字应该获取到的宽高应该是第二次设置的
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24, 24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(48, 48))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth1, &getheight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	if( getwidth>=getwidth1 || getheight>=getheight1 )
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, getwidth, getwidth1, getheight, getheight1);
		GOTOERR;
	}
	if((ret=NDK_PrnTrueTypeFontText(0, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStart();
	NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);
	if( cls_show_msg("打印%s,字体大小应该是48*48,应该清晰无乱码,是[ENTER],否[其他]", asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case7:获取不同字符串的宽高,应该不一样,如A的宽高值比a的大,h比a高,与a一样宽
	if((ret=NDK_PrnSetTrueTypeFontSizeInPixel(24, 24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(&ch1, 1, &getwidth, &getheight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(&ch2, 1, &getwidth1, &getheight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PrnGetTrueTypeFontSize(&ch3, 1, &getwidth2, &getheight2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if( getwidth<=getwidth1 || getheight!=getheight1 || getwidth1>=getwidth2 || getheight1!=getheight2)
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d,%d,%d)", __LINE__, TESTAPI, getwidth, getheight, getwidth1, getheight1, getwidth2, getheight2);
		GOTOERR;
	}//获取到A、a、h的高度都一样;宽度从大到小排序A>h>a
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PrnInit(g_PrnSwitch);	
	NDK_PrnDestroyTrueTypeFont();
	return;
}

