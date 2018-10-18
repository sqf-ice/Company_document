/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 36.c
* Author 			: zhengry
* version			: 
* DATE			: 20170426
* directory 		: 
* description		: NDK_SrcSetTrueTypeFontSizeInPixel设置TTF字体的宽度和高度和NDK_SrcGetTrueTypeFontSize获取字体宽高
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScrSetTrueTypeFontSizeInPixel、NDK_ScrGetTrueTypeFontSize"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
#define FILENAME36  "angsa.ttf"
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr36
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhengry         20170426  	   created
*****************************************************************/
void scr36(void)
{
	/*private & local definition*/
	int ret=0, getwidth=0, getheight=0, getwidth1=0, getheight1=0, getwidth2=0, getheight2=0;
	char asc_str[]="Innovation in China 0123456789!";
	char ch1= 'A', ch2= 'a', ch3= 'h';
	uint ViewWidth = 0, ViewHeight = 0;

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	cls_show_msg1(5, "请将%s下载到与测试程序同一个路径下", FILENAME36);
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);

	//case1:未加载ttf字体，调用设置字体宽高函数与获取宽高函数应该失败
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(12, 12))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=NDK_ScrInitTrueTypeFont(FILENAME36, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_ASCII))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:在未设置宽高情况，获取字符串宽高应该为0
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
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
	
	//case3.1:异常参数：width/height传入小于等于-1值,会转换成1
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(-1, 12))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(12, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.2:异常参数：str传NULL,strlen=-1,传-1的时候会转换成1
	if((ret=NDK_ScrGetTrueTypeFontSize(NULL, strlen(asc_str), &getwidth, &getheight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, -1, &getwidth, &getheight))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	
	//case4:设置宽高为最大屏幕宽和最大屏幕高,将无法显示,屏幕宽高为320,240,实际获取到的宽高为7147,279
	NDK_ScrClrs();
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(ViewWidth, ViewHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
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
	sleep(2);
	if( cls_show_msg("应无内容显示,是[ENTER],否[其他]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case5:设置非8整数倍大小字体,应能正常显示
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(31, 45))!=NDK_OK)
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
	if( cls_show_msg("显示内容为%s,字体大小应该是31*45,应该清晰无乱码,是[ENTER],否[其他]", asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6：设置两次宽高，同一字应该获取到的宽高应该是第二次设置的
	NDK_ScrClrs();
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(12, 12))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24, 24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth1, &getheight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	if( getwidth>=getwidth1 || getheight>=getheight1 )
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, getwidth, getwidth1, getheight, getheight1);
		GOTOERR;
	}
	if((ret=NDK_ScrDispTrueTypeFontText(0, 0, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrDispTrueTypeFontText(0, getheight1, asc_str, strlen(asc_str)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_ScrRefresh();
	NDK_KbGetCode(0,0);
	if( cls_show_msg("显示内容为%s,字体大小应该是24*24,且显示的字符之间不应该互相重叠,是[ENTER],否[其他]", asc_str) != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case7:获取不同字符串的宽高,应该不一样,如A的宽高值比a的大,j比a高,与a一样宽
	if((ret=NDK_ScrSetTrueTypeFontSizeInPixel(24, 24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(&ch1, 1, &getwidth, &getheight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(&ch2, 1, &getwidth1, &getheight1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetTrueTypeFontSize(&ch3, 1, &getwidth2, &getheight2))!=NDK_OK)
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
	NDK_ScrDestroyTrueTypeFont();
	return;
}
