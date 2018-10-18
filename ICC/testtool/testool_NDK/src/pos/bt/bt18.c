/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt18.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetMaxBondedDevices 能否正确设置记住手机的个数
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetMaxBondedDevices"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt18
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20150330		created
*****************************************************************/
void bt18(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	/*private & local definition*/
	int ret = -1,i = 0;	  
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	if(cls_show_msg("首次进入此案例,选择是[Enter],重启后进入此案例选择[其他]")==ENTER)
	{
		//case1:蓝牙串口未打开,设置蓝牙记录的手机信息数目应该返回失败
		/*由于开发解释，必须在第一次蓝牙串口打开之前测试，因此已经移植到bt.c中*/
/*		if((ret=NDK_BTSetMaxBondedDevices(0))!=NDK_ERR_IOCTL)
		{
			send_result("line %d:测试失败(%d)", __LINE__, ret);
			return;
		}*/
		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case2:参数错误应该返回失败
		if((ret=NDK_BTSetMaxBondedDevices(6))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTSetMaxBondedDevices(-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case3:正常测试 :不记录手机个数
		if((ret=NDK_BTSetMaxBondedDevices(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_BTDisconnect();//pos端断开连接
		if(cls_show_msg("手机与pos蓝牙连接时有配对对话框,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);				
			GOTOERR;
		}	
		/*
		此处设置0不能验证出来，需要重启后才会起作用 
		原因是蓝牙模块记录了最后一次连接的手机 所以再次连接时不会出现配对对话框
		而是要重启后蓝牙模块就把记录的手机删除了,而设置的0是由K21传给蓝牙模块
		此时手机再去连pos时就会出现配对对话框了
		 ####是否考虑设置0不验证呢??####
		*/
		cls_show_msg("接下来测试再次连接时还需配对,任意键继续");
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_BTDisconnect();
		if(cls_show_msg("手机与pos蓝牙连接时有配对对话框,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);				
			GOTOERR;
		}

		//case4:正常测试:记录手机个数为5
		if((ret=NDK_BTSetMaxBondedDevices(5))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg("准备五部手机(可安卓可苹果可混搭)分别与pos进行连接,任意键继续");
		for(i=0;i<5;i++)
		{
			cls_show_msg1(3, "使用第%d部手机配对中...", i+1);
			if((ret=lib_bt_pairing(1))!=NDK_OK)
			{
				send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			NDK_BTDisconnect(); 
		}
		if(cls_show_msg("是否每部手机与pos蓝牙连接时均有配对对话框,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);				
			GOTOERR;
		}	
		cls_show_msg("接下来依次进行这五部手机的重新连接,连接成功后任意键继续(需一台成功后断开连接后进行下一台连接)");	
		if(cls_show_msg("5部手机不需配对能够直接连上pos,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}	
		//使用第六部手机进行连接时需要配对
		cls_show_msg("使用未与pos配对连接过的新的手机与pos蓝牙进行连接,任意键继续");
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_BTDisconnect();
		if(cls_show_msg("新的手机与pos进行连接时有配对对话框,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}	
		
		//case5:此时连接的第一部手机再进行连接时也需要配对
		cls_show_msg("使用先前连接的第一部手机与pos蓝牙进行连接,任意键继续");
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_BTDisconnect();
		if(cls_show_msg("第一部手机与pos进行连接时有配对对话框,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}	
		cls_show_msg("pos重启后再进入此案例进行第一部、第二部手机的重新连接,任意键继续");
		if(cls_show_msg("是否重启pos,是[Enter],否[其他]")==ENTER)
		{
			NDK_SysReboot();
		}
	}
	
	NDK_PortClose(PORT_NUM_BT);
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:重启后验证第一部不需配对，第二部需要配对
	cls_show_msg("再使用先前连接的第一部手机与pos蓝牙进行连接后,任意键继续");
	if(cls_show_msg("此部手机不需配对直接连上pos,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	
	NDK_BTDisconnect();
	//接着连接第二部手机会配对
	cls_show_msg("使用先前连接的第二部手机与pos蓝牙进行连接,任意键继续");
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("此部手机与pos进行连接时有配对对话框,是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}	

	send_result("%s测试通过", TESTAPI);
	
ERR:
	NDK_PortClose(PORT_NUM_BT);
	return;
}


