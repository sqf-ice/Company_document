/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WLAN/ETH交叉
* file name		: systest44.c
* Author 			: chensj
* version			: V1.0
* DATE			: 2013/08/07
* directory 		: 
* description		: WLAN/ETH深度交叉
* related document	: NL-GP730序员参考手册.doc
*				  
************************************************************************
* log			: 
* Revision 1.0  2013/08/07 chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"WLAN/ETH深度交叉"

/*------------global variables definition-----------------------*/
static int nConfInit[2] = {0};	 /*是否配置过了参数:0未配置 1已配置*/
static char destipforwifi[16] = {0};
static char destipforeth[16] = {0};

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
*			  		  chensj    	20130807 		created
*****************************************************************/
void WLAN_ETH(void)
{
	/*private & local definition*/
	int  i = 0,slen = 0, slen2 = 0, rlen = 0, rlen2 = 0, timeout = SO_TIMEO,succ=0;
	uint h_tcp = -1,h_tcp2 = -1;
	char buf[PACKMAXLEN] = {0},rbuf[PACKMAXLEN] = {0},rbuf2[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1(g_keeptime, "Wifi参数未配置");
		return;
	}
	
	if(nConfInit[1] == 0)
	{
		cls_show_msg1(g_keeptime, "ETH参数未配置");	
		return;
	}

	/*process body*/
	//setlinktype(WLAN);
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	while (1)
	{		
		if(cls_show_msg1(3, "正在进行第%d次%s通讯",++i ,TESTITEM)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		
		/*ETH连接*/
		NDK_NetAddRouterTable(COMM_TYPE_ETH,destipforeth);
		Local2Global(ETH);
		if(__LinkUp_ETH()!=SUCC || __NetUp_ETH()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH连接失败",__LINE__,i,TESTITEM);
			goto ERR;
		}
		Global2Local(ETH);
		
		/* wifi连接*/
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,destipforwifi);
		Local2Global(WLAN);
		if(__LinkUp_WLAN()!=SUCC||__NetUp_WLAN()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi连接失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}
		Global2Local(WLAN);
	
		/*ETH建立传输层*/
		NDK_NetAddRouterTable(COMM_TYPE_ETH,destipforeth);
		Local2Global(ETH);
		if(__TransUp(&h_tcp2)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH建立传输层失败",__LINE__,i,TESTITEM);
			goto ERR3;
		}
		Global2Local(ETH);
		
		/*wifi建立传输层*/
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,destipforwifi);
		Local2Global(WLAN);
		if(__TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi建立传输层失败",__LINE__,i,TESTITEM);
			goto ERR4;
		}
		Global2Local(WLAN);

		/*ETH发送数据*/
		NDK_NetAddRouterTable(COMM_TYPE_ETH,destipforeth);
		Local2Global(ETH);
		if((slen2=sock_send(h_tcp2, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH发送数据失败(实际:%d,预期:%d)",__LINE__,i,TESTITEM, slen2, sendpacket.len);
			goto ERR4;
		}
		
		/*wifi发送数据*/
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,destipforwifi);
		Local2Global(WLAN);
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi发送数据失败(实际:%d,预期:%d)",__LINE__,i,TESTITEM, slen, sendpacket.len);
			goto ERR4;
		}

		/*ETH接收数据*/
		NDK_NetAddRouterTable(COMM_TYPE_ETH,destipforeth);
		Local2Global(ETH);
		memset(rbuf2, 0, sizeof(rbuf2));
		if ((rlen2=sock_recv(h_tcp2, rbuf2, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH接收数据失败(实际:%d,预期:%d)",__LINE__,i,TESTITEM, rlen2, sendpacket.len);
			goto ERR4;
		}

		/*wifi接收数据*/
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,destipforwifi);
		Local2Global(WLAN);
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi数据失败(实际:%d,预期:%d)",__LINE__,i,TESTITEM, rlen, sendpacket.len);
			goto ERR4;		
		}

		/*校验收发数据*/
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi校验数据失败",__LINE__,i,TESTITEM);
			goto ERR4;
		}

		if(MemCmp(sendpacket.header, rbuf2, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wlm校验数据失败",__LINE__,i,TESTITEM);
			goto ERR4;
		}

		/*wifi断开传输层*/
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,destipforwifi);
		Local2Global(WLAN);
		g_socktype = netsetting.socktype;
		if (__TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi断开传输层失败",__LINE__,i,TESTITEM);
			goto ERR4;
		}

		/*ETH断开传输层*/
		NDK_NetAddRouterTable(COMM_TYPE_ETH,destipforeth);
		Local2Global(ETH);
		g_socktype = netsetting.socktype;
		if (__TransDown(h_tcp2)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH断开传输层失败",__LINE__,i,TESTITEM);
			goto ERR3;
		}

		/*wifi断开连接*/
		Local2Global(WLAN);
		if(__NetDown_WLAN()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试wifi断开连接失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}

		/*ETH断开连接*/
		Local2Global(ETH);
		if(__NetDown_ETH()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试ETH断开连接失败",__LINE__,i,TESTITEM);
			goto ERR;
		}
		succ++;
		continue;
		
		ERR4:
			Local2Global(WLAN);
			g_socktype = netsetting.socktype;
			__TransDown(h_tcp);
		ERR3:
			Local2Global(ETH);
			g_socktype = netsetting.socktype;
			__TransDown(h_tcp2);
		ERR2:
			Local2Global(WLAN);
			__NetDown_WLAN();
		ERR:
			__NetDown_ETH();
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试完成,已进行%d次,成功:%d次",TESTITEM,i-1,succ);
	return;

}

void systest44(void)
{
	/*private & local definition*/
	int keyin = 0, tmpflag = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=0)
		SetWifiStopflag(0);//确保本用例在维持wifi连接的情况下测试
		
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_WLAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WIFI配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		//备份wlan参数
		Global2Local(WLAN);
		strcpy(destipforwifi,netsetting.SvrIP.Str);
		//建链前先设置路由表
		NDK_NetAddRouterTable(COMM_TYPE_WIFI,netsetting.SvrIP.Str);//该函数为WIFI创建路由表
		nConfInit[0]=1;
		
		if(conf_conn_ETH(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,ETH配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		//备份eth参数
		Global2Local(ETH);
		strcpy(destipforeth,netsetting.SvrIP.Str);
		NDK_NetAddRouterTable(COMM_TYPE_ETH,netsetting.SvrIP.Str);//该函数为以太网创建路由表
		nConfInit[1]=1;
		
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		WLAN_ETH();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
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
			WLAN_ETH();
			break;
		case '0':
			//cls_show_msg("任意键进行WLAN参数配置...");
			switch(conf_conn_WLAN())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份wlan参数
				Global2Local(WLAN);
				strcpy(destipforwifi,netsetting.SvrIP.Str);
				//建链前先设置路由表
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
			case ESC://用户放弃
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
				Global2Local(ETH);
				strcpy(destipforeth,netsetting.SvrIP.Str);
				if((cls_show_msg("ETH服务器IP:%s,是否需要添加到路由表?[ENTER]是，[其他]否", netsetting.SvrIP.Str)) == ENTER ) 	
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
			case ESC://用户放弃
			default:
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

