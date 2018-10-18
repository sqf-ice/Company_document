/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: scr22.c
* Author 			: jiangym
* version			: 
* DATE			: 20130218
* directory 		: 
* description		: 测试 NDK_ScrFontAdd 能否正确添加自定义字体
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScrFontAdd"
//黑白屏自定义字库
#define     PCPATH      "pcc16x16.bin"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513
#define     PAPATH      "pca8x16.bin"
//彩屏自定义字库
#define     PCPATHCP    "hz24x24"
#define     PAPATHCP    "yw12x24"
#define     MAXWAITTIME 10//30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr22
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	    jiangym         20130224  	   created
*
*****************************************************************/
void scr22(void)
{
	/*private & local definition*/
	int ret = 0,i = 0;
	uint punWidth = 0, punHeight = 0, puncd = 0;
	char cPath[64] = {0}, aPath[64] = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)//获取屏幕类型作为后续测试的条件
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if(puncd!=16)
	{
		if(NDK_FsExist(PCPATH)!=NDK_OK&&NDK_FsExist(PAPATH)!=NDK_OK)
		{
			send_result("%s字库文件不存在,请在POS的测试程序路径目录下载自定义点阵字库重新测试", TESTAPI);
			return;
		}
		strcpy(cPath,PCPATH);//测试黑白屏字库
		strcpy(aPath,PAPATH);
	}
	else
	{
		if(NDK_FsExist(PCPATHCP)!=NDK_OK&&NDK_FsExist(PAPATHCP)!=NDK_OK)
		{
			send_result("%s字库文件不存在,请在POS的测试程序路径目录下载自定义点阵字库重新测试", TESTAPI);
			return;
		}
		strcpy(cPath,PCPATHCP);//测试彩屏屏字库
		strcpy(aPath,PAPATHCP);
	}
	
	//测试前置:	
	//case1:参数异常:pcCpath和pcApath不能同时为NULL
	if((ret=NDK_ScrFontAdd(NULL,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:pcCpath和pcApath单独一个为NULL，返回应该成功
	if((ret=NDK_ScrFontAdd(NULL,aPath))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrFontAdd(cPath,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
			
	//case3:添加用户自定义字体，同时设置用户自定义字体,并获取字体的宽和高:自定义模式暂不支持,先用额外模式支持
	if((ret=NDK_ScrFontAdd(cPath,aPath))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrSetFontType(DISPFONT_USER))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(puncd!=16)
	{
		//黑白屏预期结果
		if(punWidth != 16 || punHeight != 16)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
			GOTOERR;
		}
		if(cls_show_msg1(MAXWAITTIME, "显示扩展字体,汉字是否为16X16:[确认]是,[其他]否")!=ENTER&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	else
	{
		//彩屏预期结果
		if(punWidth != 24 || punHeight != 24)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
			GOTOERR;
		}
		if(cls_show_msg1(MAXWAITTIME, "显示扩展字体,汉字是否为24X24:[确认]是,[其他]否")!=ENTER&&auto_flag!=1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}	
	
	//case4:测试添加用户自定义字体后再修改成默认字体,后面显示应全为默认字体
	if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=0;i<4;i++)  
	{
		if(puncd!=16)
		{
			if(cls_show_msg1(MAXWAITTIME, "第%d次显示默认字体,汉字是否为12X12[确认]是,[其他]否",i+1)!=ENTER&&auto_flag!=1)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
		else
		{
			if(cls_show_msg1(MAXWAITTIME, "第%d次显示默认字体,汉字是否为16X16[确认]是,[其他]否",i+1)!=ENTER&&auto_flag!=1)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
	}

	send_result("%s测试通过", TESTAPI);
	//恢复默认设置
	goto ERR;
ERR:
	NDK_ScrSetFontType(DISPFONT_CUSTOM);
	return;
}

