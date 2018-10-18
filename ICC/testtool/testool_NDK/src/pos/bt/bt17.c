/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt17.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetBleStatus 能否正确设置是否支持BLE连接
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetBleStatus"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt17
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20150319		created
*****************************************************************/
void bt17(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	/*private & local definition*/
	int ret = -1;	  
	char outbuf[16] = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//case1:蓝牙串口未打开 设置蓝牙是否支持ble连接返回失败
	/*由于开发解释，必须在第一次蓝牙串口打开之前测试，因此已经移植到bt.c中*/
/*	if((ret=NDK_BTSetBleStatus(0))!=NDK_ERR_IOCTL)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return;
	}*/
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

	//case2:参数错误应该返回失败
	if((ret=NDK_BTSetBleStatus(2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:关闭BLE后使用lightbule 搜索时应该搜索不到
	if((ret=NDK_BTSetBleStatus(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("使用LightBlue应该搜索不到POS蓝牙:%s,是[Enter],否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:开启BLE后使用lightbule 搜索时应该能搜索到	
	if((ret=NDK_BTSetBleStatus(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("使用LightBlue应该搜索到POS蓝牙:%s,是[Enter],否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
		
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}


