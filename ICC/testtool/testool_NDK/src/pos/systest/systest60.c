/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	BT/WLM深交叉
* file name			: systest60.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: BT/WLM深交叉
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
#define	TESTITEM	"BT/WLM深交叉"
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
void BT_WLM(int nWlmType)
{
	/*private & local definition*/
	uint h_tcp = -1;
	int  ret = -1, i = 0, slen = 0, rlen = 0, recvLen = 0, succ=0, cmdlen = 0, nSq = 0, timeout = SO_TIMEO;
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
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "WLM参数未配置");	
		return;
	}
	
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	while (1)
	{	
		NDK_WlModemGetSQ(&nSq);
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出(信号值:%d)", ++i, TESTITEM, succ, nSq)==ESC)
			break;
		Local2Global(BT);
		if(update_snd_packet(&sendpacket)!=SUCC)//使用BT数据包进行测试
			break;
		//sleep(10);//每次挂断之后重新连接要等待10秒,减轻绎芯片的压力
		
		//WLM连接
		Local2Global(nWlmType);
		if(__LinkUp_WLM()!=SUCC ||__NetUp_PPP()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM连接失败",__LINE__,i);
			goto ERR;
		}
		Global2Local(nWlmType);

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
		
		/*WLM建立传输层*/
		Local2Global(nWlmType);
		if(__TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM TransUp失败", __LINE__, i);
			goto ERR2;
		}
		Global2Local(nWlmType);
		
		//蓝牙清缓冲区
		Local2Global(BT);
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			goto ERR3;
		}	
		
		/*Wlm发送数据*/
		Local2Global(nWlmType);
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM发送失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
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
		
		/*wlm接收数据*/
		Local2Global(nWlmType);
		memset(recvBuf, 0, sizeof(recvBuf));
		if((rlen=sock_recv(h_tcp, recvBuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM接收失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			goto ERR3;
		}
		//WLM校验收发数据
		if (MemCmp(sendpacket.header, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM数据校验失败", __LINE__, i);
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
		
		//WLM TransDown
		Local2Global(nWlmType);
		if (__TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM TransDown失败", __LINE__, i);
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
		
		//WLM NetDown
		Local2Global(nWlmType);
		if (__NetDown_PPP()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WLM NetDown失败", __LINE__, i);
			goto ERR;
		}					
		succ++;
		continue;
	ERR3:
		Local2Global(nWlmType);
		g_socktype = netsetting.socktype;
		__TransDown(h_tcp);	
	ERR2:
		Local2Global(BT);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
	ERR:
		Local2Global(nWlmType);
		__NetDown_PPP();
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s测试完成,已进行%d次,成功:%d次",TESTITEM,i-1,succ);
	return;
}

void systest60(void)
{
	/*private & local definition*/
	int ret = 0, nWlmType = 0;; 
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,BT配置失败,请检查配置文件",TESTITEM);
			return;
		}
		Global2Local(BT);
		nConfInit[0]=1;

		//MDM参数配置
		if(conf_conn_WLM(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WLM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		nWlmType = getlinktype();
		Global2Local(nWlmType);
		nConfInit[1]=1;
		
		g_CycleTime = 10;
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);	
		//执行交叉测试
		BT_WLM(nWlmType);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.蓝牙配置\n"
							"9.WLM配置");
		
		switch(ret)
		{
		case '1':
			BT_WLM(nWlmType);
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
			switch(conf_conn_WLM(TRUE))
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份wlm参数
				nWlmType = getlinktype();
				Global2Local(nWlmType);
				nConfInit[1]=1;
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
			return;
			break;
		default:
			continue;
			break;
		}
	}	
	return;
}


