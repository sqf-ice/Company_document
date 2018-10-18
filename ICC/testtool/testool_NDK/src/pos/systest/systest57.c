/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 蓝牙综合	
* file name			: systest57.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 蓝牙综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20131202  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"蓝牙综合"
#define MAXWAITTIME		30
#define DEFAULT_CNT_VLE	100
#define	DEFAULT_CNT_STR	"100"
#define	PACKETLIFE		(10)

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
* history 				: author			date			remarks
					 	jiangym			20140227	created
*****************************************************************/	
static void create_bt_ability_packet(Packet *packet, char *buf)
{
	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len= BUFSIZE_BT-MIN_BTCMD_LEN;
	packet->lifecycle = PACKETLIFE;//10;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}
static void bt_ability()
{
	int ret = -1,i = 0, recvLen = 0, cmdlen = 0, datalen = 0;
	char sendBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0}, recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};//原始数据
	char cmdBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	float commtimes = 0, rate = 0;
#if K21_ENABLE
	char str[32] = {0};
#endif
#if !(defined ME15CHX||defined ME15B)
	Packet sendpacket;
#else
	int cnt = 0;
	float time1 = 0, time2 = 0;
#endif
	
	/*process body*/
#if !(defined ME15CHX||defined ME15B) 
	create_bt_ability_packet(&sendpacket, sendBuf);
#else
	datalen = BUFSIZE_BT-MIN_BTCMD_LEN;
	cnt = PACKETLIFE;
	for (i=0; i<sizeof(sendBuf); i++)
		sendBuf[i]=rand()%256;
#endif
	//测试前置：
	NDK_PortClose(PORT_NUM_BT);
	if((ret = NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			return ;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:配对连接失败(%d)", __LINE__,ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	cls_printf("进行蓝牙读写性能测试...");
	//循环读写10次 计算读写通讯速率
	i=0;
	while(1)
	{
		i++;
#if !(defined ME15CHX||defined ME15B)
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		datalen = sendpacket.len;
#else		
		if(i>cnt)
			break;
#endif
		cmdlen = datalen+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
		
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)//清接收缓冲
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}
		//发送数据回传命令
		cls_printf("数据收发中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x05, sendBuf, datalen, cmdBuf);
#if !(defined ME15CHX||defined ME15B)
		lib_StartStopwatch();
#else
		if(i==1)
			time1 = lib_ReadStopwatch();//读取第一次开始测试时的时钟
#endif
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto DISCONNECT;
		}
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT,cmdlen, cmdBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))//MAXWAITTIME
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			goto DISCONNECT;
		}
#if !(defined ME15CHX||defined ME15B)
		commtimes+=lib_ReadStopwatch();
#else
		time2=lib_ReadStopwatch();//不断更新时间,最后一次更新时间后的时间差就是通讯时间
		commtimes=time2-time1;
#endif
		bt_cmd_uppacket(cmdBuf, datalen, recvBuf);
		if(memcmp(sendBuf, recvBuf, datalen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i);
			goto DISCONNECT;
		}

DISCONNECT:
		//发送响应确认报文
		cls_printf("发送响应数据报文中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			continue;
		}
	}
	if((ret = NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:断开蓝牙连接失败(%d)", __LINE__, ret);
		return ;
	}
	NDK_PortClose(PORT_NUM_BT);
	rate = (float)cmdlen/(commtimes/PACKETLIFE);
#if !K21_ENABLE
	cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "蓝牙读写数据通讯速率为%fB/s", rate);
#else
	{
		memset(str,0,sizeof(str));
		ftos(rate,str);
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "蓝牙读写数据通讯速率为%sB/s", str);
	}
#endif
	return;
}

static void bt_read_write(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, succconut = 0, cmdlen = 0, datalen = 0;
	char sendBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0}, recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};//原始数据
	char cmdBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
#if !(defined ME15CHX||defined ME15B)
	Packet sendpacket;
#else
	uint len = 0;
	char tbuf[16] = {0};
	int cnt = 0;
#endif

	
	/*process body*/
