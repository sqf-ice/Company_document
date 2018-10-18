/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util17.c
* Author 			: jiangym
* version			:
* DATE				:  
* directory 	:
* description	: 测试NDK_Utf8ToGbk接口能否将UTF8转成GBK
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_Utf8ToGbk"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	:
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	     jiangym         20150530 	   created
*****************************************************************/
void util17(void)
{
	//以下为局部变量
	int ret = 0, i = 0;
	uchar bufin[][64] = {
						{0XE4,0XB8,0XAD,0XE6,0X96,0X87,0XE7,0X83,0XAD,0XE7,0X82,0XB9,0XE7,0X9A,0X84,0XE9,0X80,0X9A,0XE8,0XAE,0XAF},//简体中文
						{0XE7,0XB9,0X81,0XE9,0XAB,0X94,0XE5,0XAD,0X97,0XE7,0X95,0XAB,0XE5,0XBB,0XA3,0XE5,0X9C,0X8B,0XE6,0XA8,0X82},	//繁体字畫廣國樂
						{0XE4,0XB8,0X80,0XE4,0XB8,0X81,0XE4,0XB8,0X83,0XE4,0XB8,0X87,0XE4,0XBB,0XAC,0XE4,0XBB,0XB0,0XE4,0XBB,0XB2},//表格前部:一丁七万们仰仲
						{0XE5,0XA2,0XBC,0XE5,0XA3,0X81,0XE5,0XA3,0X85,0XE5,0XA3,0X91,0XE6,0XAA,0XAC,0XE6,0XAC,0XA0,0XE6,0XAF,0XB3},//中部生僻字:墼壁壅壑檬欠毳
						{0XE8,0XB7,0X84,0XE8,0XB7,0X86,0XE8,0XB7,0X8B,0XE9,0XBE,0X8C,0XE9,0XBE,0X99,0XE9,0XBE,0X9B,0XE9,0XBE,0X9F},//尾部:跄跆跋龌龙龛龟
						};
	uchar  bufin1[][64] = {"`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.",//特殊字符 
						   "test"};//英文字符
	uchar  bufout[32] = {0};
	char  *cmpbuf[32] = {"中文热点的通讯",
						 "繁體字畫廣國樂",
						 "一丁七万们仰仲",
						 "墼壁壅壑檬欠毳",
						 "跄跆跋龌龙龛龟"
						};
	uchar bufin_err[64] = {0XE4,0XB8,0XA0,0XE6,0X96,0X85,0XE7,0X8A,0XAD,0XE7,0X82,0XBE};//"中文热点"对应的编码错误
	uint bufinlen = 1, bufoutlen = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:参数异常返回失败
	if((ret=NDK_Utf8ToGbk(NULL,bufinlen,bufout,&bufoutlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_Utf8ToGbk((uchar *)"中文",bufinlen,bufout,&bufoutlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_Utf8ToGbk(bufin[0],0,bufout,&bufoutlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_Utf8ToGbk(bufin[0],bufinlen,NULL,&bufoutlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_Utf8ToGbk(bufin[0],bufinlen,bufout,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试:测试中文,繁体字,英文,特殊字符是否正常转换
	for(i=0;i<SZ_ARRAY(bufin);i++)
	{
		memset(bufout,0,sizeof(bufout));
		bufoutlen = 0;
		if((ret=NDK_Utf8ToGbk(bufin[i],strlen((char *)bufin[i]),bufout,&bufoutlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(strcmp((char *)bufout,cmpbuf[i]) || bufoutlen != strlen((char *)bufout))
		{
			send_result("line %d:%s测试失败(%d,%s,%d)", __LINE__, TESTAPI, i, bufout, bufoutlen);
			RETURN;
		}
	}
	for(i=0;i<SZ_ARRAY(bufin1);i++)
	{
		memset(bufout,0,sizeof(bufout));
		bufoutlen = 0;
		if((ret=NDK_Utf8ToGbk(bufin1[i],strlen((char *)bufin1[i]),bufout,&bufoutlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(strcmp((char *)bufout,(char *)bufin1[i]) || bufoutlen != strlen((char *)bufout))
		{
			send_result("line %d:%s测试失败(%d,%s,%d)", __LINE__, TESTAPI, i, bufout, bufoutlen);
			RETURN;
		}
	}

	//case3:编码错误的情况下，不应跑飞
	if((ret=NDK_Utf8ToGbk(bufin_err,strlen((char *)bufin_err),bufout,&bufoutlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
		
	send_result("%s测试通过", TESTAPI);
	return;
}

