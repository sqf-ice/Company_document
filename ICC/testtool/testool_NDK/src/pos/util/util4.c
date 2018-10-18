/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util4.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_AscToHex 和NDK_HexToAsc     接口功能
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
#define 	TESTAPI		"NDK_AscToHex,NDK_HexToAsc "

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
* history 		 		:
*****************************************************************/
void util4(void)
{
	//以下为局部变量
	uchar asciibuf[13] = {0}, asciibuf2[13] = {0}, bcdbuf[7] = {0};
	int len = 8, ret = 0;
	uchar type = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_AscToHex(NULL,len,type,bcdbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AscToHex((uchar *)"30313233",-1,type,bcdbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AscToHex((uchar *)"30313233",len,-1,bcdbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AscToHex((uchar *)"30313233",len,type,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_HexToAsc(NULL,len,type,asciibuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_HexToAsc((uchar *)"13ab",-1,type,asciibuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_HexToAsc((uchar *)"13ab",4,-1,asciibuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_HexToAsc((uchar *)"13ab",4,type,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:测试字符串为偶数情况下,使用0模式
	memset(asciibuf,0,sizeof(asciibuf));
	memcpy(asciibuf,"3039617A415A",strlen("3039617A415A"));//注意:字符串必须是大写字符串,否则经过两次转换之后小写字符会变成大写字符例如：asciibuf="7a",asciibuf2="7A"
	memset(bcdbuf,0,sizeof(bcdbuf));
	if((ret=NDK_AscToHex(asciibuf,strlen((char *)asciibuf),0,bcdbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("09azAZ",(char *)bcdbuf))//转换后bcdbuf = {0x30,0x39,0x61,0x7a,0x41,0x5a}对应字符"09azAZ"
	{
		send_result("line %d:%s测试失败,bcdbuf=%s", __LINE__, TESTAPI, bcdbuf);
		RETURN;
	}
	memset(asciibuf2,0,sizeof(asciibuf2));
	if((ret=NDK_HexToAsc(bcdbuf,12,0,asciibuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp((char *)asciibuf,(char *)asciibuf2))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:测试字符串为偶数情况下,使用1模式(有BUG)
	memset(bcdbuf,0,sizeof(bcdbuf));
	if((ret=NDK_AscToHex(asciibuf,strlen((char *)asciibuf),1,bcdbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("09azAZ",(char *)bcdbuf))//转换后bcdbuf = {0x30,0x39,0x61,0x7a,0x41,0x5a}对应字符"09azAZ"
	{
		send_result("line %d:%s测试失败,bcdbuf=%s", __LINE__, TESTAPI, bcdbuf);
		RETURN;//此处有BUG：asciibuf经过1模式转换后输出的bcdbuf变成"???039617A415A"
	}
	memset(asciibuf2,0,sizeof(asciibuf2));
	if((ret=NDK_HexToAsc(bcdbuf,12,1,asciibuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp((char *)asciibuf2,(char *)asciibuf))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case4:测试字符串为奇数情况下,使用0模式(有BUG)
	memset(asciibuf,0,sizeof(asciibuf));
	memcpy(asciibuf,"50524",strlen("50524"));
	memset(bcdbuf,0,sizeof(bcdbuf));
	if((ret=NDK_AscToHex(asciibuf,strlen((char *)asciibuf),0,bcdbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("PR@",(char *)bcdbuf))//转换后bcdbuf = {0x50,0x52,0x40}对应字符"PR@"
	{
		send_result("line %d:%s测试失败,bcdbuf=%s", __LINE__, TESTAPI, bcdbuf);
		RETURN;//此处有BUG，返回bcdbuf对应字符串为"PR@"
	}
	memset(asciibuf2,0,sizeof(asciibuf2));
	if((ret=NDK_HexToAsc(bcdbuf,6,0,asciibuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp((char *)asciibuf2,"505240"))//左对齐右补0
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case5:测试字符串为奇数情况下,使用1模式
	memset(bcdbuf,0,sizeof(bcdbuf));  
	if((ret=NDK_AscToHex(asciibuf,strlen((char *)asciibuf),1,bcdbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp("$",(char *)bcdbuf))//转换后bcdbuf = {0x05,0x05,0x24}对应字符"$"
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	memset(asciibuf2,0,sizeof(asciibuf2));
	if((ret=NDK_HexToAsc(bcdbuf,6,1,asciibuf2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp((char *)asciibuf2,"050524"))//右对齐左补0
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
      
	send_result("%s测试通过", TESTAPI);
	return;
}

