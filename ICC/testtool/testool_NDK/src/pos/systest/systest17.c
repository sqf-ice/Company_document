/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest17.c
* Author 			: chensl
* version			: 
* DATE			: 20130124
* directory 	: 
* description	: 磁卡/LAN交叉测试
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
#define	TESTITEM		"磁卡/LAN交叉"

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
*			  		chensl		   20130124	 		created
*****************************************************************/
static void card_LAN(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0;
	uint h_tcp = -1;
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);

	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}
	systest_MagCard();
	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ_count)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;

		TransDown(h_tcp);//UP前的保护
		//UP
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
			continue;
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
			continue;		
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
			continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
			continue;		
		//DOWN
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:TransDown失败", __LINE__);
			continue;
		}
		if(systest_MagCard()!=STRIPE)
			continue;
		
		succ_count++;
	}
	
	NetDown();
	systest_MagCard();
			
	cls_show_msg("通讯次数:%d\n成功次数:%d", i, succ_count);
	return;
}

void systest17(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0, tmpflag = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=0)
		SetWifiStopflag(0);//确保本用例在维持wifi连接的情况下测试
	
	while (1)
	{
		keyin = cls_show_msg("1.交叉测试\n"
							"0.LAN配置");
		switch(keyin)
		{
		case '1':
			card_LAN();
			break;
		case '0':
			switch(conf_conn_LAN())
			{
			case SUCC:
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

