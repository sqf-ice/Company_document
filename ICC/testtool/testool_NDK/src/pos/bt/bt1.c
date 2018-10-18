/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt1.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTReset是否可以正常复位
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define    TESTAPI		"NDK_BTReset"
#define MAXWAITTIME		10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt1
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt1(void) 
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, get_status = -1;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//case4:bt状态栏图标测试，未连接情况下应该不显示bt图标
#if STABAR_ENABLE
	int nKeyin = ENTER;
	nKeyin = cls_show_msg1(MAXWAITTIME,"请查看POS状态栏上的是否不显示蓝牙连接图标，是[确认]，否[其它]");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

	NDK_PortClose(PORT_NUM_BT);

#if !K21_ENABLE  //中端平台需要在open之后才能正常使用
	//case1:未打开蓝牙串口返回失败
	if((ret=NDK_BTReset())!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	if(!g_at_enable)//如果为BM77,是硬复位不测试这一点
	{
		//case2:正常连接上后进行复位,复位后蓝牙会断开连接
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//ME30MH蓝牙需要开启广播 YC1021芯片
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
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTReset())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTStatus(&get_status))!= NDK_OK || get_status!= 1)
		{
			send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, get_status);
			GOTOERR;
		}
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//case3:正常测试
	//打开蓝牙串口,进行复位之后可以进行数据的传输
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTReset())!=NDK_OK)
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
		send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if STABAR_ENABLE
	//case5:bt状态栏图标测试，连接成功后应该正常显示bt图标
	nKeyin = cls_show_msg1(MAXWAITTIME,"请查看POS状态栏上的是否正常显示蓝牙连接图标，是[确认]，否[其它]");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(!g_at_enable)//如果为BM77,是硬复位不测试这一点
	{
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//case6:蓝牙断开连接后再进行reset后看是否能正常搜索并连接
		if((ret=NDK_BTReset())!=NDK_OK)
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
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTStatus(&get_status))!= NDK_OK || get_status!= 0)
		{
			send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, get_status);
			goto ERR;
		}
	}
	send_result("%s测试通过", TESTAPI);	
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