#if !(defined ME15CHX||defined ME15B)  //ME15B内存7K,栈4K局部变量不能超过4K 故不用sendpacket相关函数
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
#else
	cls_printf("设置包的发送次数:\n");
	strcpy(tbuf, "200");
	if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len, INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
		strcpy(tbuf, "200");
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
	
	//测试前置：
	NDK_PortClose(PORT_NUM_BT);
	if((ret = NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			return ;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:配对连接失败(%d)", __LINE__,ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	//读写压力测试
	i=0;
	while(1)
	{
		if(cls_show_msg1(3,"开始第%d次蓝牙串口读写压力(已成功%d次),ESC退出", i+1, succconut)==ESC)
			break;
		i++;
#if !(defined ME15CHX||defined ME15B)
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		datalen = sendpacket.len;
#else
		if(i > cnt)
			break;
#endif
		cmdlen = datalen+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
		
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)//清接收缓冲
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}

		//发送数据回传命令
		cls_printf("发送数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x05, sendBuf, datalen, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto DISCONNECT;
		}

		cls_printf("接收数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT,cmdlen, cmdBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))//MAXWAITTIME
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			goto DISCONNECT;
		}
		
		if((ret = bt_cmd_uppacket(cmdBuf, datalen, recvBuf))!=BTCMD_OK)
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:解析数据包失败%d", __LINE__, ret);
		
		cls_printf("比较数据中...");
		if(memcmp(sendBuf, recvBuf, datalen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i);
			goto DISCONNECT;
		}
		succconut++;//通讯成功次数增加
		
DISCONNECT:
		//发送响应确认报文
		cls_printf("发送响应数据报文中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			continue;
		}
	}
	//断开蓝牙连接
	//通知手机断开连接
	//cls_printf("发送挂断通知中...");
	//memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	//bt_cmd_packet(0x03, NULL, 0, cmdsendBuf);
	//NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf);
	packetnum = 0;//连接断开后序列号要清0
	
	if((ret = NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:断开蓝牙连接失败(%d)", __LINE__, ret);
		return ;
	}
	NDK_PortClose(PORT_NUM_BT);
	
	cls_show_msg_record(FILENAME,FUNCNAME,"蓝牙串口读写压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	return;
}

static void bt_open_close()
{
	int ret = 0, i = 0, cnt = 0, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	
	/*process body*/
	if(g_SequencePressFlag) 
	{
		cnt = GetCycleValue();  //连续压力测试，需要手工输入循环次数的都直接设置默认值
	}
	else
	{
		cls_printf("默认测试次数:\n");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
	}
		
	NDK_PortClose(PORT_NUM_BT);
	
	while(1)
	{
		if(i++>=cnt)//达到测试次数后退出
			break;
		if(cls_show_msg1(2,"蓝牙串口开关压力测试中\n总共:%d次,已执行:%d次,已成功:%d次,按ESC退出",cnt,i-1,succ)==ESC)
			break;
		
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, i);
			continue;
		}
		if(g_btdiscoverflag)
		{
			if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
				continue ;
			}
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			NDK_BTDisconnect();
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		cls_show_msg1(3,"连接成功,3秒后开始挂断操作");//此处如果不延时,手机会判断连接不成功,报错
		
		//POS主动断开连接
		if((ret = NDK_BTDisconnect()) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败(%d)", __LINE__, i, ret);
			continue;
		}
		
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			continue;
		}
		succ++;
	}

	NDK_PortClose(PORT_NUM_BT);
	cls_show_msg_record(FILENAME,FUNCNAME,"总共进行%d次蓝牙开关压力测试,已成功%d次",i-1,succ);
	return;
}

static void mobilephone_press()
{
	int ret = 0, i = 0, cnt = 0, succ = 0;//link_status = 0
	char CntStr[8] = DEFAULT_CNT_STR, cmdsendBuf[MIN_BTCMD_LEN] = {0};
	uint unLen = 0;
	
	/*process body*/
	if(g_SequencePressFlag) 
	{
		cnt = GetCycleValue();  //连续压力测试，需要手工输入循环次数的都直接设置默认值
	}
	else
	{
		cls_printf("默认测试次数:\n");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
	}
		
	NDK_PortClose(PORT_NUM_BT);
	
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打开蓝牙串口失败", __LINE__);
		return;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			return ;
		}
	}
	while(1)
	{
		if(i++>=cnt)//达到测试次数后退出
			break;
		if(cls_show_msg1(7,"手机建链压力测试中\n总共:%d次,已执行:%d次,已成功:%d次,按ESC退出",cnt,i-1,succ)==ESC)//测试用的苹果手机有时候发送数据比较慢，需要加长等待时间
			break;

		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			NDK_BTDisconnect();
			continue;
		}
		//cls_show_msg1(3,"连接成功,3秒后通知手机挂断操作");//此处如果不延时,手机会判断连接不成功,报错
#if BTMFI_ENABLE
		cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
		//发送指令通知手机挂断
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x03, NULL, 0, cmdsendBuf);
		NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf);
		packetnum = 0;//连接断开后序列号要清0
/*
		//判断手机断开成功
		sleep(3);
		if((ret=NDK_BTStatus(&link_status))!=NDK_OK||link_status!=1)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:手机主动断开失败(%d,%d)", __LINE__, ret, link_status);
			NDK_BTDisconnect();//手机主动断开失败,则调用POS断开
			continue;
		}
		*/
		succ++;
	}

	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	cls_show_msg_record(FILENAME,FUNCNAME,"总共进行%d次蓝牙开关压力测试,已成功%d次",i-1,succ);
	return;
}

#if 0	//这两个函数是用来验证BTFWATS测试工具的指令的
static void bt_set_time(void)
{
	/*private & local definition*/
	int ret = -1, i = 0;
	char cmdsendBuf[BUFSIZE_BT] = {0};
	char delaytime1[2]={0x00,0x05},delaytime2[2]={0x00,0x10};
	
	/*process body*/	
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, i);
		return;
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:配对连接失败(%d)", __LINE__,ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	//清接收缓冲
	{				
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
		goto DISCONNECT;
	}
	
	//发送数据回传命令
	cls_show_msg("按任意键开始设置扫描时间间隔t1值为5秒...");
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x01, delaytime1, 2, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN+2, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
		goto DISCONNECT;
	}

	cls_show_msg("按任意键开始设置扫描设备到连接设备的时间间隔t2值为10秒...");
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x02, delaytime2, 2, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN+2, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
		goto DISCONNECT;
	}
	
