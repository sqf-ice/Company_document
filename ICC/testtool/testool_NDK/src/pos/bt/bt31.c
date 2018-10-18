/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt31.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙NDK_BTMasterConnect主动连接及回连功能(海外版本)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterConnect"
#define MAXWAITTIME  10000  //单位ms  即10s
#define TIMEOUT  2  //2s
#define	WUCHASEC	0.03 //这个值待定

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt31
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  
*****************************************************************/
void bt31(void)	 
{
	/*private & local definition*/
	int ret=0,status=0,i=0;
	/*这里需要去获取手机的mac 存入
	mac1为红米2手机的蓝牙mac  0x20,0x82,0xc0,0xc5,0x35,0x0c,     mac2为我自己手机的Mac 0x60,0xA3,0x7D,0x1E,0xC2,0x20
	mac 倒着写0x0c,0x35,0xc5,0xc0,0x82,0x20     iphone mac  0xD9,0x1C,0x49,0xAF,0x3B,0xBC*/
	char ph_mac1[6]={0x0c,0x35,0xc5,0xc0,0x82,0x20},ph_mac2[6]={0x20,0xC2,0x1E,0x7D,0xA3,0x60};
	char err_mac[5]={0x00,0x11,0x22,0x33,0x44},unpair_mac[6]={0x00,0x11,0x22,0x33,0x44,0x55};
	float tmp = 0.0;
	char str[32] = {0};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1:蓝牙串口未打开，主动连接失败，获取主动连接状态失败
	if((ret=NDK_BTMasterConnect(ph_mac1,1,MAXWAITTIME))!=NDK_ERR_OPEN_DEV)//-1
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
	
	//case2:主动连接一个未配对的mac地址的设备 应该返回失败
	if((ret=NDK_BTMasterConnect(unpair_mac,0,MAXWAITTIME))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	/* 
	能连接成功的前置条件:
	先设置记录手机个数为2,然后使用2部手机分别与pos发起配对,记录这2部手机的mac   
	*/
	if((ret=NDK_BTSetMaxBondedDevices(2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("准备2部手机分别与pos进行连接,任意键继续");
	for(i=0;i<2;i++)
	{
		cls_show_msg1(3, "使用第%d部手机配对中...", i+1);//这里使用的工具需要验证后再确认
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_BTDisconnect(); 
	}
	
	//case3:参数非法测试包括参数不支持和参数错误以及mac格式错误
	if((ret=NDK_BTMasterConnect(ph_mac1,2,MAXWAITTIME))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(ph_mac1,-1,MAXWAITTIME))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(ph_mac1,3,MAXWAITTIME))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterConnect(err_mac,1,MAXWAITTIME))!=NDK_ERR ) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case4:测试mac传入为NULL,是连接最后一次连接的手机(预期也是连上第二部手机)
	if((ret=NDK_BTMasterConnect(NULL,0,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//判断状态是已连接状态
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//需要增加人工判断是否连接的是第二部手机
	if(cls_show_msg("与pos连接上的手机是否为第二部手机,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//判断状态是未连接状态
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	
	//case5:主动连接一个正确的mac地址的设备(mac为第一部手机)应该能够连接成功
	//回连第一部手机
	if((ret=NDK_BTMasterConnect(ph_mac1,1,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//判断状态是已连接状态
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
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

	//case6:超时时间太短的情况下 回连是否会不成功,比如设置超时时间为0
	if((ret=NDK_BTMasterConnect(NULL,1,0))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case7:超时测试，准确性测试 (把手机拿走)
	cls_show_msg("把第二部手机拿远后任意键继续...");
	lib_StartStopwatch();
	if(((ret=NDK_BTMasterConnect(ph_mac2,0,TIMEOUT*1000))!=NDK_ERR_TIMEOUT)||((tmp=lib_ReadStopwatch()-TIMEOUT)>WUCHASEC))
	{
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, str);
		GOTOERR;	
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

