/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt8.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_PortRead能否读取蓝牙数据,NDK_PortWrite能否正常发送蓝牙数据
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define     TESTAPI		"蓝牙读写"
#define     MAXWAITTIME		10
#define		MAXTIMEOUT	10//超时测试时间
#define		WUCHASEC	0.03 //这个值待定
#define		MAX_SIZE	BUFSIZE_BT-MIN_BTCMD_LEN
#define 	SNDCNT			(71-MIN_BTCMD_LEN)
#define 	SNDCNT1			(128-MIN_BTCMD_LEN)


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt8
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt8(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, j = 0, cmdlen = 0, recvLen = 0;
	float tmp = 0.0, unTime = 0.0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
#if K21_ENABLE
	char str[32] = {0};
#endif

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;

#if !K21_ENABLE
	//case1:蓝牙串口未打开进行读写操作,应返回相应错误值
	if((ret = NDK_PortRead(PORT_NUM_BT, sizeof(recvBuf), recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, recvLen);
		GOTOERR;
	}
	if((ret = NDK_PortWrite(PORT_NUM_BT, sizeof(sendBuf), sendBuf)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	
	//测试前置,打开串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:应保证在高、低波特率下，串口收发大数据(指蓝牙串口缓冲区大小，一般多为4K)正常
	//大数据读取(低端2K-1，终端4K)	
#if BTMFI_ENABLE
	cls_show_msg("手机端允许连接后初始化并连接pos后任意键继续");
#else
	cls_show_msg1(3,"进行%dB数据读写",MAX_SIZE);
#endif
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
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
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:应能向蓝牙串口写指定长度的数据:正常收发非整8字节数据
	cls_show_msg1(3,"进行非整8字节数据读写");
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, SNDCNT, cmdsendBuf);//将sendBuf中前SNDCNT字节作为测试数据
	cmdlen=SNDCNT+MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s发送失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	{
		send_result("line %d:%s测试接收错(ret=%d)(实际%d, 预期%d)", __LINE__, TESTAPI, ret, recvLen, cmdlen);
		GOTOERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	bt_cmd_uppacket(cmdrecvBuf, SNDCNT, recvBuf);
	if(memcmp(sendBuf, recvBuf, SNDCNT))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:发送64整数倍但又不超过4K的数据
	cls_show_msg1(3,"进行64整数倍但又不超过4K的数据读写");
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, SNDCNT1, cmdsendBuf);//将sendBuf中前SNDCNT1字节作为测试数据
	cmdlen=SNDCNT1+MIN_BTCMD_LEN;
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
	bt_cmd_uppacket(cmdrecvBuf, SNDCNT1, recvBuf);
	if(memcmp(sendBuf, recvBuf, SNDCNT1))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:读超时测试:应该返回超时,并且超时时间在误差范围内
	cls_show_msg1(1,"读超时测试中");
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(defined ME15CHX||defined ME15B)
	lib_StartStopwatch();
	unTime = 0;
#else
	unTime = lib_ReadStopwatch();
#endif
#if defined ME30GX||defined ME32GX
	if( ((ret=NDK_PortRead(PORT_NUM_BT, sizeof(recvBuf), recvBuf, MAXTIMEOUT*1000, &recvLen))!=NDK_ERR_TIMEOUT) || (tmp=abs(INT_ReadStopwatch()-unTime-MAXTIMEOUT))>WUCHASEC)	/*读取串口数据*/
#else
	if( ((ret=NDK_PortRead(PORT_NUM_BT, sizeof(recvBuf), recvBuf, MAXTIMEOUT*1000, &recvLen))!=NDK_ERR_TIMEOUT) || (tmp=fabs(lib_ReadStopwatch()-unTime-MAXTIMEOUT))>WUCHASEC)	/*读取串口数据*/
#endif
	{
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%ss)", __LINE__, TESTAPI, ret, str);
#else
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
#endif
		GOTOERR;
	}
	
	//case6:测试读完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	cls_show_msg1(1,"测试读完关闭串口中");
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
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);//延时1秒,确保手机收到06指令
	if((ret = NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:若提供了蓝牙串口关闭功能，则在读(case5)或写大量数据后（不论是阻塞还是非阻塞，若是非阻塞，不要等待读写完成，均以读写函数返回为准），立即进行蓝牙串口关闭功能（此时有可能读写未实际完成），不应有程序跑飞或死机等异常
	//测试写完数据马上关闭串口，不应该出现异常(跑飞或者死机)
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !K21_ENABLE  //K21可以不需要此动作之前的连接还存在且能进行往串口写数据
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	cls_show_msg1(3,"测试写完关闭串口中");
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(3);//延时1秒,确保手机收到06指令
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:重新打开蓝牙串口进行数据收发应该成功
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许连接后初始化并连接pos后任意键继续");
#else
	cls_show_msg1(3,"测试重新打开蓝牙串口进行数据收发中");
#endif
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
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
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);
	if((ret = NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:关闭蓝牙串口后，读写不应该成功
	if((ret = NDK_PortRead(PORT_NUM_BT, sizeof(recvBuf), recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, recvLen);
		GOTOERR;
	}
	if((ret = NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

