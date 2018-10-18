/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	BT/WLAN深交叉
* file name			: systest61.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: BT/WLAN深交叉
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
#define	TESTITEM	"BT/WLAN深交叉"
#define MAXWAITTIME		10

/*------------global variables definition-----------------------*/
static int nConfInit[2] = {0};	 /*是否配置过了参数:0未配置 1已配置*/

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
					 	jiangym			20140304	created
*****************************************************************/	
void BT_WLAN(void)
{
	/*private & local definition*/
	uint h_tcp = -1;
	int  ret = -1, i = 0, slen = 0, rlen = 0, recvLen = 0, succ=0, cmdlen = 0, timeout = SO_TIMEO;
	char sendBuf[PACKMAXLEN] = {0}, recvBuf[PACKMAXLEN] = {0};//将BUFSIZE_BT-MIN_BTCMD_LEN修改成PACKMAXLEN，避免由于后设置以太网模块在初始化包的时候传入的缓冲小于设置时的长度引起跑飞
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};//加入报文头后的命令
	Packet sendpacket;
		
	/*process body*/
	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "BT参数未配置");
		return;
	}
	
	if(nConfInit[1] == 0)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "WLAN参数未配置");	
		return;
	}
	
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	while (1)
	{	
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, TESTITEM, succ)==ESC)
			break;
		Local2Global(BT);
		if(update_snd_packet(&sendpacket)!=SUCC)//使用BT数据包进行测试
			break;
		
		/* wifi连接*/
		Local2Global(WLAN);
		if(__LinkUp_WLAN()!=SUCC||__NetUp_WLAN()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN NetUp失败", __LINE__, i);
			goto ERR;
		}
		Global2Local(WLAN);
		
		//打开蓝牙串口
		Local2Global(BT);
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败(%d)", __LINE__, i, ret);
			goto ERR2;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			goto ERR2;
		}
		Global2Local(BT);
		
		/*wifi建立传输层*/
		Local2Global(WLAN);
		if(__TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN TransUp失败", __LINE__, i);
			goto ERR2;
		}
		Global2Local(WLAN);
		
		//蓝牙清缓冲区
		Local2Global(BT);
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			goto ERR3;
		}	
		
		/*wifi发送数据*/
		Local2Global(WLAN);
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN发送失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
			goto ERR3;
		}
		
		//蓝牙发送数据
		Local2Global(BT);
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x05, sendpacket.header, sendpacket.len, cmdsendBuf);
		cmdlen = sendpacket.len+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			goto ERR3;
		}

		/*wifi接收数据*/
		Local2Global(WLAN);
		memset(recvBuf, 0, sizeof(recvBuf));
		if((rlen=sock_recv(h_tcp, recvBuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN接收失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			goto ERR3;		
		}		
		//wifi校验收发数据
		if (MemCmp(sendpacket.header, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN数据校验失败", __LINE__, i);
			goto ERR3;	
		}
		
		//蓝牙接收数据
		Local2Global(BT);
		memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			bt_comm_once(12,0);
			goto ERR3;
		}
		bt_cmd_uppacket(cmdrecvBuf, sendpacket.len, recvBuf);
		if(memcmp(sendBuf, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙数据校验失败", __LINE__, i);
			goto ERR3;
		}
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			goto ERR3;
		}

		//wifi断开传输层
		Local2Global(WLAN);
		g_socktype = netsetting.socktype;
		if (__TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN TransDown失败", __LINE__, i);
			goto ERR3;
		}	
		
		//关闭蓝牙串口
		Local2Global(BT);
		//断开蓝牙连接
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x03, NULL, 0, cmdsendBuf);
		NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf);
		packetnum = 0;//连接断开后序列号要清0
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败", __LINE__, i);
			goto ERR2;
		}
		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			goto ERR2;
		}
		
		//wifi断开连接
		Local2Global(WLAN);
		if(__NetDown_WLAN()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLAN NetDown失败", __LINE__, i);
			goto ERR;
		}					
		succ++;
		continue;
	ERR3:
		Local2Global(WLAN);
		g_socktype = netsetting.socktype;
		__TransDown(h_tcp);	
	ERR2:
		Local2Global(BT);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
	ERR:
		Local2Global(WLAN);
		__NetDown_WLAN();	
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s测试完成,已进行%d次,成功:%d次",TESTITEM,i-1,succ);
	return;
}
void systest62(void)
{
	/*private & local definition*/
	int ret = 0, tmpflag = 0;  
		
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=0)
		SetWifiStopflag(0);//确保本用例在维持wifi连接的情况下测试

	if(auto_flag==1)
	{
		//自动化测试内容
		//BT配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,BT配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		Global2Local(BT);
		nConfInit[0]=1;
		
		//WLAN配置参数，失败则退出
		if(conf_conn_WLAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WLAN配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		Global2Local(WLAN);
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
		nConfInit[1]=1;
		
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		BT_WLAN();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.蓝牙配置\n"
							"9.WLAN配置");
		
		switch(ret)
		{
		case '1':
			BT_WLAN();
			break;
		case '0':
			//cls_show_msg("任意键进行BT参数配置...");
			switch(conf_conn_BT())
			{
			case SUCC:
				cls_show_msg1(2, "BT配置成功!");
				//备份wlan参数
				Global2Local(BT);
				nConfInit[0]=1;
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
			case NDK_ERR_QUIT://用户放弃
			default:
				continue;//return;
				break;
			}
			break;
		case '9':	
			switch(conf_conn_WLAN())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份wlan参数
				Global2Local(WLAN);
				if((cls_show_msg("wifi服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER ) 	
				{
					NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
					nConfInit[1]=1;
				}
				else
				{
					cls_show_msg("请重新配置，选择正确的路由表!!!");
					nConfInit[1]=0;
				}	
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				continue;//return;
				break;
			}
			break;		
		case ESC:
			SetWifiStopflag(tmpflag);
			return;
			break;
		default:
			continue;
			break;
		}
	}	

	SetWifiStopflag(tmpflag);
	return;
}