DISCONNECT:
	cls_show_msg("按任意键开始通知手机停止测试（动作）...");
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x04, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN+2, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	
	//断开蓝牙连接
	cls_show_msg1(1,"POS端挂断蓝牙连接中...");
	if((ret = NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败(%d)", __LINE__, i, ret);
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	NDK_PortClose(PORT_NUM_BT);
		
	return;
}

static void bt_read(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, cmdlen = 0;
	char recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};//原始数据
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	
	/*process body*/	
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, i);
		return;
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:配对连接失败(%d)", __LINE__,ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	//清接收缓冲
	{				
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
		goto DISCONNECT;
	}

	cls_show_msg1(1,"等待接收数据包中,请在手机端发送8个字节的0x38...");
	cmdlen = MIN_BTCMD_LEN+8;
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT,cmdlen, cmdrecvBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))//MAXWAITTIME
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
		goto DISCONNECT;
	}

	cls_show_msg1(1,"接收数据完成,解析数据包中...");
	if((ret = bt_cmd_uppacket(cmdrecvBuf, 8, recvBuf))!=BTCMD_OK)
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:解析数据包失败%d", __LINE__, ret);

	cls_show_msg1(1,"发送数据包中...");
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x07, recvBuf, 8, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
		goto DISCONNECT;
	}
	
DISCONNECT:
	cls_show_msg1(1,"设置0x03通知手机挂断（动作）...");
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x03, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN+2, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	
	//断开蓝牙连接
	cls_show_msg1(1,"POS端挂断蓝牙连接中...");
	if((ret = NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败(%d)", __LINE__, i, ret);
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	NDK_PortClose(PORT_NUM_BT);
		
	return;
}
#endif

static void bt_open_comm_close(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, succconut = 0, cmdlen = 0, datalen = 0;
	char sendBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0}, recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};//原始数据
	char cmdBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
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
	cls_printf("设置包的发送次数:\n");
	strcpy(tbuf, "200");
	if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len, INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
		strcpy(tbuf, "200");
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

	//测试前置：
	NDK_PortClose(PORT_NUM_BT);
	i=0;
	while(1)
	{
		if(cls_show_msg1(2,"开始第%d次蓝牙串口流程压力(已成功%d次),ESC退出", i+1, succconut)==ESC)
			break;
		i++;
#if !(defined ME15CHX||defined ME15B)
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		datalen = sendpacket.len;
#else
		if(i > cnt)
			break;
#endif
		cmdlen = datalen+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度

		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, i);
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
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			NDK_BTDisconnect();
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		//cls_show_msg1(3,"连接成功,3秒后开始发送数据");//此处如果不延时,手机会判断连接不成功,报错
#if BTMFI_ENABLE
		cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	//清接收缓冲
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			goto DISCONNECT;
		}

		//发送数据回传命令
		//cls_show_msg1(1, "发送数据包中...");
		cls_printf("发送数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x05, sendBuf, datalen, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto DISCONNECT;
		}

		//cls_show_msg1(2, "接收数据包中...");
		cls_printf("接收数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT,cmdlen, cmdBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))//MAXWAITTIME
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			bt_comm_once(12,0);
			goto DISCONNECT;
		}
		
		cls_printf("比较数据中...");
		bt_cmd_uppacket(cmdBuf, datalen, recvBuf);
		if(memcmp(sendBuf, recvBuf, datalen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i);
			goto DISCONNECT;
		}
		
		//发送响应确认报文
		cls_printf("发送响应数据报文中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto DISCONNECT;
		}
		succconut++;//通讯成功次数增加
		
DISCONNECT:
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret = NDK_BTDisconnect()) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败(%d)", __LINE__, i, ret);
			continue;
		}
		NDK_PortClose(PORT_NUM_BT);
	}

	cls_show_msg_record(FILENAME,FUNCNAME,"蓝牙串口数传+链接压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	return;
}

