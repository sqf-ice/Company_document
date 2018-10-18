/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: scr24.c
* Author 			: jiangym
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试 NDK_ScrLoadBDF和NDK_ScrDispBDFText接口功能
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		 "NDK_ScrLoadBDF,NDK_ScrDispBDFText"
#define 	BDFFILE		 "10x20.bdf"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513
#define 	BDFFILE1	 "18x20.bdf"
#define 	ERRFILE		 "err.bdf"
#define     MAXWAITTIME   10//30
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr24
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	    jiangym        20130224  	   created
*
*****************************************************************/
void scr24(void)
{
	/*private & local definition*/
	int ret = 0;
	uint pw = 0, ph = 0, pw1 = 0, ph1 = 0, x=0, y=0;
	ushort text[] = {0x0023, 0x0033, 0x0043, 0x0066,0x0000};//对应BDF字符#3Cf
	ushort text1[] = {0x0024, 0x0034, 0x0044, 0x0064,0x0000};//对应BDF字符$4Dd
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);
	
	if(NDK_FsExist(BDFFILE)!=NDK_OK&&NDK_FsExist(BDFFILE1)!=NDK_OK)
	{
		send_result("%s测试bdf字库不存在,请在POS的测试程序路径目录下载后重新测试", TESTAPI);
		return;
	}
	
	//NDK_ScrLoadBDF
	//case1:异常测试:BDF路径参数为NULL情况或者文件不存在时,应返回参数错误
	if((ret=NDK_ScrLoadBDF(0,NULL,&pw,&ph))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLoadBDF(1,ERRFILE,&pw,&ph))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:测试punWidth和punHeight支持NULL,返回成功
	if((ret=NDK_ScrLoadBDF(0,BDFFILE,NULL,&ph))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrLoadBDF(0,BDFFILE,&pw,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:测试第一个参数为-1,返回成功,该用例只为提高测试覆盖度
	if((ret=NDK_ScrLoadBDF(-1,BDFFILE,&pw,&ph))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//NDK_ScrDispBDFText
	//case4:测试unFontID BDF字体未加载
	if((ret=NDK_ScrDispBDFText(60000,x,y,text))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispBDFText(0,x,y,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case5:越界测试:unX和unY越界，返回失败
	if((ret=NDK_ScrDispBDFText(0,ViewWidth+1,ViewHeight,text))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispBDFText(0,ViewWidth,ViewHeight+1,text))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case6:正常测试:加载BDF字体后，使用加载后的BDF字体显示
	if((ret=NDK_ScrLoadBDF(0,BDFFILE,&pw,&ph))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pw!=10 || ph!=20)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, pw, ph);
		RETURN;
	}
	if((ret=NDK_ScrLoadBDF(1,BDFFILE1,&pw1,&ph1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pw1!=18 || ph1!=20)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, pw1, ph1);
		RETURN;
	}
	//输出ID0的BDF字体
	NDK_ScrClrs();
	if((ret=NDK_ScrDispBDFText(0,x,y,text))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//刷新
	NDK_ScrRefresh();
	sleep(3);
	//增加人工确认输出的字体是否正确
	if(ENTER != cls_show_msg1(MAXWAITTIME, "屏幕上显示:#3Cf,按[确认]测试通过,[其它]失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//输出ID1的BDF字体
	NDK_ScrClrs();
	if((ret=NDK_ScrDispBDFText(1,x,y,text1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//刷新
	NDK_ScrRefresh();
	sleep(3);
	//增加人工确认输出的字体是否正确
	if(ENTER != cls_show_msg1(MAXWAITTIME, "屏幕上显示:$4Dd,按[确认]测试通过,[其它]失败")&&auto_flag!=1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

