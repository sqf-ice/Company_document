/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt24.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterBondConfirm能否在PINCODE和SSP模式下进行配对确认
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterBondConfirm"	
#define SBTNAME "test24"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 		: bt24
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150608		created
*****************************************************************/
void bt24(void)
{
	/*private & local definition*/
	char skey[7]={0}, smac[6]={0};
	int ret=0, status=0;
	EM_PAIRING_MODE pair_mode=0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//case1:蓝牙串口未打开，获取蓝牙配对状态或配对确认应该失败，返回值为“NDK_ERR_OPEN_DEV”	
	if((ret=NDK_BTMasterBondConfirm(skey,1))!=NDK_ERR_OPEN_DEV)
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

	//case2:未配对 调用确认配对 返回失败
	if((ret=NDK_BTMasterBondConfirm(skey,1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case3:参数非法测试
	if((ret=NDK_BTMasterBondConfirm(NULL,1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterBondConfirm(skey,2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterBondConfirm(skey,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//扫描从设备MAC地址
	cls_show_msg("请打开从设备蓝牙后，按任意键继续");
	memset(smac,0,sizeof(smac));
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:从设备在JUST WORK/PASSKEY模式下，进行配对确认应该操作失败
	cls_show_msg("从设备请[设置蓝牙配对模式]为JUST WORK模式后,进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(ret=(NDK_BTMasterBondConfirm(skey,1))!=NDK_ERR)
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//刚刚发起配对时配对模式应该返回未发起配对255，配对状态应该返回空闲状态0//秋龙说4现在是JUST WORK SC模式，未发起配对对应的值是255
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,skey,&status))!=NDK_OK||pair_mode!=255||status!=0)
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, pair_mode, status);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"配对成功,子用例测试通过");
	
	//case5.1:从设备在SSP配对模式下，进行确认配对预期提示配对成功
	cls_show_msg("从设备请[设置蓝牙配对模式]为SSP模式后,进入[等待配对]后按任意键继续,主设备进行确认配对操作");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
	 	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//cls_show_msg("在提示是否确认配对时,主从设备都按确认配对,任意键继续");
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"配对成功,子用例测试通过");
	
	//case5.2:从设备在SSP配对模式下，进行取消配对预期提示配对失败
	cls_show_msg("从设备请[设置蓝牙配对模式]为SSP模式后,进入[等待配对]后按任意键继续,主设备进行取消配对操作");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
	 	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_btmaster_pairing(0))!=3)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"配对失败,子用例测试通过");

	//case6:正常测试:从设备选择passkey模式，从设备输入正确key预期会提示配对成功
	cls_show_msg("从设备请[设置蓝牙配对模式]为passkey模式后,进入[等待配对]后按任意键继续,从设备输入正确的KEY");
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
	cls_show_msg1(2,"配对成功,子用例测试通过");

	//case7:正常测试:从设备选择passkey模式，从设备输入错误key预期会提示配对失败
	cls_show_msg("从设备请[设置蓝牙配对模式]为passkey模式后,进入[等待配对]后按任意键继续,从设备输入错误的KEY");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_btmaster_pairing(1))!=3)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	cls_show_msg1(2,"配对失败,子用例测试通过");
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}

