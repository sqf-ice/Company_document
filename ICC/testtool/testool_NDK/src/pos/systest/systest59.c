/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	BT/MDM深交叉
* file name			: systest59.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: BT/MDM深交叉
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
#define	TESTITEM	"BT/MDM交叉"
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
					 	jiangym			20140228	created
*****************************************************************/	
void BT_MDM(int nMdmType)
{
	/*private & local definition*/
	int  ret = -1, i = 0, slen = 0, rlen = 0, recvLen = 0, succ = 0, cmdlen = 0;
	char sendBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0}, recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};
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
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "MDM参数未配置");	
		return;
	}
	
	setlinktype(nMdmType);
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	while (1)
	{		
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, TESTITEM, succ)==ESC)
			break;
		
		setlinktype(nMdmType);
		if(update_snd_packet(&sendpacket)!=SUCC)//使用MDM数据包进行测试
			break;
		/*MDM连接*/
		Local2Global(nMdmType);
		if(nMdmType == ASYN)
		{
			if(__LinkUp_ASYN()!=SUCC)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次ASYN拨号失败", __LINE__, i);
				continue;
			}
		}
		else
		{
			if(__LinkUp_SDLC()!=SUCC)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次SDLC拨号失败", __LINE__, i);
				continue;
			}
		}
		Global2Local(nMdmType);
		
		//打开蓝牙串口
		Local2Global(BT);
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		Global2Local(BT);
		
		/*MDM无需建立传输层*/
		
		//清缓冲区
		mdm_clrportbuf_all();
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		
		/*MDM发送数据*/
		Local2Global(nMdmType);
		if((slen=mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次MDM发送失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
			goto ERR;
		}
		
		//蓝牙发送数据
		Local2Global(BT);
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x05, sendpacket.header, sendpacket.len, cmdsendBuf);
		cmdlen = sendpacket.len+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		
		/*MDM接收数据*/
		Local2Global(nMdmType);
		memset(recvBuf, 0, sizeof(recvBuf));
		if((rlen=mdm_rev(recvBuf, sendpacket.len, 20))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次MDM接收失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			goto ERR;
		}
		/*MDM校验收发数据*/
		if(MemCmp(sendpacket.header, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次MDM数据校验失败", __LINE__, i);
			goto ERR;
		}
		
		//蓝牙接收数据
		Local2Global(BT);
		memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, cmdlen);
			goto ERR;
		}
		bt_cmd_uppacket(cmdrecvBuf, sendpacket.len, recvBuf);
		if(memcmp(sendBuf, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次蓝牙数据校验失败", __LINE__, i);
			goto ERR;
		}
		memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		
		/*MDM断开连接*/
		Local2Global(nMdmType);
		cls_printf("MODEM挂断中(第%d次)...", i);
		if(MDM_NetDown(nMdmType)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次MODEM挂断失败", __LINE__, i);
			goto ERR;
		}
		
		//关闭蓝牙串口
		Local2Global(BT);
		//断开蓝牙连接
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败", __LINE__, i);
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			continue;
		}			
		succ++;
		continue;
	ERR:
		Global2Local(BT);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		Global2Local(nMdmType);
		MDM_NetDown(nMdmType);
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s测试完成,已进行%d次,成功:%d次",TESTITEM,i-1,succ);
	return;
}

int bt_comm(Packet *sendpacket)
{
	int ret = -1;
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:打开蓝牙串口失败", __LINE__);
		return ret;
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:配对失败(%d)", __LINE__,ret);
		return ret;
	}
	if((ret=bt_comm_once(sendpacket->len,1))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:通讯失败(%d)", __LINE__, ret);
		NDK_BTDisconnect();
		NDK_PortClose(PORT_NUM_BT);
		return ret;
	}
	//断开蓝牙连接
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:断开蓝牙连接失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_BT);
		return ret;
	}	
	//关闭蓝牙串口
	if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:关闭蓝牙串口失败(%d)", __LINE__, ret);
		return ret;
	}			
	return SUCC;
}
//BT与MDM的浅交叉
void bt_mdm_cross(int nMdmType)
{
	/*private & local definition*/
	int i = 0, cnt = 0, succ = 0, succ2 = 0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	float dial = 0.0, hang = 0.0;	
	
	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "BT参数未配置");
		return;
	}
	if(nConfInit[1] == 0)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "MDM参数未配置");	
		return;
	}
	/*process body*/
	//初始化发包供BT与MDM共用
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();
	
	if(sendpacket.forever==FALSE)  //因下面循环两次调用到update_snd_packet造成实际次数减半
	{
		cnt = sendpacket.lifecycle;
		sendpacket.lifecycle = 2*cnt;
	}
	while (1)
	{		
		if(cls_show_msg1(2, "正在进行第%d次%s测试,按ESC退出", ++i, TESTITEM)==ESC)
			break;
		
		//蓝牙通讯
		Local2Global(BT);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		if(bt_comm(&sendpacket)==SUCC)
		{
			succ++;
		}
		Global2Local(BT);
		
		//MDM 通讯
		Local2Global(nMdmType);
		if(update_snd_packet(&sendpacket)!=SUCC)//使用MDM数据包进行测试
			break;
		if(mdm_dial_comm(MDMDialStr, &sendpacket, FALSE, &dial, &hang)==SUCC)
		{
			succ2++;
		}
		Global2Local(nMdmType);
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"浅交叉测试%d次,BT成功:%d次,MDM成功:%d次",i-1,succ,succ2);
	return;
}

void systest59(void)
{
	/*private & local definition*/
	int ret = 0, nMdmType = 0; 
	
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
		if(conf_conn_MDM()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,ETH配置失败,请检查配置文件",TESTITEM);
			return;
		}
		nConfInit[1]=1;
		nMdmType = getlinktype();
		Global2Local(nMdmType);
		g_CycleTime = 10;
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);	
		//执行交叉测试
		BT_MDM(nMdmType);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.深交叉压力\n"
							"2.浅交叉压力\n"
							"0.蓝牙配置\n"
							"9.MDM配置");
		
		switch(ret)
		{
		case '1':
			BT_MDM(nMdmType);
			break;
		case '2':
			bt_mdm_cross(nMdmType);
			break;	
		case '0':
			//cls_show_msg("任意键进行BT参数配置...");
			switch(conf_conn_BT())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份BT参数
				Global2Local(BT);
				nConfInit[0]=1;
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
			case ESC://用户放弃
			default:
				continue;//return;
				break;
			}
			break;
		case '9':	
			//cls_show_msg("任意键进行MDM参数配置...");
			switch(conf_conn_MDM())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份MDM参数
				nConfInit[1]=1;
				nMdmType = getlinktype();
				Global2Local(nMdmType);
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
			case ESC://用户放弃
			default:
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


