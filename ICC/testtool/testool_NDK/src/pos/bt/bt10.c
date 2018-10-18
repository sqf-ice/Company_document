/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt10.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_PortReadLen能否正确读蓝牙缓冲区字节
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"蓝牙读取缓冲区"
#define   MAXWAITTIME	 10
#define		MAX_SIZE	 BUFSIZE_BT-MIN_BTCMD_LEN

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt10
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt10(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, ret1 = -1,len = 0,len1 = 0, readlen = 0, j = 0, cmdlen = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	char cmdrecvBuf[BUFSIZE_BT+10] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	//国芯ME30测试过程中cmdrecvBuf缓冲区由于安卓工具问题会多返回9字节数据,故需要增加缓冲区大小,否则会飞20170801
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;

#if !K21_ENABLE
	//case1:对未开的串口进行取缓冲区字节数操作,应返回相应错误值
	if((ret = NDK_PortReadLen(PORT_NUM_BT,&len)) != NDK_ERR_OPEN_DEV)
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
#endif
	
	//case2:打开串口,传入非法参数非法串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_PortReadLen(-1,&len)) != NDK_ERR_PARA)||((ret1=NDK_PortReadLen(PORT_NUM_BT,NULL)) != NDK_ERR_PARA))
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
	//case2:正常测试能够获取到正确的缓存区数据长度
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
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
#if K21_ENABLE
	sleep(5);//给出足够的时间然蓝牙后台送回数据到POS接收缓冲区
#endif
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret = NDK_PortReadLen(PORT_NUM_BT,&len)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	if(len != cmdlen)//第一次读到的长度不为4096则多读几次
	{
		while(1)//经过开发和应用确认由于内核原因NDK_PortReadLen函数最大只能返回4095,所以这里必须循环的调用该函数通过多次调用的长度和来判断数据是否丢
		{
			sleep(1);//由于手机端IOS蓝牙工具速度比较慢是连续读会读到长度0故增加延时
			if((ret = NDK_PortReadLen(PORT_NUM_BT,&len1)) != NDK_OK)
			{				
				send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len1);
				goto ERR;
			}
			if(len1==0)
			{
				if(readlen!=cmdlen)//多次调用后的长度和应该等于预期值 ##这边如果报错，预期值比实际值少9字节的话，已定位为安卓apk的问题20170523 added
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, readlen, sizeof(cmdsendBuf));
					goto ERR;		
				}
				break;
			}
			readlen+=len1;//NDK_PortReadLen取到的长度必须和NDK_PortRead实际读到的一致
			memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
			if(((ret=NDK_PortRead(PORT_NUM_BT, len1, cmdrecvBuf, MAXWAITTIME*1000, &len)) != NDK_OK) || (len!=len1))
			{
				send_result("line %d:%s测试失败(ret=%d)(实际%d, 预期%d)", __LINE__, TESTAPI, ret, len, len1);
				goto ERR;
			}
		}
	}
	
	//case4:清缓冲后在调用获取到的缓冲区数据长度应该为0
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);//给出足够的时间然蓝牙后台送回数据到POS接收缓冲区
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_PortReadLen(PORT_NUM_BT,&len)) != NDK_OK||len != 0)
	{				
 		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
 		GOTOERR;
	}
	
	//case5:串口数据接收后，接收缓冲区会清0
	sleep(5);
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, sizeof(sendBuf), cmdsendBuf);
	cmdlen = sizeof(sendBuf) + MIN_BTCMD_LEN;
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)||(len != cmdlen))
	{
		send_result("line %d:%s测试失败(ret=%d)(实际%d, 预期%d)", __LINE__, TESTAPI, ret, len, cmdlen);
		GOTOERR;
	}
	bt_cmd_uppacket(cmdrecvBuf, sizeof(sendBuf), recvBuf);
	if(memcmp(sendBuf, recvBuf, sizeof(sendBuf)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret = NDK_PortReadLen(PORT_NUM_BT,&len)) != NDK_OK||len != 0)
	{				
		send_result("line %d:%s测试失败(%d),%d", __LINE__, TESTAPI, ret, len);
		GOTOERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

