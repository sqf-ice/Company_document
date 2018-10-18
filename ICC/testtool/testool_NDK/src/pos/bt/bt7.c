/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt7.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_PortOpen能否初始化打开蓝牙,NDK_PortClose能否正常关闭蓝牙
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"蓝牙开关"
#define   MAXWAITTIME		10
#define   BPS_NUM		8
#define   MAX_SIZE    BUFSIZE_BT-MIN_BTCMD_LEN

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt7
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt7(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1;
#if !K21_ENABLE
	int j = 0;
	char *bps[BPS_NUM] ={ "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400"};//"300", "1200",
#endif

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

#if !K21_ENABLE
	//未打开蓝牙串口使用手机蓝牙助手进行搜索POS应搜不到
	//前提是需要把之前配对的取消,没取消即使是连接也连不上
	/*//本用例移到systest57 abnormal_test6 20151217 linwl
	cls_show_msg("把先前已配对的POS取消,取消后按任意键继续");
	if(cls_show_msg("使用手机蓝牙搜索%s,应搜索不到,正确[Enter],错误[其他]",BtName)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}*/
	//case1:应提供对蓝牙串口波特率设置功能
	cls_printf("测试蓝牙串口波特率设置中");
	for (; j<BPS_NUM; j++)
	{
		if((ret=NDK_PortOpen(PORT_NUM_BT, bps[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if((ret = NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
		{				
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
#endif

#if K21_ENABLE
	//case2:异常参数测试(此处无效)
	if((ret=NDK_PortOpen(PORT_NUM_BT, "115201"))!=NDK_OK)
#else
	if((ret=NDK_PortOpen(PORT_NUM_BT, "115201"))!=NDK_ERR_PARA)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_BT);
	
	//case3:正常测试，打开后可以成功读写数据
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
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