static void bt_press(void)
{
	/*private & local definition*/
	int key='0';
	
	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++key == '5' || cls_show_msg1(3, "即将进行连续压力测试,ESC退出")==ESC)
				return ;			
		}
		else
		{
			key = cls_show_msg("1.读写压力\n"
								"2.开关压力\n"
								"3.流程压力\n"
								"4.手机开关压力\n"
#if 0//调试指令集用
								"5.手动接收\n"
								"6.其它指令测试\n"
#endif
								);
		}
		
		switch(key)
		{
		case '1':
			bt_read_write();
			break;
		case '2':
			bt_open_close();
			break;	
		case '3':
			bt_open_comm_close();
			break;
		case '4':
			mobilephone_press();
			break;
#if 0
		case '5':
			bt_read();
			break;
		case '6':
			bt_set_time();
			break;
#endif			
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

//case1:在（手机端）提示配对时，拒绝配对，不应连接上。再次连接，接受配对，应能连接成功；
void abnormal_test1(void)
{
	int ret = -1,status = 1;
	if((ret = NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)//"115200,8,N,1"
	{
		cls_show_msg("line %d:%s测试失败(ret:%d)", __LINE__, TESTITEM,ret);
		goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	if(g_pair_mode == 2 || g_pair_mode == 3)
	{
		cls_show_msg("请打开BTFWATS工具下[手动测试],按任意键开始测试,当POS提示等待配对是进行[取消]配对的操作");
		if((ret=lib_bt_pairing(0))!=NDK_ERR)
		{
			cls_show_msg("line %d:配对应该失败(ret:%d)", __LINE__, ret);
			goto END;
		}
	}
	else
		cls_show_msg("请打开BTFWATS工具下[手动测试],与POS连接,出现PIN对话框时点取消,当提示未连接时继续");
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		cls_show_msg("line %d:%s测试失败(ret:%d,status:%d)", __LINE__, TESTITEM,ret,status);
		goto END;
	}
	if(cls_show_msg("POS与手机蓝牙连接失败,是[Enter],否[其他]")!=ENTER)
	{
		cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
		goto  END;
	}
	
	cls_show_msg("按任意键开始测试,并使用手机蓝牙工具进行搜索配对连接,当POS提示等待配对时请进行[确认]配对的操作");
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:配对应该成功(%d)", __LINE__, ret);
		goto END;
	}
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		cls_show_msg("line %d:%s测试失败(ret:%d,status:%d)", __LINE__, TESTITEM, ret, status);
		goto END;
	}
	if(cls_show_msg("POS与手机蓝牙连接成功,是[Enter],否[其他]")!=ENTER)
	{
		cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
		goto  END;
	}
	NDK_LedStatus(LED_RFID_RED_FLICK);
	cls_show_msg("子用例测试通过,任意键继续");
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	NDK_LedStatus(LED_RFID_RED_OFF);//关闭led灯
	return;
}

//case2:在有效通信范围内，长时间放置机器后（某些机具在长时间无操作的待机态下，蓝牙会进入休眠），再使用蓝牙功能，应能连接成功；
void abnormal_test2(void)
{
	int ret = -1;
	
#if !K21_ENABLE//本用例从BT7移到此处 20151217 linwl
	if(cls_show_msg("使用手机蓝牙工具搜索%s,应搜索不到,正确[Enter],错误[其他]",BtName)!=ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return;
	}
#endif
	cls_show_msg("长时间放置机器后,再按任意键开始,并使用手机蓝牙工具进行搜索配对连接");
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg("line %d:配对连接失败(%d)", __LINE__, ret);
    	goto END;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		cls_show_msg("line %d:数据收发失败(%d)", __LINE__,ret);
		goto END;
	}
	cls_show_msg("子用例测试通过,任意键继续");
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//case3:在（手机端）提示配对时，POS重启或关机，重启后蓝牙应仍能正常工作；
void abnormal_test3(void) 
{
	int ret = -1;

	cls_show_msg("请打开BTFWATS工具下[手动测试],按任意键POS进入等待配对操作,在配对过程中请将POS关机或者重启,重启后执行正常蓝牙测试,功能应正常");
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg("line %d:配对连接失败(%d)", __LINE__,ret);
		goto END;
	}
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//case4:在成功连接到手机后，POS重启或关机，重启后蓝牙应仍能正常工作；
void abnormal_test4(void)
{
	int ret = -1;

	cls_show_msg("请打开BTFWATS工具下[手动测试],按任意键开始测试,并使用手机蓝牙工具进行搜索配对连接");
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg("line %d:配对连接失败(%d)", __LINE__,ret);
		goto END;
	}
	cls_show_msg("请将POS关机或者重启,重启后执行正常蓝牙测试,功能应正常");	
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//case5:整机休眠后（不论整机休眠是否关闭蓝牙），（手机端）应能搜索到设备（蓝牙），并成功进行配对与连接 或 （手机端）通过与蓝牙模块的各种交互（包括：（配对）连接、数据通讯、断开等方式）应能唤醒整机；
void abnormal_test5(void)
{
	int ret = -1;
	char sendbuf[12] = {0}, cmdrecbuf[MIN_BTCMD_LEN+12]= {0};
	int len = 0;
#if !BTMFI_ENABLE
	char recbuf[12] = {0}, cmdsendbuf[MIN_BTCMD_LEN+12] = {0};
	int cmdlen = 0;
#endif
	
	//NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	cls_show_msg("POS等待约5s后按任意键进休眠后,使用BTFWATS的手动测试进行搜索配对连接(预期能够连接上并唤醒整机)");//预期就不要了吧 要不显示太长了呀
	//BM77配对成功后不会唤醒。原因是BM77的配对全部在模块内完成，POS没有收到数据因此没有中断产生不会唤醒 林文玺解释 20150416
	NDK_SysDelay(5);
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
	{
		cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
		goto END;
	}
	sleep(1);//ME15等进入休眠需要一点时间
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg("line %d:配对连接失败(%d)", __LINE__,ret);
		goto END;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__,ret);
		goto END;
	}
	cls_show_msg("POS等待约5s后按任意键进休眠后,使用手机蓝牙工具发送数据:12个8后查看POS(预期能够唤醒整机)");
	NDK_SysDelay(5);
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
	{
		cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
		goto END;
	}
	sleep(1);//ME15等进入休眠需要一点时间
	cls_printf("POS已经唤醒,接收数据中");
	memset(cmdrecbuf,0,sizeof(cmdrecbuf));
	memset(sendbuf,0x38,sizeof(sendbuf));
