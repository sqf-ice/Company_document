/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt19.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetType 能否正确设置蓝牙类型
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetType"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt19
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20150319		created
*****************************************************************/
void bt19(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	/*private & local definition*/
	int ret = -1;		
	char ph_type[3]={0x0c,0x02,0x5a}, ear_type[3]={0x04,0x04,0x24};
	char outbuf[16] = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1:未打开蓝牙串口进行设置应该返回失败(实际不是这样)
	//开发解释:开机第一次 OPEN之前 调用失败(未下patch)接下来OPEN之前调用 都会成功.
	//因为在此之前由于进行了设置调用过open了 这里在未open前调用就会成功了
	if((ret=NDK_BTSetType(ph_type))!=/*NDK_ERR_IOCTL*/NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, outbuf);
		GOTOERR;
	}
	//case2:参数为NULL 应该返回失败
	if((ret=NDK_BTSetType(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:设置为手机图标那么搜索时显示的应该为手机图标	
	if((ret=NDK_BTSetType(ph_type))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("在手机设置中搜索蓝牙:%s的图标是否为手机图标,是[Enter],否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:设置为耳机图标那么搜到时显示的应该为耳机图标
	if((ret=NDK_BTSetType(ear_type))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("在手机设置中重新搜索蓝牙:%s的图标是否为耳机图标,是[Enter],否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case5:测试最后一次设置有效,先设置耳机,再设置手机,应该搜到的手机图标
	if((ret=NDK_BTSetType(ear_type))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTSetType(ph_type))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("在设置中重新搜索蓝牙:%s的图标是否为手机图标,是[Enter],否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}


