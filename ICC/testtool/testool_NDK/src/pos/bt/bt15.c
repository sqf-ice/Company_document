/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt15.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetDiscoverableStatus是否可以正确设置后是否能搜索到蓝牙
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetDiscoverableStatus"
#define  MAXWAITTIME 10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt15
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20150205		created
*****************************************************************/
void bt15(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	/*private & local definition*/
	int ret = -1, status= -1;	  
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1:BM77蓝牙模块不支持，返回NDK_ERR_NOT_SUPPORT 20150415 jiangym
	if(g_at_enable)//BM77不支持
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		
		cls_show_msg1(10,"此蓝牙模块不支持%s函数", TESTAPI);
		return;
	}
/*
	//本测试点移植到bt.c中,因为未打开返回失败是在未下patch前返回,之后进行设置都能成功  20150415 jiangym
	//case1:蓝牙串口未打开 返回失败
	if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_ERR_IOCTL)//0
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}
*/
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	/*memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, outbuf);
		GOTOERR;
	}*/

	//case2:参数设置不为0或1或2应该返回失败
	if((ret=NDK_BTSetDiscoverableStatus(3))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTSetDiscoverableStatus(-1))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
//ME30MH前面测试需要设置开启广播才能用后这里就没有系统默认的情况了
	if(!g_btdiscoverflag)
	{
		//case3:正常测试:系统默认为能搜索到
		if(cls_show_msg("若手机与蓝牙%s已配对请先取消配对后,判断此时应该能搜索到%s,是[Enter],否[其他]", BtName, BtName)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}	
	//case4:正常测试:设置为0 不能被搜索到
	if((ret=NDK_BTSetDiscoverableStatus(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("此时手机进行重新搜索应该搜索不到蓝牙:%s,是[Enter],否[其他]", BtName)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case5:设置为1 能被搜索到 并连接成功后进行数据收发
	if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)//此时不调用disconnect,否则手机端工具会原先配对的取消配对
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("在BTFWATS工具中按del(其它工具进行:断开连接但不取消配对)操作后,按任意键继续");

	//case6:设置2,蓝牙只能被已配对的手机搜索到并能进行收发数据,未配对过的手机不能搜到,苹果手机无已配对概率故会搜索不到
	if((ret=NDK_BTSetDiscoverableStatus(2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if defined ME20 //对于ME20设置2跟设置0是一样的效果 搜索不到
	if(cls_show_msg("使用此手机进行搜索蓝牙%s应该搜索不到,是[Enter],否[其他]", BtName)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#else
	//此时使用已配对过的手机应该可以直接与蓝牙进行连接通讯
	if(cls_show_msg("使用安卓手机按[Enter],使用苹果手机按[其他]")==ENTER)
	{
		cls_show_msg("使用此安卓手机打开蓝牙工具,与%s连接后,任意键继续", BtName);
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
	}
	else
	{
		if(cls_show_msg("使用此苹果手机进行搜索蓝牙%s应该搜索不到,是[Enter],否[其他]", BtName)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	if(cls_show_msg("此时使用未配对过的手机进行搜索蓝牙%s应该搜索不到,是[Enter],否[其他]", BtName)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
	if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:蓝牙串口关闭了也能设置成功(未下patch时第一次未打开蓝牙才会返回失败,之后都能返回成功了)
	if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	//恢复默认设置
	NDK_BTSetDiscoverableStatus(1);
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}