#if BTMFI_ENABLE
	//MFI下数据与自动化工具数据处理方式不同，需修改
	if(((ret=NDK_PortRead(PORT_NUM_BT, 12, cmdrecbuf, 5*1000, &len)) != NDK_OK) || (len != 12))
	{
		cls_show_msg("line %d:接收数据失败(%d,%d)", __LINE__, ret, len);
		goto END;
	}
	//比较数据
	if(memcmp(sendbuf, cmdrecbuf, 12))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		goto END;
	}
#else
	cmdlen=MIN_BTCMD_LEN+12;
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecbuf, 5*1000, &len)) != NDK_OK) || (len != cmdlen))
	{
		cls_show_msg("line %d:接收数据失败(%d,%d)", __LINE__, ret, len);
		goto END;
	}
	//解析数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret = bt_cmd_uppacket(cmdrecbuf, 12, recbuf))!=BTCMD_OK)
	{
		cls_show_msg("line %d:解析数据包失败%d", __LINE__, ret);
		goto END;
	}
	//比较数据
	if(memcmp(sendbuf, recbuf, 12))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		goto END;
	}
	//发送07指令收到手机数据
	memset(cmdsendbuf, 0, sizeof(cmdsendbuf));
	bt_cmd_packet(0x07, recbuf, 12, cmdsendbuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendbuf))!=NDK_OK)
	{
		cls_show_msg("line %d:POS发送数据失败(%d)", __LINE__, ret);
		goto END;
	}	
#endif
	cls_show_msg("POS等待约5s后按任意键进休眠后,使用手机蓝牙工具进行断开蓝牙后查看POS(预期能够唤醒整机)");
	NDK_SysDelay(5);
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
	{
		cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
		goto END;
	}
	sleep(1);//ME15等进入休眠需要一点时间
	if(cls_show_msg("pos已被唤醒,是[Enter],否[其他]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		goto END;
	}
	cls_show_msg("子用例测试通过,任意键继续");
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//case6:整机休眠唤醒后(不论是何种唤醒方式,如:按键唤醒、蓝牙唤醒、定时唤醒等),再重新进行蓝牙连接应成功(整机休眠关闭蓝牙的情况) 或 直接进行蓝牙数据通讯成功(整机休眠不关闭蓝牙的情况)
void abnormal_test6(void)
{
	int ret = -1, status = 0;
	int keyin = -1;
	char sendbuf[12] = {0}, cmdrecbuf[MIN_BTCMD_LEN+12]= {0};
	int len = 0;
#if !BTMFI_ENABLE
	char recbuf[12] = {0}, cmdsendbuf[MIN_BTCMD_LEN+12] = {0};
	int cmdlen = 0;
#endif

	NDK_PortClose(PORT_NUM_BT);
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(ret:%d)", __LINE__, ret);
    	goto END;
	}
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg( "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
			goto END; ;
		}
	}
	cls_show_msg("使用BTFWATS的手动测试进行蓝牙搜索配对连接,任意键继续");	
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg("line %d:配对连接失败(%d)", __LINE__,ret);
		goto END;
	}
#if BTMFI_ENABLE
	cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__,ret);
		goto END;
	}
	cls_show_msg("接下来是选择休眠后的唤醒方式,等待约5s后任意键继续");
	
	while(1)
    {
		keyin = cls_show_msg("1.按键唤醒\n"
							 "2.蓝牙唤醒\n"
#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE	
							 "3.定时唤醒"
#endif
							 );
		
		switch(keyin) 
		{
			case '1':
				cls_show_msg("按任意键进入休眠,休眠后按键唤醒");
				NDK_SysDelay(5);
#if ANDIROD_ENABLE
				if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
				if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
				{
					cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
					goto END;
				}
				sleep(1);//ME15等进入休眠需要一点时间
				if((ret=NDK_BTStatus(&status)) != NDK_OK)
				{
					cls_show_msg("line %d:获取蓝牙状态失败(%d)", __LINE__, ret);
					goto END;
				}	
				if(status)//休眠唤醒后若是断开状态则进行重新连接应该成功且能正常数据通讯
				{
					cls_printf("使用手机蓝牙工具进行重新连接");
					if((ret=lib_bt_pairing(1))!=NDK_OK)
					{
						cls_show_msg("line %d:重新配对连接失败(%d)", __LINE__,ret);
						goto END;
					}
				}
				//休眠唤醒后若是未断开状态 则进行正常数据通讯	
				if((ret=bt_comm_once(12,0))!=NDK_OK)
				{
					cls_show_msg("line %d:数据收发失败(%d)", __LINE__,ret);
					goto END;
				}
				break;
			case '2':
				cls_show_msg("按任意键进入休眠,休眠后手机蓝牙工具发送数据:12个8唤醒");
				NDK_SysDelay(5);
#if ANDIROD_ENABLE
				if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
				if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
				{
					cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
					goto END;
				}
				sleep(1);//ME15等进入休眠需要一点时间
				if((ret=NDK_BTStatus(&status)) != NDK_OK)
				{
					cls_show_msg("line %d:获取蓝牙状态失败(%d)", __LINE__, ret);
					goto END;
				}	
				if(status)//休眠唤醒后若是断开状态则进行重新连接应该成功且能正常数据通讯
				{
					cls_printf("使用手机蓝牙工具进行重新连接");
					if((ret=lib_bt_pairing(1))!=NDK_OK)
					{
						cls_show_msg("line %d:重新配对连接失败(%d)", __LINE__,ret);
						goto END;
					}
				}
				memset(cmdrecbuf,0,sizeof(cmdrecbuf));
				memset(sendbuf,0x38,sizeof(sendbuf));
#if BTMFI_ENABLE
				//在连接上的情况下收手机发送的数据
				if(((ret=NDK_PortRead(PORT_NUM_BT, 12, cmdrecbuf, 5*1000, &len)) != NDK_OK) || (len != 12))
				{
					cls_show_msg("line %d:接收数据失败(%d,%d)", __LINE__, ret, len);
					goto END;
				}
				//比较数据
				if(memcmp(sendbuf, cmdrecbuf, 12))
				{
					cls_show_msg("line %d:数据校验失败", __LINE__);
					goto END;
				}			
#else
				//在连接上的情况下收手机发送的数据
				cmdlen=MIN_BTCMD_LEN+12;
				if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecbuf, 5*1000, &len)) != NDK_OK) || (len != cmdlen))
				{
					cls_show_msg("line %d:接收数据失败(%d,%d)", __LINE__, ret, len);
					goto END;
				}
				//解析数据
				memset(recbuf,0,sizeof(recbuf));
				if((ret = bt_cmd_uppacket(cmdrecbuf, 12, recbuf))!=BTCMD_OK)
				{
					cls_show_msg("line %d:解析数据包失败%d", __LINE__, ret);
					goto END;
				}
				//比较数据
				if(memcmp(sendbuf, recbuf, 12))
				{
					cls_show_msg("line %d:数据校验失败", __LINE__);
					goto END;
				}
				//发送07指令收到手机数据
				memset(cmdsendbuf, 0, sizeof(cmdsendbuf));
				bt_cmd_packet(0x07, recbuf, 12, cmdsendbuf);
				if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendbuf))!=NDK_OK)
				{
					cls_show_msg("line %d:POS发送数据失败(%d)", __LINE__, ret);
					goto END;
				}
