/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt5.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetPIN,NDK_BTGetPIN是否可以正常设置获取PIN
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTSetPIN,NDK_BTGetPIN"
#define   MAXPIN    	6
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt5
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt5(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}

	/*private & local definition*/
	char *inbuf[] ={"654321", 
					"123",
					"ne#1^3"};
	char abnorname[MAXPIN+3] = "1234567";
	char outbuf[MAXPIN+1] = {0}, oldbuf[MAXPIN+1] = {0};
	int ret = -1, i=0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
#if BTPCI_ENABLE
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR3;
	}
	if((ret=NDK_BTSetPIN(inbuf[0]))!=NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_BTGetPIN(oldbuf))!=NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	send_result("PCI版本蓝牙不支持%s,测试通过", TESTAPI);
	return;
#endif

#if !K21_ENABLE
	//case1:蓝牙串口未打开进行设置PIN应该失败
	if((ret=NDK_BTSetPIN(inbuf[0]))!=NDK_ERR_OPEN_DEV)//linwl 将inbuf修改成inbuf[0] 20140603 去除编译警告
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BTGetPIN(outbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR3;
	}
	
	if(g_at_enable)//BM77
	{	
		//case3:未进入命令模式设置获取会失败
		if((ret=NDK_BTSetPIN(inbuf[0]))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR3;
		}
		if((ret=NDK_BTGetPIN(outbuf))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR3;
		}
	}
	
	//测试前置:进入命令模式后才能进行获取默认的PIN
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	//获取先前的PIN
	memset(oldbuf,0,sizeof(oldbuf));
	if((ret=NDK_BTGetPIN(oldbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR2;
	}
	
	//case 4:异常测试
	//case 4.1:pinstr=NULL的情况
	if((ret=NDK_BTSetPIN(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTGetPIN(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case 4.2:PIN长度超过6字节
	if((ret=NDK_BTSetPIN(abnorname))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case 5:测试不同设置的PIN的情况,包括长度小于6,包含字母以及正常6位数的
	for(i=0;i<SZ_ARRAY(inbuf);i++)
	{
		if((ret=NDK_BTSetPIN(inbuf[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//蓝牙外挂flash的新方案增加用例测试重新open后蓝牙PIN变成旧名称的问题 20160728 linwl
		NDK_PortClose(PORT_NUM_BT);
		NDK_PortOpen(PORT_NUM_BT, NULL);
		NDK_BTEnterCommand();
		memset(outbuf,0,sizeof(outbuf));
		if((ret=NDK_BTGetPIN(outbuf))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(strcmp(outbuf, inbuf[i]))
		{
			send_result("line %d:%s测试失败%s,%s", __LINE__, TESTAPI, outbuf, inbuf[i]);
			GOTOERR;
		}
	}
	
	//case6:先设置一个pin,再设置个pin，看获取的是否是第二次设置的 20140603 added by jiangym
	if((ret=NDK_BTSetPIN(inbuf[0]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTSetPIN(inbuf[1]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetPIN(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(outbuf, inbuf[1]))
	{
		send_result("line %d:%s测试失败%s,%s", __LINE__, TESTAPI, outbuf, inbuf[1]);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_BTSetPIN(oldbuf);
ERR2:
	NDK_BTExitCommand();
ERR3:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}


