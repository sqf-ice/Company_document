/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			:  ETH/WLAN交叉
* file name			: 22.c
* Author 			: chensj
* version			: 
* DATE				: 20130521
* directory 		: 
* description		:  ETH/WLAN交叉测试
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"ETH/WLAN交叉"
/*------------global variables definition-----------------------*/
//static char destipforwifi[16] = {0};
//static char destipforeth[16] = {0};

/*----------global variables declaration("extern")--------------*/
static int nConfInit[2] = {0};	 /*是否配置过了参数:0未配置 1已配置*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		      chensj	        20130524	 	created
*****************************************************************/
void wlan_ETH(LinkSetting *lnk, NetworkSetting *net)
{
	/*private & local definition*/
	int total[2] = {0, 0}, succ[2] = {0, 0}, i = 0, cnt = 0;
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0}, *type = NULL;
	Packet sendpacket;
	
	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1(g_keeptime, "Wifi参数未配置");
		return;
	}
	if(nConfInit[1] == 0)
	{
		cls_show_msg1(g_keeptime, "Eth参数未配置");	
		return;
	}

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);

	// ETH/WLAN交叉测试(测试前需要将链路及网络参数导入全局设置)
	if(sendpacket.forever==FALSE)  //因下面循环两次调用到update_snd_packet造成实际次数减半
	{
		cnt = sendpacket.lifecycle;
		sendpacket.lifecycle = 2*cnt;
	}
	while (1)
	{
		TransDown(h_tcp);
		NetDown();//保护	
		
		i = (total[0]+total[1])%2;//i = rand()%2; linwl20131009未提高交叉效果不再使用随机的方式
		memcpy(&linksetting, &lnk[i], sizeof(LinkSetting));
		memcpy(&netsetting, &net[i], sizeof(NetworkSetting));
		type = getlinktype()==ETH?"ETH":"WLAN";
		//通讯前增加路由表设置为了确保每次通讯TCP使用哪种连接方式的准确性20180208
		if(getlinktype()==ETH)
			NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);
		else
			NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);
		if(cls_show_msg1(2, "正在进行第%d次%s通讯(已成功%d次)", total[i]+1, type, succ[i])==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		++total[i];
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s NetUp失败", __LINE__, total[i], type);
			continue;
		}
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s TransUp失败", __LINE__, total[i], type);
			continue;
		}
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s发送失败(实际%d, 预期%d)", __LINE__, total[i], type, slen, sendpacket.len);
			continue;
		}
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s接收失败(实际%d, 预期%d)", __LINE__, total[i], type, rlen, sendpacket.len);
			continue;
		}
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s校验失败", __LINE__, total[i], type);
			continue;
		}
		if(TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s TransDown失败", __LINE__, total[i], type);
			continue;
		}
		if(NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s NetDown失败", __LINE__, total[i], type);
			continue;
		}
		++succ[i];
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WLAN通讯(%s)次数:%d 成功次数:%d", lnk[0].WLANEssid, total[0], succ[0]);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"ETH通讯次数:%d 成功次数:%d", total[1], succ[1]);
	return;
}

void systest22(void)
{
	/*private & local definition*/
	LinkSetting links[2];
	NetworkSetting nets[2];
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_WLAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WIFI配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//备份wlan参数
		memcpy(&links[0], &linksetting, sizeof(LinkSetting));
		memcpy(&nets[0], &netsetting, sizeof(NetworkSetting));
		//strcpy(destipforwifi,netsetting.SvrIP.Str);
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
		nConfInit[0]=1;
		
		if(conf_conn_ETH(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,ETH配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//备份eth参数
		memcpy(&links[1], &linksetting, sizeof(LinkSetting));
		memcpy(&nets[1], &netsetting, sizeof(NetworkSetting));
		//strcpy(destipforeth,netsetting.SvrIP.Str);
		NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);//该函数为以太网创建路由表
		nConfInit[1]=1;

		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		wlan_ETH(links, nets);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}

	while (1)
	{	
		keyin = cls_show_msg("1.交叉压力\n"
							"0.WIFI配置\n"
							"9.ETH配置\n");
		switch(keyin)
		{
		case '1':
			wlan_ETH(links, nets);
			break;
		case '0':
			//cls_show_msg("任意键进行WLAN参数配置...");
			switch(conf_conn_WLAN())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份wlan参数
				memcpy(&links[0], &linksetting, sizeof(LinkSetting));
				memcpy(&nets[0], &netsetting, sizeof(NetworkSetting));
				//strcpy(destipforwifi,netsetting.SvrIP.Str);
				if((cls_show_msg("wifi服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER ) 	
				{
					NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
					nConfInit[0]=1;
				}
				else
				{
					cls_show_msg("请重新配置，选择正确的路由表!!!");
					nConfInit[0]=0;
				}	
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
			//cls_show_msg("任意键进行ETH参数配置...");
			switch(conf_conn_ETH(TRUE))
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份eth参数
				memcpy(&links[1], &linksetting, sizeof(LinkSetting));
				memcpy(&nets[1], &netsetting, sizeof(NetworkSetting));
				//strcpy(destipforeth,netsetting.SvrIP.Str);
				if((cls_show_msg("eth服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER ) 					
				{
					NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);//该函数为以太网创建路由表
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