#endif
				break;	
#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE
			case '3':
				cls_show_msg("按任意键进入休眠,休眠后等待180s自动唤醒");
				NDK_SysSetSuspendDuration(180);
				if((ret=NDK_SysGoSuspend()) != NDK_OK)
				{
					cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
					NDK_SysSetSuspendDuration(0);//恢复默认
					goto END;
				}
				sleep(1);//ME15等进入休眠需要一点时间
				if((ret=NDK_BTStatus(&status)) != NDK_OK)
				{
					cls_show_msg("line %d:获取蓝牙状态失败(%d)", __LINE__, ret);
					NDK_SysSetSuspendDuration(0);//恢复默认
					goto END;
				}
				if(status)//休眠唤醒后若是断开状态则进行重新连接应该成功且能正常数据通讯
				{
					cls_printf("使用手机蓝牙工具进行重新连接");
					if((ret=lib_bt_pairing(1))!=NDK_OK)
					{
						cls_show_msg("line %d:重新配对连接失败(%d)", __LINE__,ret);
						NDK_SysSetSuspendDuration(0);//恢复默认
						goto END;
					}
#if BTMFI_ENABLE
					cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
				}
				//休眠唤醒后若是未断开状态 则进行正常数据通讯	
				if((ret=bt_comm_once(12,0))!=NDK_OK)
				{
					cls_show_msg("line %d:数据收发失败(%d)", __LINE__,ret);
					NDK_SysSetSuspendDuration(0);//恢复默认
					goto END;
				}
				NDK_SysSetSuspendDuration(0);//恢复默认
				break;
#endif
			case ESC:
				cls_show_msg("请选择唤醒方式");
				continue;
			default:
				continue;
				break;
		}
		break;
	}	
	NDK_LedStatus(LED_RFID_RED_FLICK);
	cls_show_msg("子用例测试通过,任意键继续");
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	NDK_LedStatus(LED_RFID_RED_OFF);//关闭led灯
	return;
}

static void bt_abnormal(void)
{
  int keyin = 0;
  
  while(1)
    {
		keyin = cls_show_msg("1.异常1 2.异常2\n"
							 "3.异常3 4.异常4\n"
							 "5.异常5 6.异常6");
		switch(keyin) 
		{
			case '1':
				abnormal_test1();//case1:在（手机端）提示配对时，拒绝配对，不应连接上。再次连接，接受配对，应能连接成功；应在3种PIN配对模式（随机PIN、输入PIN、SSP）下分别验证。
				break;
			case '2':	
				abnormal_test2();//case2:在有效通信范围内，长时间放置机器后（某些机具在长时间无操作的待机态下，蓝牙会进入休眠），再使用蓝牙功能，应能连接成功；
				break;	
			case '3':
				abnormal_test3();//case3:在（手机端）提示配对时，POS重启或关机，重启后蓝牙应仍能正常工作；
				break;
			case '4':
				abnormal_test4();//case4:在成功连接到手机后，POS重启或关机，重启后蓝牙应仍能正常工作；
				break;	
			case '5':
				abnormal_test5();//case5:整机休眠后（不论整机休眠是否关闭蓝牙），（手机端）应能搜索到设备（蓝牙），并成功进行配对与连接 或 （手机端）通过与蓝牙模块的各种交互（包括：（配对）连接、数据通讯、断开等方式）应能唤醒整机；
				break;
			case '6':
				abnormal_test6();//case6:整机休眠唤醒后(不论是何种唤醒方式,如:按键唤醒、蓝牙唤醒、定时唤醒等),再重新进行蓝牙连接应成功(整机休眠关闭蓝牙的情况) 或 直接进行蓝牙数据通讯成功(整机休眠不关闭蓝牙的情况)
				break;
			case ESC:
				return;
			default:
				continue;
				break;
		}
	}	
	return;
}

