/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt13.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetPairingMode是否能够正确的设置蓝牙连接模式
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetPairingMode"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt13
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20141118		created
*****************************************************************/
void bt13(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = 0, tempairmode = g_pair_mode;
	
	/*process body*/
	//BLE只有just work 模式,在平常测试中验证即可
	if(cls_show_msg("测试%s...如果是苹果手机6s以下版本测试,此案例不测,[ESC]返回,[其他]继续", TESTAPI)==ESC)
	{
		send_result("%s测试,苹果手机6s以下不支持just work以外的模式,用户取消测试", TESTAPI);
		return;
	}
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
#if !K21_ENABLE
	//case1:蓝牙串口未打开 设置失败
	if((ret=NDK_BTSetPairingMode(PAIRING_MODE_PINCODE))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//BM77未进入命令模式 设置失败	
	if(g_at_enable)
	{
		if((ret=NDK_BTSetPairingMode(PAIRING_MODE_PINCODE))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	// 进入命令模式后才能进行设置
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:参数异常返回失败
	if((ret=NDK_BTSetPairingMode(PAIRING_MODE_PASSKEY+2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
//PCI认证版本不支持just work模式
#if JUSTWORK_ENABLE
	//case3:不同的模式情况不同	BLE协议不测
	//case3.1:just work mode 直接连上
	//cls_show_msg("手机端在设置中把之前配对的蓝牙取消后任意键继续");
	//cls_printf("JUSTWORK测试中,请使用手机蓝牙测试程序扫描连接POS蓝牙%s",BtName)
	g_pair_mode=PAIRING_MODE_JUSTWORK;
	if((ret=NDK_BTSetPairingMode(g_pair_mode))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTExitCommand())!=NDK_OK)//BM77需要在退出命名模式情况能搜到蓝牙
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//蓝牙外挂flash的新方案增加用例测试重新open后蓝牙配对模式变成旧名称的问题 20160728 linwl
	NDK_PortClose(PORT_NUM_BT);
	NDK_PortOpen(PORT_NUM_BT, NULL);
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
		send_result("line %d:配对失败,请重新配对(%d)", __LINE__, ret);
		GOTOERR;
	}
	if(cls_show_msg("手机与POS是否是直接连上,是[Enter]继续,否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	
#if !JUSTWORK_FLAG
//新增pincode标志位蓝牙:芯片20707-A2没有pincode模式
#if PINCODE_ENABLE
	//case3.2:pincode mode  需要输pin
	//cls_show_msg("手机端在设置中把之前配对的蓝牙取消后任意键继续");
	if((ret=NDK_BTEnterCommand())!=NDK_OK)//BM77要在进入命令模式情况下能设置
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	g_pair_mode=PAIRING_MODE_PINCODE;
	if((ret=NDK_BTSetPairingMode(g_pair_mode))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTExitCommand())!=NDK_OK)//BM77需要在退出命名模式情况能搜到蓝牙
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//蓝牙外挂flash的新方案增加用例测试重新open后蓝牙配对模式变成旧名称的问题 20160728 linwl
	NDK_PortClose(PORT_NUM_BT);
	NDK_PortOpen(PORT_NUM_BT, NULL);
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
		send_result("line %d:配对失败,请重新配对(%d)", __LINE__, ret);
		GOTOERR;
	}
	if(cls_show_msg("手机与POS连接时有PIN输入框且要输入PIN,是[Enter]继续,否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	NDK_BTEnterCommand();//与最后的退出配对使用
	if(!g_at_enable)//BM77无2 3 4 模式
	{
		//case3.3:SSP mode
#if BTPCI_ENABLE
		cls_show_msg("手机端在设置中把之前配对的蓝牙取消后任意键继续");
#endif
		g_pair_mode=PAIRING_MODE_SSP;
		if((ret=NDK_BTSetPairingMode(g_pair_mode))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:配对失败,请重新配对(%d)", __LINE__, ret);
			GOTOERR;
		}
		if(cls_show_msg("手机显示SSP配对码,且要确认配对,是[Enter]继续,否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case3.4:Passkey mode
		//此处有进行了模式切换,应该不需要提示取消配对
		//cls_show_msg("手机端在设置中把之前配对的蓝牙取消后任意键继续");
		g_pair_mode=PAIRING_MODE_PASSKEY;
		if((ret=NDK_BTSetPairingMode(g_pair_mode))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:配对失败,请重新配对(%d)", __LINE__, ret);
			GOTOERR;
		}
		if(cls_show_msg("手机端显示PASSKEY配对码,POS需要输入配对码,是[Enter]继续,否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}

#if JUSTWORK_SC_ENABLE
		if(cls_show_msg("如果是安卓手机测试,请按[ESC]返回,苹果6s以上手机测试按[其他]继续")==ESC)
		{ 
			send_result("%s测试,安卓手机和苹果手机6s以下版本手机不支持just work and sc模式,用户取消测试", TESTAPI);
			GOTOERR;
		}
		else
		{
			//case3.5:just work and sc模式有配对对话框,手机需要按配对确认
			g_pair_mode=PAIRING_MODE_JUSTWORK_SC;
			if((ret=NDK_BTSetPairingMode(g_pair_mode))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if((ret=lib_bt_pairing(1))!=NDK_OK)
			{
				send_result("line %d:配对失败,请重新配对(%d)", __LINE__, ret);
				GOTOERR;
			}
			if(cls_show_msg("手机显示配对对话框,且要确认配对,是[Enter]继续,否[其他]")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
			if((ret=NDK_BTDisconnect())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
#endif
	}
#endif

	send_result("%s测试通过", TESTAPI);	
ERR:
	//恢复设置中的设置
	g_pair_mode = tempairmode;
	NDK_BTSetPairingMode(g_pair_mode);
	//退出命令模式
	NDK_BTExitCommand();
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

