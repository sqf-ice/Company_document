/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt23.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterGetBondStatus能否获取到正确的蓝牙配对状态
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterGetBondStatus"	
#define SBTNAME "test23"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt23
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150605		created
*****************************************************************/
void bt23(void)	//无参数无返回
{
	/*private & local definition*/
	char skey[7]={0};
	int status=0,ret=0;
	char smac[6]={0};
	EM_PAIRING_MODE pair_mode=0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//case1:蓝牙串口未打开，获取蓝牙配对状态应该失败
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,skey,&status))!=NDK_ERR_OPEN_DEV)
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
	
	//case2:未进行主动配对，获取配对状态成功，但获取状态应为空闲状态(0)，获取到的配对模式应该为255//秋龙说4现在是JUST WORK SC模式，未发起配对对应的值是255
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,skey,&status))!=NDK_OK||pair_mode!=255||status!=0)
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, pair_mode, status);
		GOTOERR;	
	}
	
	//case3:参数非法测试
	if((ret=NDK_BTMasterGetBondStatus(NULL,skey,&status))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,NULL,&status))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,skey,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//扫描并选择测试设备
	cls_show_msg("请打开从设备蓝牙后，按任意键继续");
	memset(smac,0,sizeof(smac));
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case4:正常测试:从设备的配对模式为JUST WORK 获取配对模式应与实际配对模式一致 且获取配对成功的状态
	cls_show_msg("从设备请[设置蓝牙配对模式]为JUST WORK模式后,进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//刚刚发起配对,未收到配对请求时,配对模式应该返回未发起配对255，配对状态应该返回空闲状态0
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

	//case5.1:正常测试:从设备的配对模式为PIN CODE 输入错误的pin码且确认配对
	cls_show_msg("从设备请[设置蓝牙配对模式]为PIN CODE模式后,进入[等待配对]后按任意键继续,配对时主设备请输入错误的PINCODE");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_btmaster_pairing(1))!=3)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2,"配对失败,子用例测试通过");
	
	//case5.2:从设备的配对模式为PIN CODE 输入正确的pin码且确认配对
	cls_show_msg("从设备请[设置蓝牙配对模式]为PIN CODE模式后,进入[等待配对]后按任意键继续,配对时主设备请输入正确的PINCODE");
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
	
	//case5.3:从设备的配对模式为PIN CODE  输入正确的pin码但取消配对
	cls_show_msg("从设备请[设置蓝牙配对模式]为PIN CODE模式后,进入[等待配对]后按任意键继续,配对时主设备不输入PIN直接按取消");
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

	//case6:取消配对后配对状态应该是空闲状态
	if((ret=NDK_BTMasterGetBondStatus(&pair_mode,skey,&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, status);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}

