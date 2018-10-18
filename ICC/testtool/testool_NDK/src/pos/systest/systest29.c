/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest29.c
* Author 			: chenjs
* version			: 
* DATE			: 20130227
* directory 	: 
* description	: MDM/LAN交叉测试
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
#define	TESTITEM		"MDM/LAN交叉"

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
*			  		chenjs		   20130227	 		created
*****************************************************************/

static int LAN_dial_comm(Packet *sendpacket)
{
	/*private & local definition*/
	int ret = 0, slen = 0, rlen = 0, timeout = SO_TIMEO;
	uint h_tcp = -1;
	char rbuf[PACKMAXLEN] = {0};
	
	/*process body*/
	if(NetUp()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetUp失败", __LINE__);
		return FAIL;
	}
	if((ret=TransUp(&h_tcp))!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败(%d)", __LINE__, ret);
		goto ERR;
	}
	//LAN发送数据
	if((slen=sock_send(h_tcp, sendpacket->header, sendpacket->len, timeout))!=sendpacket->len)
	{
		cls_show_msg1(g_keeptime, "line %d:LAN发送数据失败(实际%d, 预期%d)", __LINE__, slen, sendpacket->len);
		goto ERR;
	}
	//LAN接收数据
	if((rlen=sock_recv(h_tcp, rbuf, sendpacket->len, timeout))!=sendpacket->len||(ret=MemCmp(sendpacket->header, rbuf, sendpacket->len)))
	{
		cls_show_msg1(g_keeptime, "line %d:LAN接收数据失败(实际%d, 预期%d)(%d)", __LINE__, rlen, sendpacket->len, ret);
		goto ERR;
	}
	if((ret=TransDown(h_tcp))!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransDown挂断失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if(NetDown()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetDown失败", __LINE__);
		return FAIL;
	}
	return SUCC;
ERR:
	TransDown(h_tcp);
	NetDown();	
	return FAIL;
}

static void mdm_LAN(LinkType *lnktype)
{
	/*private & local definition*/
	int i = 0, succ1 = 0, succ2 = 0,cnt = 0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	float dial = 0.0, hang = 0.0;
	
	/*process body*/
	//初始化发包供MDM与LAN共用
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);

	if(sendpacket.forever==FALSE)
	{
		cnt = sendpacket.lifecycle;
		sendpacket.lifecycle = 2*cnt;  //此处循环次数变两倍是因为循环里面两次调用update_snd_packet
	}
	
	mdm_clrportbuf_all();
	while(1)
	{
		if(ESC==cls_show_msg1(2, "正在进行第%d次%s测试,按ESC退出", i+1, TESTITEM))
			break;
		
		//以MDM类型的包格式来作为两种通讯方式下的发包
		setlinktype(lnktype[1]);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		//有线MDM通讯
		if(mdm_dial_comm(MDMDialStr, &sendpacket, FALSE, &dial, &hang)==SUCC)
			succ1++;

		//LAN通讯
		setlinktype(lnktype[0]);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		if(LAN_dial_comm(&sendpacket)==SUCC)
			succ2++;
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"交叉测试%d次.MDM成功%d次,LAN成功%d次.", i, succ1, succ2);
	return;
}

void systest29(void)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type[2] = {NONE, NONE};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_LAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试LAN配置失败,请检查配置文件",TESTITEM);
			return;
		}
		type[0] = getlinktype();

		if(conf_conn_MDM()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试MDM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		type[1] = getlinktype();

		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		mdm_LAN(type);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.LAN配置\n"
							"9.mdm配置");
		
		switch(ret)
		{
		case '1':
			mdm_LAN(type);
			break;
		case '0':
			switch(conf_conn_LAN())
			{
			case SUCC:
				type[0] = getlinktype();
				cls_show_msg1(2, "网络配置成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);	
				//return;
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				//return;
				break;
			}
			break;
		case '9':
			switch(conf_conn_MDM())
			{
			case SUCC:
				type[1] = getlinktype();
				cls_show_msg1(2, "配置modem成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:配置modem失败!!!\n任意键退出配置...", __LINE__);	
				break;
			case NDK_ERR_QUIT://用户放弃
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

