/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt22.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterBond能否进行主动配对
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterBond"	//待测API或API对的名字（当是API对，格式：”api1/api2”）

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt22
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150605		created
*****************************************************************/
//以下为本用例入口函数
void bt22(void)	//无参数无返回
{
	/*private & local definition*/
	char smac[6]={0}, //smac[6]={0x08,0x7a,0x4c,0x08,0xd8,0xb9},//从设备的MAC地址
		 smac1[6]={0x11,0x22,0x33,0x44,0x55,0x66};
	int ret=0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//case1:蓝牙串口未打开，获取连接结果应该失败	
	if((ret=NDK_BTMasterBond(smac))!=NDK_ERR_OPEN_DEV)
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
	
	//case2:参数非法测试：配对的设备mac地址为null，为7个字符或5个字符，则获取连接结果应该失败
	if((ret=NDK_BTMasterBond(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:给定一个不是扫描出来的设备的MAC地址，则操作失败。
	if((ret=NDK_BTMasterBond(smac1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_btmaster_pairing(1))!=3)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:配对一个不存在的MAC然后再case5中配对一个存在的MAC后获取配对状态的初始状态应该为0，不应该出现初始状态为3的情况
	if((ret=NDK_BTMasterBond(smac1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:正常测试:选择特定的MAC地址的从模式，配对模式设为任意一种，进行正常操作
	//****配对iPhone手机,同时把iPhone手机的mac地址传给smac,然后验证是否能够配对成功 
	//****若成功后可以使用扫描指定名称的从设备,然后把mac地址传给smac进行配对
	cls_show_msg("请打开从设备蓝牙后，按任意键继续");
	memset(smac,0,sizeof(smac));
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("从设备请[设置蓝牙配对模式]后进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试后置
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}
