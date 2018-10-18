/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 蓝牙主模式综合	
* file name			: systest85.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 蓝牙主模式综合测试
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
#include <pthread.h>

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"蓝牙主模式综合"
#define MAXWAITTIME		30
#define DEFAULT_CNT_VLE	20
#define	DEFAULT_CNT_STR	"20"
#define	PACKETLIFE		(10)
#define	MAX_SIZE	BUFSIZE_BT-MIN_BTCMD_LEN

/*------------global variables definition-----------------------*/
int thread_exit_flag = 0;
int connect_type = 0;
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
					 	jiangym			20150611	created
*****************************************************************/	
#if 1
static void create_bt_ability_packet(Packet *packet, char *buf)
{
	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len= BUFSIZE_BT;
	packet->lifecycle = PACKETLIFE;//10;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}
#endif
#if 0
//扫描获取mac
int lib_btmaster_scan(char *pszMac, int dectype)
{
	int ret = 0,snum=0,i=0;
	ST_BT_DEV sresult[15];
	
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		cls_show_msg1(5, "line %d:主设备扫描失败(%d)", __LINE__, ret);
		return ret;	
	}
	
	memset(sresult,0,sizeof(sresult));
	while(1)
	{
		if(cls_show_msg1(1,"正在扫描蓝牙从模式设备,ESC键退出")==ESC)
			break;
		if((ret=NDK_BTMasterGetScanResults(NULL,sresult,15,&snum))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备获取扫描结果失败(%d)", __LINE__, ret);
			NDK_BTMasterStopScan();
			return ret;
		}
		if (snum > 0)
		{
			for (i=0; i<snum; i++)
			{
				if(cls_show_msg("是否选择BT[%d]:%s MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x作为测试从设备,是按[确认],否则按其它", i, sresult[i].sBtName,sresult[i].sBtMac[0],sresult[i].sBtMac[1],sresult[i].sBtMac[2],sresult[i].sBtMac[3],sresult[i].sBtMac[4],sresult[i].sBtMac[5])==ENTER)
				{
					strcpy(pszMac,sresult[i].sBtMac);
					NDK_BTMasterStopScan();
					return NDK_OK;
				}
			}
		}
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		cls_show_msg("line %d:主设备停止扫描失败(%d)", __LINE__, ret);
		NDK_BTMasterStopScan();
		return ret;
	}
	cls_show_msg1(2, "line %d:未扫描到测试用的蓝牙从设备", __LINE__);
	return NDK_ERR_QUIT;
}