#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15空间资源限制，屏蔽不必要的函数
//本用例用到的其它私有函数
int wati_paraing()
{
	int ret = -1, link_status =0, accept = 1, flag = 0;
	uint ulen = 0;
	char pkey[7] = {0};
	time_t diff = 0, oldtime = time(NULL);
	
	if(g_pair_mode==2||g_pair_mode==3)
	{	
		while(1)
		{
			if(cls_show_msg1(1, "从设备等待配对中...,退出请按[ESC]") ==  ESC)
				break;
			accept = 1;
			NDK_BTStatus(&link_status);
			if(!link_status)
			{
				//NDK_BTDisconnect();//断开放在主设备端 20170828 linying modify
				//cls_show_msg("蓝牙已连接,请取消配对后任意键继续");
				cls_show_msg1(5,"主pos与从POS已连接,任意键跳过配对");
				//cls_show_msg1(5,"配对成功,任意键继续");
				return 2;
			}
			ret = NDK_BTGetPairingStatus(pkey,&flag);
			if(ret == NDK_ERR_NOT_SUPPORT)//蓝牙不支持该功能
			{
				cls_show_msg("请确认从模式蓝牙模块是否支持此配对模式");
				return ret;
			}
			else if(ret == NDK_ERR)//配对模式需要为SSP和PassKey模式
			{
				cls_show_msg("设置的配对模式不支持本功能,请设置SSP和PassKey模式");
				return ret;
			}
			else if(ret == NDK_OK)
			{
				switch(flag)
				{
					case 0:
						if((diff=time(NULL)-oldtime)>60)
						{
							cls_show_msg("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
							return flag;
						}
						continue;
						break;
					case 1:
						if(pkey[0] == '\0')
						{
							cls_printf("输入主设备端显示的配对码:\n");
							if((ret=lib_kbgetinput(pkey, 0, 6, &ulen, INPUTDISP_PASSWD, 0, INPUT_CONTRL_LIMIT_ERETURN)) == NDK_ERR || ret == NDK_ERR_TIMEOUT)
							{
								accept = 0;
							}
						}
						else
						{
							if(cls_show_msg("[Enter]确认配对,[其他]取消配对")==ENTER)
								accept = 1;
							else
								accept = 0;
						}
						NDK_BTConfirmPairing(pkey, accept);
						break;
					case 2:
						cls_show_msg1(5,"配对成功,任意键继续");
						return flag;
						break;
					case 3:
						cls_show_msg("配对失败,任意键继续");
						return flag;
						break;
					default:
						cls_show_msg("检测到未知的配对状态(%d)",flag);
						return flag;
						break;
				}
			}
			else
			{
				cls_show_msg1(5,"未知的返回值(%d)",ret);
				return ret;
			}
		}
	}
	else	
		cls_show_msg("从POS等待主POS与之配对,配对成功后任意键继续");	
	return NDK_OK;
}

void bt_client_ras(void)
{
	int ret = -1,len = 0;
	char recbuf[BUFSIZE_BT]={0};
	
	cls_printf("等待主设备发送数据中..");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	 
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)",__LINE__,ret);
		//NDK_PortClose(PORT_NUM_BT);
		return;
	}
	while(1)
	{
		if(cls_show_msg1(2,"数据收发中.ESC退出")==ESC)
			break;
		//收数据
		len = 0;
		memset(recbuf,0,sizeof(recbuf));
		ret = NDK_PortRead(PORT_NUM_BT,sizeof(recbuf),recbuf,0,&len);//期望接收长度传入2047 超时时间为0 函数调用结果在中低端上存在差异 中端有读到数据会返回NDK_OK，低端有读到数据一样返回NDK_ERR_TIMEOUT
		//cls_show_msg1(1,"接收到数据长度为:%d,%d",len, ret);
		if(ret!=NDK_OK&&ret!=NDK_ERR_TIMEOUT)
		{
			cls_show_msg("line %d:从设备接收数据失败(%d)", __LINE__, ret);
			//NDK_PortClose(PORT_NUM_BT);
			return;
		}
		else if(len!=0)//if(ret!==NDK_OK&&len!=0)
		{
			//cls_show_msg1(1,"发送数据中，长度为:%d",len);
			//发数据
			//update_BT_packet(recbuf,len);
			if((ret=NDK_PortWrite(PORT_NUM_BT, len, recbuf)) != NDK_OK)	
			{
				cls_show_msg("line %d:从设备发送数据失败(%d)", __LINE__, ret);
				//NDK_PortClose(PORT_NUM_BT);
				return; 	
			}
		}					
	}
}

