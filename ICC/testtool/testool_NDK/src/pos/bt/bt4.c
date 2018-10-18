/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt2.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTGetLocalMAC是否可以正常获取蓝牙的MAC地址
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTGetLocalMAC"
#define   MAXLEN	128

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt4
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt4(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}

	/*private & local definition*/
	char  outbuf[20] = {0};
	int  ret = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
#if !K21_ENABLE
	//case1:蓝牙串口未打开进行获取MAC，应该失败
	if((ret=NDK_BTGetLocalMAC(outbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:测试%s失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif 	

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	if(g_at_enable)//BM77
	{	
		//case3:未进入命令模式,不能获取到Mac
		if((ret=NDK_BTGetLocalMAC(outbuf))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:测试%s失败(%d)(%s)", __LINE__, TESTAPI, ret, outbuf);
			GOTOERR2;
		}
	}
	
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR2;
	}
	
	//case 4:异常测试:测试mac参数为NULL
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalMAC(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:测试%s失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case 5:正常测试
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalMAC(outbuf))!=NDK_OK)
	{
		send_result("line %d:测试%s失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	//均使用此方式输出
	send_result("%s测试完成,mac地址:%02x:%02x:%02x:%02x:%02x:%02x请与手机扫描到的蓝牙MAC核对是否一致!", TESTAPI, outbuf[5], outbuf[4], outbuf[3], outbuf[2], outbuf[1], outbuf[0]);//由于自检和应用显示MAC的方式一样,所以使用自检无法识别设置反的情况,改用手机扫描的MAC结果来比较
ERR:
	NDK_BTExitCommand();
ERR2:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}


