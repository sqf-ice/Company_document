/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest65.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 打印/BT交叉
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
#define	TESTITEM		"打印/BT交叉"
#define	MAXWAITTIME	(120)	//30->120 for 针打

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
static void print_BT(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0, ret = -1, cmdlen=0, recvLen=0;
	char sendBuf[PACKMAXLEN] = {0}, recvBuf[PACKMAXLEN] = {0};
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	Packet sendpacket;

	/*process body*/
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);

	if(lib_initprn(g_PrnSwitch)!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return;
	}
	print_bill();
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:打印机状态异常!", __LINE__);
		return; 						
	}
	while (1)
	{
		//保护动作
		NDK_PortClrBuf(PORT_NUM_BT);	
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, TESTITEM, succ_count)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
				
		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败(%d)", __LINE__, i, ret);
			continue;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			continue;
		}
		cls_printf("正在进行打印与BT交叉测试...");
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;
		}
		//蓝牙清缓冲区
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;
		}
		
		//蓝牙发送数据
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x05, sendpacket.header, sendpacket.len, cmdsendBuf);
		cmdlen = sendpacket.len+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;
		}
		
		//蓝牙接收数据
		memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			bt_comm_once(12,0);
			continue;
		}
		bt_cmd_uppacket(cmdrecvBuf, sendpacket.len, recvBuf);
		if(memcmp(sendBuf, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙数据校验失败", __LINE__, i);
			continue;
		}
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;
		}
		
		//断开蓝牙连接(低端产品需要调用此函数断开连接,中端产品加上此调用也无影响)
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x03, NULL, 0, cmdsendBuf);
		NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf);
		packetnum = 0;//连接断开后序列号要清0
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败", __LINE__, i);
			continue;
		}
		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;
		}		
		succ_count++;	
	}
		
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯次数:%d\n成功次数:%d", i-1, succ_count);
	return;
}

void systest65(void)
{
	if(auto_flag==1)//支持自动测试
	{
		//自动化测试内容
		//BT配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,BT配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//prnt配置
		conf_prn();
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		print_BT();
		
		//按顺序轮流进行IC/SAM1/SAM2/SAM3和BT的交叉
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	while (1)
	{	
		keyin = cls_show_msg("1.交叉测试\n"
							"0.BT配置\n"
							"9.打印配置");
		switch(keyin)
		{
		case '1':
			print_BT();
			break;
		case '0':
			conf_conn_BT();
			break;
		case '9':
			conf_prn();
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