void bt_client(void)
{
	int ret = -1, nkey =0;
	uint ulen = 0;
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return;
	}
	
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while(1)
    {
		keyin = cls_show_msg("1.设置名称和PIN\n"
							 "2.设置蓝牙配对模式\n"
							 "3.等待配对"
							 "4.数据收发");
		switch(keyin) 
		{
		case '1':
			//进入蓝牙命令模式
			if((ret=NDK_BTEnterCommand())!=NDK_OK)
			{
				cls_show_msg("line %d:进入命令模式失败(%d)", __LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			//设置蓝牙名称
			cls_printf("当前蓝牙名称:\n");
			//为了兼容低端产品ME15使用封装函数
			if ((ret=lib_kbgetinput(BtName,0, sizeof(BtName)-1, &ulen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				strcpy(BtName, DEFAULT_BT_NAME);//不允许空BtName,故若取消输入,也要给个默认值
			if((ret=NDK_BTSetLocalName(BtName))!=NDK_OK)
			{
				cls_show_msg("file:%s,line:%d设置蓝牙名称失败(ret=%d)",__FILE__ ,__LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			//设置蓝牙PIN
			cls_printf("当前蓝牙PIN:\n");
			//为了兼容低端产品ME15使用封装函数
			if ((ret=lib_kbgetinput(BtPin,0, sizeof(BtPin)-1, &ulen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				strcpy(BtPin, DEFAULT_BT_PIN);//不允许空BtPin,故若取消输入,也要给个默认值
			if((ret=NDK_BTSetPIN(BtPin))!=NDK_OK)
			{
				cls_show_msg("file:%s,line:%d设置蓝牙名称失败(ret=%d)",__FILE__ ,__LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			//退出蓝牙命令模式
			if((ret=NDK_BTExitCommand())!=NDK_OK)
			{
				cls_show_msg("line %d:退出命令模式失败(%d)", __LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			break;
		case '2':
			//进入蓝牙命令模式
			if((ret=NDK_BTEnterCommand())!=NDK_OK)
			{
				cls_show_msg("line %d:进入命令模式失败(%d)", __LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			cls_show_msg1(2,"请设置蓝牙配对模式(提示:苹果手机只能设置just work模式)");
			nkey = cls_show_msg("1.Just Work\n"
								"2.PIN Code\n"
								"3.SSP PIN\n"
								"4.PassKey");
			switch(nkey)
			{	
				case '1':
					NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK);
					g_pair_mode = 0;
					cls_show_msg1(1,"设置为Just Work模式");
					break;
				case '2':
					NDK_BTSetPairingMode(PAIRING_MODE_PINCODE);
					g_pair_mode = 1;
					cls_show_msg1(1,"设置为PIN Code模式");
					break;
				case '3':
					NDK_BTSetPairingMode(PAIRING_MODE_SSP);
					g_pair_mode = 2;
					cls_show_msg1(1,"设置为SSP PIN模式");
					break;
				case '4':
					NDK_BTSetPairingMode(PAIRING_MODE_PASSKEY);
					g_pair_mode = 3;
					cls_show_msg1(1,"设置为PassKey模式");
					break;
				default:
					NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK);
					g_pair_mode = 0;
					cls_show_msg1(1,"默认Just Work模式");
					break;
			}
			//退出蓝牙命令模式
			if((ret=NDK_BTExitCommand())!=NDK_OK)
			{
				cls_show_msg("line %d:退出命令模式失败(%d)", __LINE__, ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			break;
		case '3':
			if((ret = wati_paraing()) != NDK_OK && ret != 2)
			{				
				cls_show_msg("line %d:配对连接失败(%d)",__LINE__,ret);
			}	
			break;
		case '4':
			bt_client_ras();
			break;
		case ESC:
			NDK_PortClose(PORT_NUM_BT);
			return;
			break;
		default:
			continue;
			break;
		}
	}
	NDK_SysMsDelay(1000);//延时100ms  为了避免从设备过早断开  2017028 linying added
    if((ret=NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg("line %d:断开蓝牙失败(%d)", __LINE__, ret);
		return ;
	}
	if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:蓝牙串口关闭失败(%d)", __LINE__, ret);
		return;
	}
	return;
}
#endif

void systest57(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
#if 1//K21_ENABLE
	cls_show_msg1(5, "提示:测试前先进行配置,异常测试重启后也需配置,任意键继续");
#endif
	setlinktype(BT);//设置通讯类似为BT，避免数据包太长引起失败

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
			return;
		g_CycleTime = 100;

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行压力测试
		bt_press();

		//执行性能测试
		bt_ability();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while(1)
    {
		keyin = cls_show_msg("1.压力 2.性能\n"
							 "0.配置 3.异常\n"
#if BT_MASTER_ENABLE
							 "4.蓝牙客户端"
#endif
							 );
		switch(keyin) 
		{
			case '1':
				bt_press();
				break;
			case '2':	
				bt_ability();
				break;	
			case '3':
				bt_abnormal();
				break;
			case '0':
				conf_conn_BT();
#if BTMFI_ENABLE
			NDK_BTSetBleStatus(0);//在MFI协议下，关闭BLE方式
#endif
				break;
#if BT_MASTER_ENABLE
			case '4':
				bt_client();
				break;
#endif
			case ESC:
				cls_show_msg("请在配置处修改蓝牙配对模式后重新进行本用例测试");
				return;
			default:
				continue;
				break;
		}
	}	
	return;
}