//蓝牙从设备的操作(蓝牙客户端)
static void bt_client(void)
{
	int ret = -1, keyin = 0;
	int link_status =0, accept = 1, flag = 0, ulen = 0;
	char pkey[7] = {0};
	char recbuf[BUFSIZE_BT]={0} ;
	int len = 0;
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	/*process body*/
	cls_show_msg("蓝牙客户端先进行配置蓝牙配对模式,任意键继续");
	while(1)
    {
        keyin = cls_show_msg("1.打开连接\n"
							 "2.数据收发\n"
							 "3.断开关闭\n"
							 "0.配置");
		switch(keyin) 
		{
		case '1':
			if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
			{
				cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
				return ;
			}
			cls_show_msg1(1,"打开蓝牙成功");
			if(g_pair_mode==2||g_pair_mode==3)
			{
				while(1)
				{
					accept = 1;
					NDK_BTStatus(&link_status);
					if(!link_status)//判断出已连接 直接返回??
					{
						cls_show_msg("主设备与从设备已连接,任意键继续");
						return;
					}
					else
					{	ret = NDK_BTGetPairingStatus(pkey,&flag);
						if(ret == NDK_ERR_NOT_SUPPORT)//蓝牙不支持该功能
						{
							cls_show_msg("请确认从模式蓝牙模块是否支持此配对模式");
							return;
						}
						else if(ret == NDK_ERR)//配对模式需要为SSP和PassKey模式
						{
							cls_show_msg("设置的配对模式不支持本功能,请设置SSP和PassKey模式");
							return;
						}
						else if(ret == NDK_OK)
						{
							switch(flag)
							{
								case 0:
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
									//return ret;
									break;
								case 3:
									if(cls_show_msg("配对失败,[ESC]退出,[其他]重新连接配对")==ESC)
										return;
									cls_printf("请将主POS与从POS重新配对连接");
									memset(pkey,0,sizeof(pkey));
									break;
								default:
									cls_show_msg("检测到未知的配对状态(%d)",flag);
									return;
									break;
							}
						}
						else
						{
							cls_show_msg1(5,"未知的返回值(%d)",ret);
							return;
						}
					}
				}
			}
			else	
				cls_show_msg("从POS等待主POS与之配对,配对成功后任意键继续");
			break;
		case '2':
			memset(recbuf, 0, sizeof(recbuf));
			if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	 
			{				
				cls_show_msg("line %d:清缓冲区失败(%d)",__LINE__,ret);
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
			while(1)
			{			
				//收数据
				ret = NDK_PortRead(PORT_NUM_BT,sizeof(recbuf),recbuf,8000,&len);
				if(ret==NDK_ERR_TIMEOUT || cls_show_msg1(1,"数据收发中.ESC退出")==ESC)
					break;
				else if(ret==NDK_OK)
				{
					//发数据
					update_BT_packet(recbuf,len);
					if((ret=NDK_PortWrite(PORT_NUM_BT, len, recbuf)) != NDK_OK)	/*往蓝牙主设备发送数据*/
					{
						cls_show_msg("line %d:从设备发送数据失败(%d)", __LINE__, ret);
						NDK_PortClose(PORT_NUM_BT);
						return; 	
					}
				}
				else
				{
					cls_show_msg("line %d:从设备接收数据失败(%d)", __LINE__, ret);
					NDK_PortClose(PORT_NUM_BT);
					return;
				}						
			}
			break;
		case '3':
			if((ret=NDK_BTDisconnect()) != NDK_OK)
			{
				cls_show_msg("line %d:断开蓝牙失败(%d)", __LINE__, ret);
				return ;
			}
			if((ret=NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
			{
				cls_show_msg("line %d:关闭蓝牙串口失败(%d)", __LINE__, ret);
				return ;
			}
			cls_show_msg1(1,"断开关闭蓝牙成功");
			break;
		case '0':
			conf_conn_BT();
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
#endif
//数据传输(双向):主设备与从设备的读写压力测试
static void bt_master_read_write(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, succ = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	int len = 0;
	char bt_mac[7] = {0};
	Packet sendpacket;
	
	/*process body*/
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//连接(从设备提示连接)
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备连接从设备失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//未配对的会进行配对,已配对的就直接连接上
	if((ret=connect_pair_process(bt_mac,1))!=NDK_OK)
	{
		cls_show_msg("line %d:主从设备连接配对失败(%d)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg("蓝牙客户端打开数据收发,任意键继续");
	while(1)
	{
		if(cls_show_msg1(2,"开始第%d次蓝牙主从设备读写压力(已成功%d次),ESC退出", ++i, succ)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
			continue;	
		}
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac,BUFSIZE_BT,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
			continue;	
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(bt_mac, BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, len);
			continue;
		}
		//比较主从设备的数据
		if(len != BUFSIZE_BT)
		{
			cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, len, BUFSIZE_BT);
			continue;
		}
		if(memcmp(sendBuf, recvBuf, len))
		{
			cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
			continue;
		}
		succ++;
	}	
	//主设备断开连接
	if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	cls_show_msg("蓝牙主从设备读写压力测试完成,执行次数:%d次,成功%d次", i-1, succ);
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}
//链路层连接
static void bt_master_connect(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, cnt =0, succ = 0;
	char bt_mac[7] = {0};
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;

	/*process body*/
	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//从设备等待连接配对(从设备提示连接)
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	while(1)
	{
		if(i++>=cnt)//达到测试次数后退出
			break;
		if(cls_show_msg1(2,"蓝牙链路层连接压力测试中\n总共:%d次,已执行:%d次,已成功:%d次,按ESC退出",cnt,i-1,succ)==ESC)
			break;			
		//连接
		if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备连接从设备失败(%d)", __LINE__, ret);
			continue;	
		}
		//未配对的会进行配对,已配对的就直接连接上
		if((ret=connect_pair_process(bt_mac,1))!=NDK_OK)
		{
			cls_show_msg("line %d:主从设备连接配对失败(%d)", __LINE__, ret);
			continue;
		}
		//主设备断开连接
		if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
			continue;	
		}
		succ++;
	}
	cls_show_msg("总共进行%d次蓝牙链路层连接压力测试,已成功%d次",i-1,succ);
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}
//链路层连接+数传即流程压力
static void bt_master_flow(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, succ = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	int len = 0;
	char bt_mac[7] = {0};
	Packet sendpacket;
	
	/*process body*/
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	while(1)
	{
		if(cls_show_msg1(2,"开始第%d次主设备蓝牙流程压力测试(已成功%d次),ESC退出", ++i, succ)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		
		cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
		//连接
		if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备连接从设备失败(%d)", __LINE__, ret);
			continue;	
		}
		//未配对的会进行配对,已配对的就直接连接上
		if((ret=connect_pair_process(bt_mac,1))!=NDK_OK)
		{
			cls_show_msg("line %d:主从设备连接配对失败(%d)", __LINE__, ret);
			continue;
		}

		cls_show_msg("蓝牙客户端打开数据收发,任意键继续");
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
			continue;	
		}
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac,BUFSIZE_BT,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
			continue;	
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(bt_mac, BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, len);
			continue;
		}
		//比较主从设备的数据
		if(len != BUFSIZE_BT)
		{
			cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, len, BUFSIZE_BT);
			continue;
		}
		if(memcmp(sendBuf, recvBuf, len))
		{
			cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
			continue;
		}
		//主设备断开连接
		if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
			continue;	
		}
		succ++;
	}	
	cls_show_msg("蓝牙主设备流程压力测试完成,执行次数:%d次,成功%d次", i-1, succ);
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}
//主设备蓝牙压力测试主函数
static void bt_master_press()
{
	/*private & local definition*/
	int key=0;
	
	/*process body*/
	while (1)
	{
		key = cls_show_msg("1.数传压力\n"
							"2.连接压力\n"
							"3.连接+数传压力");
		
		switch(key)
		{
		case '1':
			bt_master_read_write();
			break;
		case '2':	
			bt_master_connect();
			break;	
		case '3':
			bt_master_flow();
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
#if 1
//主设备与从设备的读写性能
static void bt_master_ability()
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	char bt_mac[7] = {0};
	Packet sendpacket;
	float commtimes = 0, rate = 0;
	
	/*process body*/
	create_bt_ability_packet(&sendpacket, sendBuf);
	
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return ;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//连接(从设备提示连接)
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备连接从设备失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//未配对的会进行配对,已配对的就直接连接上
	if((ret=connect_pair_process(bt_mac,1))!=NDK_OK)
	{
		cls_show_msg("line %d:主从设备连接配对失败(%d)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg("蓝牙客户端打开数据收发,任意键继续");
	cls_printf("性能测试中..");
	while(1)
	{
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
			goto ERR;
		}
		lib_StartStopwatch();
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac,sendpacket.len,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
			goto ERR;
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(bt_mac, sendpacket.len, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, recvLen);
			goto ERR;
		}
		commtimes+=lib_ReadStopwatch();
		//比较主从设备的数据
		if(recvLen != sendpacket.len)
		{
			cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, recvLen, sendpacket.len);
			goto ERR;
		}
		if(MemCmp(sendBuf, recvBuf, sendpacket.len))
		{
			cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
			goto ERR;
		}
	}	
	//主设备断开连接
	if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	
	rate = (float)sendpacket.len/(commtimes/PACKETLIFE);
	cls_show_msg("蓝牙主从设备读写数据通讯速率为%fB/s", rate);
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}
#endif
//主从设备的数据读写线程
void *pthread_master(char *mac)
{
	/*private & local definition*/
	int ret = -1, j = 0, k = 0,  len = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	
	//生成测试数据
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	
	while(!thread_exit_flag)
	{	
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(mac)) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清[MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x]从设备缓冲区数据失败(%d)", __LINE__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], ret);
			NDK_PthreadJoin(pthread_self(),NULL);//退出线程的操作
	        return (void *)0;
		}
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(mac,BUFSIZE_BT,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备[MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x]写数据失败(%d)", __LINE__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], ret);
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;	
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(mac, BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取从设备[MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x]数据失败(%d,%d)", __LINE__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], ret, len);
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
		}
		//比较主从设备的数据
		if(len != BUFSIZE_BT)
		{
			cls_show_msg("line %d:主设备与从设备[MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x]数据长度收发不一致(实际:%d,预期:%d)", __LINE__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], len, BUFSIZE_BT);
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
		}
		if(memcmp(sendBuf, recvBuf, len))
		{
			cls_show_msg("line %d:主设备与从设备[MAC为:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x]收发数据校验失败", __LINE__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
		}
		if(k++ == 10)//通讯次数可以改变  通讯次数从20次改成10次 
			thread_exit_flag = 1;
	}	

	//退出线程
	return (void *)0;
}

