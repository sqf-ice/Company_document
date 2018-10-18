/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt25.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterConnect和NDK_BTMasterGetConStatus能否主动连接和获取连接状态
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterConnect,NDK_BTMasterGetConStatus"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt25
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150609		created
*****************************************************************/
//以下为本用例入口函数
void bt25(void)	//无参数无返回
{
	/*private & local definition*/
	char ble_mac[6]={0},
		 spp_mac[6]={0},
		 smac[6]={0};
	int ret=0,dev_type=0,c_status=0,i=0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1:蓝牙串口未打开，主动连接失败，获取主动连接状态失败
	if((ret=NDK_BTMasterConnect(ble_mac,dev_type))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;	
	}
	if((ret=NDK_BTMasterGetConStatus(ble_mac,&c_status))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;	
	}
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	//扫描测试样机，构造测试数据，例如准备3台设备分别是双模设备、单模BLE设备、单模SPP设备
	//扫描后将MAC分别传递给ble_mac、spp_mac、smac
	memset(spp_mac,0,sizeof(spp_mac));
	memset(smac,0,sizeof(smac));
	
	cls_show_msg("请打开单模BLE的蓝牙从设备,按任意键继续");
	if(lib_btmaster_scan(ble_mac, 2)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg("请打开单模SPP的蓝牙从设备,按任意键继续");
	if(lib_btmaster_scan(spp_mac, 1)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg("请打开双模的蓝牙从设备,按任意键继续");
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	
	//绑定3台设备
	cls_show_msg("单模BLE从设备选择配对模式后按任意键继续");
	if((ret=NDK_BTMasterBond(ble_mac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
/*	if((ret=lib_btmaster_pairing(1))!=2)  //BLE无需配对
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}*/

	cls_show_msg("单模SPP从设备选择配对模式后按任意键继续");
	if((ret=NDK_BTMasterBond(spp_mac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	cls_show_msg("双模从设备请[设置蓝牙配对模式]后,进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:参数非法测试
	if((ret=NDK_BTMasterConnect(NULL,dev_type))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(smac,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(smac,3))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetConStatus(NULL,&c_status))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetConStatus(smac,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case3:未主动连接，获取到的状态应为非连接
	if((ret=NDK_BTMasterGetConStatus(smac,&c_status))!=NDK_OK||c_status!=1)//返回ret=31
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,c_status);
		GOTOERR;	
	}
	
	//case4:MAC和连接方式同设备支持的模式不匹配情况下连接成功，但是连接状态应为未连接
	if((ret=NDK_BTMasterConnect(ble_mac,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(1);//连接需要时间,不管是否能够连接上,之后再判断状态
	if((ret=lib_btmaster_getconstatus(ble_mac, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(spp_mac,2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(1);//连接需要时间,不管是否能够连接上,之后再判断状态
	if((ret=lib_btmaster_getconstatus(spp_mac, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//保护动作 (对可能连接上的蓝牙设备进行断开,以防对后续的影响)
	NDK_BTMasterDisconnect(ble_mac);
	NDK_BTMasterDisconnect(spp_mac);
	
	//case6:本测试从设备使用双模设备(如果双模设备可以正常工作，单模应该也正常)，双模时各种连接方式都应该成功，从设备连接的配对方式可以选择4种中的任意三种
	for(i=0;i<3;i++)
	{	//i=2测试结果 配对成功，但连接超时 ，超时后退出后续再测试偶尔会出现其它函数返回-1的情况
		cls_show_msg("双模从设备请[设置蓝牙配对模式]后,进入[等待配对]后按任意键继续(连接方式:%d)",i);
		if(lib_btmaster_scan(smac, i)!=NDK_OK)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
		if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
		if(i!=2)  //i=2为BLE方式,BLE不需要配对,本司产品无配对 ,陈秋龙解释  20170704
		{
			if((ret=lib_btmaster_pairing(1))!=2)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
		if((ret=NDK_BTMasterConnect(smac,i))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
		if((ret=lib_btmaster_getconstatus(smac, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
		if((ret=NDK_BTMasterDisconnect(smac))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTMasterDisconnect(smac);
	NDK_PortClose(PORT_NUM_BT);
	return;
}


