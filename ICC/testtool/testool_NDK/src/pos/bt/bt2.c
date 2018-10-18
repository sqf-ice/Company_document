/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt2.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTEnterCommand,NDK_BTExitCommand是否可以正常进出命令模式
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define    TESTAPI		"NDK_BTEnterCommand,NDK_BTExitCommand"
#define MAXWAITTIME		10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt2
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt2(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = 0, nKeyin = ENTER;
	char pinbuf[7] = {0}, namebuf[15] = {0};//pin密码是6位,在国芯ME30测试中发现如果缓冲区只有6会有问题,需要增加一字节的缓冲区20170801

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//打开串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:BM77在进入命令模式下应搜索不到蓝牙,支持AT命令模式的BT模块情况相同,其它模块能搜索到
	if(g_at_enable)
	{
		cls_show_msg1(MAXWAITTIME,"取消之前已配对的POS后,任意键继续");
		nKeyin = cls_show_msg1(MAXWAITTIME,"使用手机蓝牙搜索POS蓝牙s%应搜索不到,正确[Enter],错误[其他]", BtName);
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
	}

	//case2:进入命令模式 可以进行PIN 的获取等操作
#if !BTPCI_ENABLE
	memset(pinbuf,0,sizeof(pinbuf));
	if((ret=NDK_BTGetPIN(pinbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	memset(namebuf,0,sizeof(namebuf));
	if((ret=NDK_BTGetLocalName(namebuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(BtName, namebuf, strlen(namebuf)) && memcmp(BtPin, pinbuf, strlen(pinbuf)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#if !BTPCI_ENABLE
	if((ret=NDK_BTSetPIN(pinbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if((ret=NDK_BTSetLocalName(namebuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:退出命令模式即进入数据模式,可进行数据间的传输
	if((ret=NDK_BTExitCommand())!=NDK_OK)
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
		send_result("line %d:%s测试配对失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	send_result("%s测试通过", TESTAPI);	
ERR:
	NDK_BTExitCommand();
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