//主设备作为从设备的数据读写线程
void *pthread_slave(void *arg)
{
	/*private & local definition*/
	int ret = -1, i = 0, recvLen = 0, j = 0;
	char sendBuf[MAX_SIZE] = {0}, recvBuf[MAX_SIZE] = {0};
	int cmdlen = 0;
    char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	/*process body*/
	//生成测试数据
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	update_BT_packet(sendBuf,MAX_SIZE);
	while(!thread_exit_flag)
	{
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK) 	//清接收缓冲
	    {				 
		    cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);		    
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
	    }
	    memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	    bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	    cmdlen=MAX_SIZE+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
	    if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	    {
		    cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);		    
			NDK_PthreadJoin(pthread_self(),NULL);//测试失败后关闭线程
	        return (void *)0;
	    }
	    memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	    if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	    {
		    cls_show_msg("line %d:接收错(%d)(实际%d, 预期%d)", __LINE__, ret, recvLen, cmdlen);
			NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
	    }
	    memset(recvBuf,0,sizeof(recvBuf));
	    bt_cmd_uppacket(cmdrecvBuf, MAX_SIZE, recvBuf);
	    if(memcmp(sendBuf, recvBuf, MAX_SIZE))
	    {
		    cls_show_msg("line %d:数据校验失败", __LINE__);
		    NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
	    }
	    memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	    bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	    if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	    {
		    cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);
		    NDK_PthreadJoin(pthread_self(),NULL);
	        return (void *)0;
	    }
		if(i++ == 10)
			thread_exit_flag= 1;
	}	
	//退出线程
	return (void *)0;
}

