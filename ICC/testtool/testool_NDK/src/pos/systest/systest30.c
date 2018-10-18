/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest30.c
* Author 			: chensl
* version			: 
* DATE			: 20130124
* directory 		: 
* description		: 无线/LAN交叉
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
#define	TESTITEM		"无线/LAN交叉"

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
* history 		 		: author			date			remarks
*			  		chensl		   20130124	 		created
*****************************************************************/
static void LAN_wlm(LinkSetting *lnk, NetworkSetting *net)
{
	/*private & local definition*/
	int ret = 0, timeout = SO_TIMEO, slen = 0, rlen = 0, total[2] = {0, 0}, succ[2] = {0, 0}, i = 0, cnt = 0;
	uint h_tcp = 0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0}, *msg[2] = {"LAN","WLM"};
	LinkType linktype1;
	
	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1(g_keeptime, "LAN参数未配置");
		return;
	}
	if(nConfInit[1] == 0)
	{
		cls_show_msg1(g_keeptime, "Wlm参数未配置");	
		return;
	}
	
	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);

	if(sendpacket.forever==FALSE)  //因下面循环两次调用到update_snd_packet造成实际次数减半
	{
		cnt = sendpacket.lifecycle;
		sendpacket.lifecycle = 2*cnt;
	}
	//LAN/WLM交叉测试(测试前需要将链路及网络参数导入全局设置)
	while (1)
	{
		TransDown(h_tcp);
		NetDown();//保护
		
		if((i=(total[0]+total[1])%2)) //if(i=rand()%2) linwl20131009未提高交叉效果不再使用随机的方式
			sleep(10);
		memcpy(&linksetting, &lnk[i], sizeof(LinkSetting));
		memcpy(&netsetting, &net[i], sizeof(NetworkSetting));
		//在每次进行通讯前需要进行路由表设置,才能保证TCP层的通道是无线还是WiFi或者以太网 sull add 20180208
		linktype1 = getlinktype();
		switch(linktype1)
		{
			case ETH:
				if((ret = NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str)) != NDK_OK)	
				{
					cls_show_msg1(g_keeptime, "line %d:第%d次ETH创建路由表失败", __LINE__, total[i]);
					continue;
				}
				break;
			case WLAN:
				if((ret = NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str)) != NDK_OK)	
				{
					cls_show_msg1(g_keeptime, "line %d:第%d次创建路由表失败", __LINE__, total[i]);
					continue;
				}
				break;
			case GPRS:
			case CDMA:
			case ASYN:
			case TD:
				if((ret = NDK_NetAddRouterTable(COMM_TYPE_PPP,netsetting.SvrIP.Str)) != NDK_OK)	
				{
					cls_show_msg1(g_keeptime, "line %d:第%d次创建路由表失败", __LINE__, total[i]);
					continue;
				}
				break;
			default:
				continue;
				break;
		}		
		
		if(ESC==cls_show_msg1(2, "正在进行第%d次%s通讯(已成功%d次),按ESC退出", total[i]+1, msg[i], succ[i]))
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		++total[i];
		
		//NetUp
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s NetUp失败", __LINE__, total[i], msg[i]);
			continue;
		}
		
		//TransUp
		if((ret=TransUp(&h_tcp))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s TransUp失败(%d)", __LINE__, total[i], msg[i], ret);
			continue;
		}
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s send失败(实际%d, 预期%d)", __LINE__, total[i], msg[i], slen, sendpacket.len);
			continue;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len||MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s recv失败(实际%d, 预期%d)", __LINE__, total[i], msg[i], slen, sendpacket.len);
			continue;
		}

		//TransDown
		if((ret=TransDown(h_tcp))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s TransDown失败(%d)", __LINE__, total[i], msg[i], ret);
			continue;
		}
		
		//NetDown
		if(NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s NetDown失败", __LINE__, total[i], msg[i]);
			continue;
		}		
		++succ[i];
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"LAN已执行次数为%d,成功%d次", total[0], succ[0]);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WLM已执行次数为%d,成功%d次", total[1], succ[1]);
	return;
}

void systest30(void)
{
	/*private & local definition*/
	int ret = 0;
	LinkSetting lnks[2];
	NetworkSetting nets[2];
	
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
		//备份lan参数
		memcpy(&lnks[0], &linksetting, sizeof(LinkSetting));
		memcpy(&nets[0], &netsetting, sizeof(NetworkSetting));
		if(getlinktype()==ETH)
			NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);//该函数为以太网创建路由表
		else
			NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
		nConfInit[0]=1;
		
		if(conf_conn_WLM(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试WLM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//备份wlm参数
		memcpy(&lnks[1], &linksetting, sizeof(LinkSetting));
		memcpy(&nets[1], &netsetting, sizeof(NetworkSetting));
		NDK_NetAddRouterTable(COMM_TYPE_PPP,netsetting.SvrIP.Str);//该函数为无线创建路由表
		nConfInit[1]=1;

		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);	
		//执行交叉测试
		LAN_wlm(lnks, nets);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.LAN配置\n"
							"9.WLM配置");
		
		switch(ret)
		{
		case '1':
			LAN_wlm(lnks, nets);
			break;
		case '0':
			switch(conf_conn_LAN())
			{
			case SUCC:
				cls_show_msg1(2, "LAN配置成功!");
				//备份lan参数
				memcpy(&lnks[0], &linksetting, sizeof(LinkSetting));
				memcpy(&nets[0], &netsetting, sizeof(NetworkSetting));
				if(getlinktype()==ETH)
				{
					if((cls_show_msg("ETH服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER )	
					{
						NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);//该函数为以太网创建路由表
						nConfInit[0]=1;
					}
					else
					{
						cls_show_msg("请重新配置，选择正确的路由表!!!");
						nConfInit[0]=0;
					}	
				}
				else
				{
					if((cls_show_msg("WIFI服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER )	
					{
						NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
						nConfInit[0]=1;
					}
					else
					{
						cls_show_msg("请重新配置，选择正确的路由表!!!");
						nConfInit[0]=0;
					}	
				}
				break;
			case FAIL:
				cls_show_msg("line %d:LAN网络未连通!!!\n任意键退出配置...", __LINE__);
				//return;
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				//return;
				break;
			}
			break;
		case '9':
			switch(conf_conn_WLM(TRUE))
			{
			case SUCC:
				cls_show_msg1(2, "WLM配置成功!");
				//备份wlm参数
				memcpy(&lnks[1], &linksetting, sizeof(LinkSetting));
				memcpy(&nets[1], &netsetting, sizeof(NetworkSetting));
				if((cls_show_msg("wlm路由表:%s,是否为需要添加的路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER ) 	
				{
					NDK_NetAddRouterTable(COMM_TYPE_PPP,netsetting.SvrIP.Str);//该函数为无线创建路由表
					nConfInit[1]=1;
				}
				else
				{
					cls_show_msg("请重新配置，选择正确的路由表!!!");
					nConfInit[1]=0;
				}	
				break;
			case FAIL:
				cls_show_msg("line %d:WLM网络未连通!!!\n任意键退出配置...", __LINE__);
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

