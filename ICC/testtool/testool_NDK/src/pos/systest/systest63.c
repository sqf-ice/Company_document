/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest63.c
* Author 			:
* version			: 
* DATE				:
* directory 			: 
* description			: 磁卡/BT交叉测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"磁卡/BT交叉"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		
*****************************************************************/
static void card_BT(void)
{
	/*private & local definition*/
	int  ret = -1, i = 0, recvLen = 0, succ=0, cmdlen = 0, datalen = 0;
	char sendBuf[PACKMAXLEN] = {0}, recvBuf[PACKMAXLEN] = {0};
	char cmdBuf[BUFSIZE_BT] = {0};//, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
#if !(defined ME15CHX||defined ME15B) 
	Packet sendpacket;
#else
	uint len = 0;
	char tbuf[16] = {0};
	int cnt = 0;
#endif

	/*process body*/
#if !(defined ME15CHX||defined ME15B) 
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
#else
	cls_printf("设置交叉次数:\n");
	strcpy(tbuf, "10");
	if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len, INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
		strcpy(tbuf, "10");
	cnt=atoi(tbuf);
	
	cls_printf("设置初始包长:\n");
	strcpy(tbuf, "1000");
	if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len,INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
		strcpy(tbuf, "1000");		
	datalen = atoi(tbuf);
	if(datalen>BUFSIZE_BT-MIN_BTCMD_LEN)
		datalen = BUFSIZE_BT-MIN_BTCMD_LEN;

	//ME15CHX内存只有4K,不能使用sendpacket相关函数,直接使用sendBuf,且固定测试BUFSIZE_BT长度随机数收发
	for (i=0; i<sizeof(sendBuf); i++)
		sendBuf[i]=rand()%256;
#endif

	i=0;
	while (1)
	{
		//保护动作
		//NDK_PortClrBuf(PORT_NUM_BT);	
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, TESTITEM, succ)==ESC)
			break;
#if !(defined ME15CHX||defined ME15B) 
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		datalen = sendpacket.len;
#else
		if(i > cnt)
			break;
#endif
		cmdlen = datalen+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度

		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败(%d)", __LINE__, i, ret);
			continue;
		}
		if(g_btdiscoverflag)
		{
			if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
				continue;
			}
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			continue;
		}
#if BTMFI_ENABLE
		cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
		if(systest_MagCard()!=STRIPE)
			continue;
		
		//蓝牙清缓冲区 //为测试底层蓝牙和控制磁卡和IC卡的芯片存在缓冲区冲突的问题,贴近应用实际使用方式不再做清缓冲操作 20160912 linwl
		/*if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}	
		if(systest_MagCard()!=STRIPE)
			continue;*/
		
		//蓝牙发送数据
		memset(cmdBuf, 0, sizeof(cmdBuf));
#if defined ME15CHX||defined ME15B 		
		bt_cmd_packet(0x05, sendBuf, datalen, cmdBuf);
#else
		bt_cmd_packet(0x05, sendpacket.header, sendpacket.len, cmdBuf);
#endif		
		//cmdlen = sendpacket.len+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		//这边刷卡要快,如果慢可能会导致后面的06指令未发,手机端会回传9字节数据而导致下次测试数据比较失败20170823
		if(systest_MagCard()!=STRIPE)
		{
			NDK_PortClrBuf(PORT_NUM_BT);//避免刷卡错后蓝牙数据没有清空而是的下次数据校验错
			continue;
		}
		sleep(1);
		
		//蓝牙接收数据
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		bt_cmd_uppacket(cmdBuf, datalen, recvBuf);
		if(memcmp(sendBuf, recvBuf, datalen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙数据校验失败", __LINE__, i);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
#if !BTMFI_ENABLE//MFI工具是原包送回的工具，如果发送03和06包会影响下次测试，故无需发送，zhengry 20170104
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
		{
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		
		//断开蓝牙连接(低端产品需要调用此函数断开连接,中端产品加上此调用也无影响)
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x03, NULL, 0, cmdBuf);
		NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf);
#endif		
		packetnum = 0;//连接断开后序列号要清0
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败", __LINE__, i);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
			continue;
		
		succ++;
	}
			
	cls_show_msg("交叉次数:%d\n成功次数:%d,注意:本用例不支持NLBluetoothTest工具", i-1, succ);
	return;
}

void systest63(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while (1)
	{
		keyin = cls_show_msg("1.交叉测试\n"
							 "0.BT配置");
		switch(keyin)
		{
		case '1':
			card_BT();
			break;
		case '0':
			conf_conn_BT();
#if BTMFI_ENABLE
			NDK_BTSetBleStatus(0);//在MFI协议下，关闭BLE方式
#endif
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}