//主设备连接三从设备,同时主设备又作为从设备被手机连接,使用四个线程同时进行
//缺点:出错处理还不够完善,只能提示某台从设备失败,还不能及时把其他线程结束,需要都执行完才能结束
static void bt_master_slave()
{
	int ret = -1;
	char bt_mac[3][6] ={{0},{0},{0} };
	char outbuf[16] = {0};
	pthread_t tidp[3], tidp1;
	pthread_attr_t attr;
	int error = -1, error1 = -1, i = 0;
	
	
	/*process body*/
	//测试前置
	thread_exit_flag=0;
	NDK_PortClose(PORT_NUM_BT);
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return;
	}
	//进行扫描连接三台从设备
	cls_show_msg("主设备连接三台从设备,任意键继续");
	for(i=0;i<3;i++)
	{	//扫描获取mac
		if((ret=lib_btmaster_scan(bt_mac[i], connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
			goto ERR;	
		}
		cls_show_msg("蓝牙[%d]客户端打开等待配对,任意键继续",i+1);
		//发起配对
		if((ret=NDK_BTMasterBond(bt_mac[i]))!=NDK_OK)
	    {
		    cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		    goto ERR;
	    }
		//进行配对
	    if((ret=lib_btmaster_pairing(1))!=2)
	    {
		    cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		    goto ERR;
	    }
		//连接
		if((ret=NDK_BTMasterConnect(bt_mac[i],connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
			goto ERR;	
		}
	} 
    cls_show_msg("把此设备作为从设备进行测试,任意键继续");
	//是否要设置不同的模式
    memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		cls_show_msg("line %d:获取蓝牙名称失败(%d)", __LINE__, ret);
		goto ERR2;
	}
    //重新配置蓝牙参数为SSP模式，  调用NDK_BTMasterBond后，底层配对模式已经修改为SSP模式，需要重新进行配置，只支持主从模式一致
    NDK_BTSetPairingMode(PAIRING_MODE_SSP);
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
	    cls_show_msg("line %d:配对失败,请重新配对(%d)", __LINE__,ret);
		goto ERR2;
	}
	cls_show_msg("三台从设备蓝牙客户端打开数据收发，任意键继续");	
	sleep(1);
	cls_printf("数据收发多线程进行中..");
	//预防内存泄漏
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	//创建3个主从设备数据收发子线程即一主多从的模式
	for(i=0;i<3;i++)
	{
		if((error=NDK_PthreadCreate(&tidp[i],&attr,(void *)pthread_master,bt_mac[i]))!=0) 
		{
			cls_show_msg("line %d:主从设备数据收发线程失败(%d)\n", __LINE__, error);
			goto ERR2;
		}
	}	
	//创建1个主设备作为从设备的线程
	if((error1=NDK_PthreadCreate(&tidp1,NULL,(void *)pthread_slave,NULL))!=0) 
	{
		cls_show_msg("line %d:主设备作为从设备数据收发线程失败(%d)\n", __LINE__, error1);
		goto ERR2;
	}
	while(1)
	{
		if(thread_exit_flag==1)	
			break;
	}
	//等待子线程结束
	for(i=0;i<3;i++)
	{
		 NDK_PthreadJoin(tidp[i],NULL);
	}
	NDK_PthreadJoin(tidp1,NULL);
	cls_show_msg("子用例测试通过,任意键继续");
ERR2:
	NDK_BTDisconnect();
ERR:
	for(i=0;i<3;i++)
		NDK_BTMasterDisconnect(bt_mac[i]);
	NDK_PortClose(PORT_NUM_BT);
	return;
}
//主设备连接多台从设备即一主多从测试
static void master_more_slave()
{	
	int ret = -1, i = 0, j = 0, recvLen = 0;
	char bt_mac[3][6] ={{0},{0},{0}};
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};//加入报文头后的命令

	/*process body*/
	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	//生成测试数据
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return;
	}
	//进行扫描连接三台从设备
	for(i=0;i<3;i++)
	{	//扫描获取mac
		cls_show_msg("主设备与从设备[%d]进行连接、数据收发,任意键继续",i+1);
		if((ret=lib_btmaster_scan(bt_mac[i], connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
			goto ERR;	
		}
		cls_show_msg("从设备[%d]蓝牙客户端打开等待配对,任意键继续",i+1);
		//发起配对
	    if((ret=NDK_BTMasterBond(bt_mac[i]))!=NDK_OK)
	    {
		    cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		    goto ERR;
	    }
		//进行配对
	    if((ret=lib_btmaster_pairing(1))!=2)
	    {
		    cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		    goto ERR;
	    }
		//连接
		if((ret=NDK_BTMasterConnect(bt_mac[i],connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
			goto ERR;	
		}
		cls_show_msg("从设备[%d]蓝牙客户端打开数据收发,任意键继续",i+1);
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac[i])) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
			goto ERR;
		}
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac[i],BUFSIZE_BT,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
			goto ERR;
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(bt_mac[i], BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, recvLen);
			goto ERR;
		}
		//比较主从设备的数据
		if(recvLen != BUFSIZE_BT)
		{
			cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, recvLen, BUFSIZE_BT);
			goto ERR;
		}
		if(MemCmp(sendBuf, recvBuf, BUFSIZE_BT))
		{
			cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
			goto ERR;
		}	   
	}
	cls_show_msg("子用例测试通过,任意键继续");
