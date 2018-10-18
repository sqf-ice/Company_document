/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt9.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_PortClrBuf能否清蓝牙缓冲区
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"蓝牙清除缓冲区"
#define   MAXWAITTIME	 10
#define		MAX_SIZE	 BUFSIZE_BT-MIN_BTCMD_LEN

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt9
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt9(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, ret1 = -1, j = 0, cmdlen = 0, recvLen = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	
#if !K21_ENABLE
	//case1:对未开的串口进行清缓冲操作,应返回相应错误值
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2:打开串口,传入非法串口,应返回对应的错误值
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_PortClrBuf(-1)) != NDK_ERR_PARA)
		||((ret1=NDK_PortClrBuf(100)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
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
	//case3:清接收缓冲之后再去读数据,应读不到数据了
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//给蓝牙足够的时间将数据送到POS端后清缓冲
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, 3*1000, &recvLen)) != NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s测试接收错(ret=%d)(实际%d, 预期%d)", __LINE__, TESTAPI, ret, recvLen, cmdlen);
		GOTOERR;
	}
	sleep(1);//避免太快清缓冲,引起手机没有收到06指令出现异常
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//为兼容NLBluetoothTest工具而增加延时，避免旧工具对06指令进行回传导致case4清缓冲后才收到06指令数据引起数据校验失败

	//case4:正常使用测试,清缓冲后校验读写的准确性
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sendBuf, 0x38, sizeof(sendBuf));
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	{
		send_result("line %d:%s测试接收错(ret=%d)(实际%d, 预期%d)", __LINE__, TESTAPI, ret, recvLen, cmdlen);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	bt_cmd_uppacket(cmdrecvBuf, MAX_SIZE, recvBuf);
	if(memcmp(sendBuf, recvBuf, MAX_SIZE))
	{
		send_result("line %d:%s测试数据校验失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	sleep(1);//避免太快挂断,引起手机没有收到06指令出现异常
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

