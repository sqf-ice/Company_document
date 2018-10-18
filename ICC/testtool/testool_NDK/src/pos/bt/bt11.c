/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt11.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTDisconnect是否可以正常断了蓝牙连接
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTDisconnect"
#define  MAXWAITTIME 10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt11
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20141118		created
*****************************************************************/
void bt11(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, get_status=0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

#if !K21_ENABLE
	//case1:蓝牙串口未打开进行disconnect，应该失败
	if((ret=NDK_BTDisconnect())!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//ME30MH蓝牙需要开启广播
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(2);
	if((ret=NDK_BTStatus(&get_status))!= NDK_OK || get_status!= 1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, get_status);
		GOTOERR;
	}
	sleep(3);//给手机足够的时间去断开连接
	/*
	if(cls_show_msg("此时蓝牙连接已断开,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	*/
	
	//case3:测试断开后使用手机扫描还能够连接上
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

