/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest64.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: SMART/BT交叉测试
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
#define	TESTITEM	"SMART/BT交叉"

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
static void smart_BT(SMART_t type, int j)
{
	/*private & local definition*/
#include "icsam.h"

	int ret = 0, ret1 = 0, i = 0, succ = 0, uidlen = 0, cmdlen = 0, recvLen = 0, datalen = 0; 
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"}, msg[32] = {0};
	char sendBuf[PACKMAXLEN] = {0}, recvBuf[PACKMAXLEN] = {0};
	char cmdBuf[BUFSIZE_BT] = {0};//, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	uchar UID[LEN_UID] = {0};
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
	cls_show_msg1(g_keeptime, "请确保已安装%s卡,任意键继续...", (type==SAM||type==IC)?CardNoStr[j]:card[type]);
	if(type==SAM||type==IC)
	{	
		sprintf(msg, "%s/BT交叉测试", CardNoStr[j]);
		UID[0] = CardNo[j];
	}
	else
		sprintf(msg, "%s/BT交叉测试", card[type]);

	i=0;
	while(1)
	{
		//保护动作
		smart_deactive(type, CardNo[j], 10);
		//NDK_PortClrBuf(PORT_NUM_BT);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, msg, succ)==ESC)
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
		//smart寻卡
		if ((ret1=smart_detect(type, UID, &uidlen))!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, i, msg, ret1);
			continue;
		}
		
		//蓝牙清缓冲区 //为测试底层蓝牙和控制磁卡和IC卡的芯片存在缓冲区冲突的问题,贴近应用实际使用方式不再做清缓冲操作 20160912 linwl
		/*if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}*/
		
		//smart激活
		if((ret1=smart_active(type, UID, &uidlen))!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, i, msg, ret1);
			continue;
		}
		
		//蓝牙发送数据
		memset(cmdBuf, 0, sizeof(cmdBuf));
#if defined ME15CHX||defined ME15B 		
		bt_cmd_packet(0x05, sendBuf, datalen, cmdBuf);
#else
		bt_cmd_packet(0x05, sendpacket.header, datalen, cmdBuf);
#endif		
		//cmdlen = sendpacket.len+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		sleep(1);
		
		//smart读写
		if ((ret1=smart_APDUrw(type, CardNo[j], UID))!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, i, msg, ret1);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
		
		//蓝牙接收数据
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			//bt_comm_once(12,0);
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
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}
#endif
		//smart下电
		if ((ret1=smart_deactive(type, CardNo[j], 10))!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, i, msg,ret1);
			NDK_PortClrBuf(PORT_NUM_BT);
			continue;
		}	
		
		//断开蓝牙连接(低端产品需要调用此函数断开连接,中端产品加上此调用也无影响)
#if !BTMFI_ENABLE//MFI工具是原包送回的工具，如果发送03和06包会影响下次测试，故无需发送，zhengry 20170104
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
		succ++;
	}
	
	smart_deactive(type, CardNo[j], 0);	
	cls_show_msg_record(FILENAME,FUNCNAME,"%s完成,已执行次数为%d,成功%d次,注意:本用例不支持NLBluetoothTest工具", msg, i-1, succ);
	return;
}


void systest64(void)
{
	/*private & local definition*/	
//#include "icsam.h"
	int ret = 0, i = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	char *CardNoStr[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
#if !defined ME15CHX//ME15C为节省代码空间进行条件编译 20170213 linwl
	if(auto_flag==1)//支持自动测试
	{
		//自动化测试内容
		//BT配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,BT配置失败,请检查配置文件",TESTITEM);
			return;
		}
		
		//执行交叉测试
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);

		//SMART配置
		switch(type=select_smarttype())
		{
		case CPU_A:
		case CPU_B:
		case MIFARE_1:
		case FELICA:
		case MIFARE_0:
		case ISO15693:
			if ((ret=RFID_init(type))!=SUCC)
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:射频卡初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				return;
			}
			smart_BT(type, i);
			break;
		case SAM:
		case IC:
			//type = CPU_A;
			break;
		default:
			break;
		}
		
		//按顺序轮流进行IC/SAM1/SAM2/SAM3和BT的交叉
#if IC1_ENABLE
		type = IC;
		i = 0;
		smart_BT(type, i);
#endif
#if SAM1_ENABLE
		type = SAM;
		i = 1;
		smart_BT(type, i);
#endif
#if SAM2_ENABLE
		type = SAM;
		i = 2;
		smart_BT(type, i);
#endif
#if SAM3_ENABLE
		type = SAM;
		i = 3;
		smart_BT(type, i);
#endif
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
#endif

	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.BT配置\n"
							"9.smart配置");		
		memcpy(g_RF_bak, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			smart_BT(type, i);
			break;
		case '0':
			conf_conn_BT();
#if BTMFI_ENABLE
			NDK_BTSetBleStatus(0);//在MFI协议下，关闭BLE方式
#endif
			break;
		case '9':
			switch(type=select_smarttype())
			{
#if !defined ME15CHX//ME15C为节省代码空间进行条件编译 20170213 linwl
			case CPU_A:
			case CPU_B:
			case MIFARE_1:
			case FELICA:
			case MIFARE_0:
			case ISO15693:
				if ((ret=RFID_init(type))!=SUCC)
					cls_show_msg("line %d:初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				else
					cls_show_msg1(2, "%s初始化成功!", card[type]);
				break;
#endif
			case SAM:
			case IC:
			//	conf_icsam_pps();
				if ((i=select_ICSAM())==NDK_ERR_QUIT)
					;//QUIT不提示
				else
					cls_show_msg1(2, "已选择%s", CardNoStr[i]);
				break;
			default:
				break;
			}
			break;
		case ESC:
			cls_show_msg("请选择其它卡片重新测试!");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