ERR:
	for(i=0;i<3;i++)
		NDK_BTMasterDisconnect(bt_mac[i]);
	NDK_PortClose(PORT_NUM_BT);
	return;
}
//主从模式共存测试(既作为从设备与手机通讯,又作为主设备与从设备进行通讯)
static void master_slave_coexist()
{
	/*private & local definition*/
	int ret = -1, j = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	int len = 0, recvLen = 0;
	char bt_mac[7] = {0};
	char outbuf[16] = {0};
    int cmdlen = 0;
    char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	NDK_PortClose(PORT_NUM_BT);
	//生成测试数据
	for (j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	update_BT_packet(sendBuf,BUFSIZE_BT);
	
	//pos作为从设备进行读写
	cls_show_msg("把此pos作为从设备进行测试,任意键继续");
	conf_conn_BT();
	//conf_bt();//是否使用这个设置函数来设置还是另外封装个简单的?
	if((ret = NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
		return;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		cls_show_msg("line %d:获取蓝牙名称失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if(g_pair_mode == 2 || g_pair_mode == 3)  // SSP PIN和PassKey两种配对模式 需要调用配对状态查询和配对确认函数进行配对操作
	{	
		cls_show_msg("把之前已配对的蓝牙取消配对后任意键继续");
		cls_printf("手机使用BTFWATS与主设备蓝牙:%s配对连接",outbuf);
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg("line %d:配对失败,请重新配对(%d)", __LINE__,ret);
			goto ERR;
		}
	}
	else //Just Work 和 PIN Code两种配对模式可以直接操作配对
		cls_show_msg("手机使用BTFWATS,与主设备蓝牙:%s连接后任意键继续",outbuf);
	cls_show_msg1(3,"进行%dB数据读写",MAX_SIZE); 
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK) 	//清接收缓冲
	{				 
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);
		goto ERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);
		goto ERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	{
		cls_show_msg("line %d:接收错(%d)(实际%d, 预期%d)", __LINE__, ret, recvLen, cmdlen);
		goto ERR;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	bt_cmd_uppacket(cmdrecvBuf, MAX_SIZE, recvBuf);
	if(memcmp(sendBuf, recvBuf, MAX_SIZE))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		goto ERR;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	cls_show_msg("把此pos作为主设备进行测试,任意键继续");
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	//发起配对
	if((ret=NDK_BTMasterBond(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		goto ERR2;
	}
	//进行配对
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		goto ERR2;
    }
	//连接
    if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
    {
		cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
		goto ERR2;	
	}
	cls_show_msg("蓝牙客户端打开数据收发,任意键继续");
	//清指定设备缓冲区NDK_BTMasterClrBuf
	if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
	{
		cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
		goto ERR2;	
	}
	//往从设备写数据
	if((ret=NDK_BTMasterWrite(bt_mac,BUFSIZE_BT,sendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
		goto ERR2;	
	}
	//主设备读从设备数据
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret=NDK_BTMasterRead(bt_mac, BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
	{				
		cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, len);
		goto ERR2;
	}
	//比较主从设备的数据
	if(len != BUFSIZE_BT)
	{
		cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, len, BUFSIZE_BT);
		goto ERR2;
	}
	if(memcmp(sendBuf, recvBuf, len))
	{
		cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
		goto ERR2;
	}
	cls_show_msg("子用例测试通过,任意键继续");
ERR2:
	NDK_BTMasterDisconnect(bt_mac);
ERR:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//主从模式切换测试
static void master_slave_change()
{
	/*private & local definition*/
	int ret = -1, i = 0, j = 0;
	char sendBuf[BUFSIZE_BT] = {0}, recvBuf[BUFSIZE_BT] = {0};
	int len = 0, recvLen = 0;
	char bt_mac[7] = {0};
	char outbuf[16] = {0};
    int cmdlen = 0;
    char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	/*process body*/
	while(1)
	{
		if(ESC==cls_show_msg1(2, "正在进行第%d次主从模式切换测试,按ESC退出", i+1))
			break;
		if( i++ > 3 )
			break;	
		
		//生成测试数据
		for (j=0; j<sizeof(sendBuf); j++)
			sendBuf[j]=rand()%256;
		update_BT_packet(sendBuf,BUFSIZE_BT);
		
		//pos作为主设备与从设备进行读写
		//测试前置
		cls_show_msg("pos作为主设备进行测试,任意键继续");
		NDK_PortClose(PORT_NUM_BT);
		if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
		{
			cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
			return;
		}
		//扫描获取mac
		if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_BT);
			return;
		} 
		cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
		//发起配对
		if((ret=NDK_BTMasterBond(bt_mac))!=NDK_OK)
	        {
		 	cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		 	NDK_PortClose(PORT_NUM_BT);
			return;
	         }
		//进行配对
	       if((ret=lib_btmaster_pairing(1))!=2)
	       {
		    	cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		    	NDK_PortClose(PORT_NUM_BT);
		   	return;
	        }
		//连接
		if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
		{
			cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
			NDK_PortClose(PORT_NUM_BT);
			return;	
		}
		cls_show_msg("蓝牙客户端打开数据收发,任意键继续");
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
		{
			cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_BT);
			return;	
		}
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac,BUFSIZE_BT,sendBuf))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_BT);
			return;	
		}
		//主设备读从设备数据
		memset(recvBuf,0,sizeof(recvBuf));
		if((ret=NDK_BTMasterRead(bt_mac, BUFSIZE_BT, recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
		{				
			cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, len);
			NDK_PortClose(PORT_NUM_BT);
			return;
		}
		//比较主从设备的数据
		if(len != BUFSIZE_BT)
		{
			cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, len, BUFSIZE_BT);
			NDK_PortClose(PORT_NUM_BT);
			return;
		}
		if(memcmp(sendBuf, recvBuf, len))
		{
			cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
			NDK_PortClose(PORT_NUM_BT);
			return;
		}
		//主设备断开连接
		if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_BT);
			return;	
		}
		NDK_PortClose(PORT_NUM_BT);

		//pos作为从设备进行读写
		cls_show_msg("把此pos作为从设备进行测试,任意键继续");
		conf_conn_BT();
		//conf_bt();
		if((ret = NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
			return;
		}
		memset(outbuf,0,sizeof(outbuf));
		if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
		{
			cls_show_msg("line %d:获取蓝牙名称失败(%d)", __LINE__, ret);
			NDK_BTDisconnect();
			NDK_PortClose(PORT_NUM_BT);
			return;
		}
		if(g_pair_mode == 2 || g_pair_mode == 3)   
		{	
			cls_show_msg("把之前已配对的蓝牙取消配对后任意键继续");
			cls_printf("手机使用自动化工具BTFWATS与POS:%s配对连接",outbuf);
			if((ret=lib_bt_pairing(1))!=NDK_OK)
			{
				cls_show_msg("line %d:配对失败,请重新配对(%d)", __LINE__,ret);
				NDK_BTDisconnect();
				NDK_PortClose(PORT_NUM_BT);
				return;
			}
		}
		else //Just Work 和 PIN Code两种配对模式可以直接操作配对
			cls_show_msg("手机使用BTFWATS,与POS:%s连接后任意键继续",outbuf);
	cls_show_msg1(3,"进行%dB数据读写",MAX_SIZE);
	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)	//清接收缓冲
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendBuf, MAX_SIZE, cmdsendBuf);
	cmdlen=MAX_SIZE+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	{
		cls_show_msg("line %d:接收错(%d)(实际%d, 预期%d)", __LINE__, ret, recvLen, cmdlen);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	memset(recvBuf,0,sizeof(recvBuf));
	bt_cmd_uppacket(cmdrecvBuf, MAX_SIZE, recvBuf);
	if(memcmp(sendBuf, recvBuf, MAX_SIZE))
	{
		cls_show_msg("line %d:数据校验失败", __LINE__);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:发送失败(%d)", __LINE__, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	//断开蓝牙连接
	if((ret = NDK_BTDisconnect()) != NDK_OK)
	{
		cls_show_msg("line %d:断开蓝牙连接失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_BT);
		return;
	}
	NDK_PortClose(PORT_NUM_BT);
	}
	cls_show_msg("主从切换测试通过,任意键继续");
	return;
}

//长时间放置后再使用蓝牙功能应能连接成功
static void master_abnormal_test1()
{
	int ret = -1;
	char bt_mac[7] = {0};
	
	cls_show_msg("长时间放置机器后,再按任意键继续");
	if((ret = NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(%d)", __LINE__, ret);
    	return;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	} 
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	//发起配对
	if((ret=NDK_BTMasterBond(bt_mac))!=NDK_OK)
	{
	    cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//进行配对
    if((ret=lib_btmaster_pairing(1))!=2)
	{
		cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//连接
    if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
		goto ERR;	
    }
	NDK_SysDelay(30);//防止主从设备还未响应连接状态前，就直接执行断开连接的动作
	//主设备断开连接
	if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	cls_show_msg("子用例测试通过,任意键继续");
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//休眠(关闭蓝牙)唤醒之后再重新连接应能成功
static void master_abnormal_test2()
{
	int ret = -1;
	char bt_mac[7] = {0};
	
	cls_show_msg("按任意键机器进休眠,休眠后按键唤醒");
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
	{
		cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
		return;
	}
	NDK_SysDelay(5); 
	if((ret = NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(%d)", __LINE__, ret);
    	return;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	} 
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	//发起配对
	if((ret=NDK_BTMasterBond(bt_mac))!=NDK_OK)
	{
	    cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//进行配对
    if((ret=lib_btmaster_pairing(1))!=2)
	{
		cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//连接
    if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
		goto ERR;	
    }
	sleep(1);//连接是非阻塞,连接需要时间,无延时立马断开会返回-1  20180514 modify
	//主设备断开连接
	if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	cls_show_msg("子用例测试通过,任意键继续");
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//休眠(不关闭蓝牙)唤醒后能数据通讯成功
static void master_abnormal_test3()
{
	int ret = -1, len = 0;
	char bt_mac[7] = {0};
	char sendBuf[12]={0},recvBuf[12]={0};
	
	if((ret = NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
    	cls_show_msg("line %d:测试失败(%d)", __LINE__, ret);
    	return;
	}
	//扫描获取mac
	if((ret=lib_btmaster_scan(bt_mac, connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
		goto ERR;	
	} 
	cls_show_msg("蓝牙客户端打开等待配对,任意键继续");
	//发起配对
	if((ret=NDK_BTMasterBond(bt_mac))!=NDK_OK)
	{
	    cls_show_msg("line %d:配对发起失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//进行配对
    if((ret=lib_btmaster_pairing(1))!=2)
	{
		cls_show_msg("line %d:配对失败(%d)", __LINE__,ret);
		goto ERR;
	}
	//连接
    if((ret=NDK_BTMasterConnect(bt_mac,connect_type))!=NDK_OK)
	{
		cls_show_msg("line %d:连接失败(%d)", __LINE__,ret);
		goto ERR;	
    }
	cls_show_msg("请等待约5秒后按任意键进休眠,休眠后按键唤醒");
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
#endif
	{
		cls_show_msg("line %d:休眠失败(%d)", __LINE__, ret);
		goto ERR;
	}
	memset(sendBuf, 0x38, sizeof(sendBuf));
	cls_show_msg("蓝牙客户端打开数据收发");
	//清指定设备缓冲区NDK_BTMasterClrBuf
	if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
	{
		cls_show_msg("line %d:主设备清缓冲区数据失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//往从设备写数据
	if((ret=NDK_BTMasterWrite(bt_mac,sizeof(sendBuf),sendBuf))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备往从设备写数据失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	//主设备读从设备数据
	memset(recvBuf,0,sizeof(recvBuf));
	if((ret=NDK_BTMasterRead(bt_mac, sizeof(sendBuf), recvBuf, MAXWAITTIME*1000, &len)) != NDK_OK)	
	{				
		cls_show_msg("line %d:主设备读取数据失败(%d,%d)", __LINE__, ret, len);
		goto ERR;
	}
	//比较主从设备的数据
	if(len != sizeof(sendBuf))
	{
		cls_show_msg("line %d:主设备数据长度收发不一致(实际:%d,预期:%d)", __LINE__, len, BUFSIZE_BT);
		goto ERR;
	}
	if(memcmp(sendBuf, recvBuf, len))
	{
		cls_show_msg("line %d:主设备收发数据校验失败", __LINE__);
		goto ERR;
	}
	//主设备断开连接
	if((ret=NDK_BTMasterDisconnect(bt_mac))!=NDK_OK)
	{
		cls_show_msg("line %d:主设备断开连接失败(%d)", __LINE__, ret);
		goto ERR;	
	}
	cls_show_msg("子用例测试通过,任意键继续");
ERR:
	//关闭串口
	NDK_PortClose(PORT_NUM_BT);
	return;
}

//主模式休眠异常测试
static void bt_master_abnormal()
{
	int keyin = 0;
	
	while(1)
	{
		keyin = cls_show_msg("1.长时间放置\n"
							 "2.休眠关闭蓝牙\n"
							 "3.休眠不关闭蓝牙");
		switch(keyin) 
		{
			case '1':
				master_abnormal_test1();
				break;
			case '2':	
				master_abnormal_test2();
				break;	
			case '3':
				master_abnormal_test3();
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

void bt_master_set(void)
{
	int keyin = 0;

	/*process body*/
	while(1)
	{
		keyin = cls_show_msg("请选择连接方式:"
							"1.默认方式\n"
							 "2.SPP方式\n" 
							 "3.BLE	方式");
		switch(keyin) 
		{
			case '1':
				connect_type = 0;
				return;
				break;
			case '2':	
				connect_type = 1;
				return;
				break;	
			case '3':
				connect_type = 2;
				return;
				break;
			default:
				connect_type = 0;
				return;
				break;
		}
	}	
	return;
}

//主模式蓝牙测试
void systest85(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	
	setlinktype(BT);//设置通讯类型为BT，避免数据包太大引起失败
	while(1)
	{
		keyin = cls_show_msg("1.压力 2.主从切换\n"
							 "3.主从多线程 8.设置\n"   //3可以拆成5和6
							 "4.异常 5.主从共存\n"
							 "6.一主多从 7.性能 ");
		switch(keyin) 
		{
			case '1':
				bt_master_press();
				break;
			case '2':	
				master_slave_change();
				break;	
			case '3':
				bt_master_slave();
				break;
			case '4':
				bt_master_abnormal();
				break;	
			case '5':
				master_slave_coexist();
				break;	
			case '6':
				master_more_slave();
				break;
			case '7': 
				bt_master_ability();
				break;
			case '8': 
				bt_master_set();
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


