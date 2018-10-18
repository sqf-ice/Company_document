/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WLAN/MDM深度交叉
* file name		: systest46.c
* Author 			: chensj
* version			: V1.0
* DATE			: 2013/08/08
* directory 		: 
* description		: WLAN/MDM深度交叉
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
#define	TESTITEM	"WLAN/MDM深度交叉"

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
*			  		  chensj    	20130808 		created
*****************************************************************/
void WLAN_MDM(int nMdmType)
{
	/*private & local definition*/
	int  i = 0, slen = 0, slen2 = 0, rlen = 0, rlen2 = 0, timeout = SO_TIMEO,succ=0;
	char buf[PACKMAXLEN] = {0},rbuf[PACKMAXLEN] = {0},rbuf2[PACKMAXLEN] = {0};
	Packet sendpacket;
	uint h_tcp = -1;
		
	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1(g_keeptime, "Wifi参数未配置");
		return;
	}
	
	if(nConfInit[1] == 0)
	{
		cls_show_msg1(g_keeptime, "MDM参数未配置");	
		return;
	}

	/*process body*/
	setlinktype(nMdmType);
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();
	while (1)
	{		
		if(cls_show_msg1(3, "正在进行第%d次%s通讯",++i ,TESTITEM)==ESC)
			break;
		
		setlinktype(nMdmType);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		
		/*MDM连接*/
		Local2Global(nMdmType);
		if(nMdmType == ASYN)
		{
			if(__LinkUp_ASYN()!=SUCC)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
				continue;
			}
		}
		else
		{
			if(__LinkUp_SDLC()!=SUCC)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
				continue;
			}
		}
		Global2Local(nMdmType);
		
		/*wifi连接*/
		Local2Global(WLAN);
		if(__LinkUp_WLAN()!=SUCC||__NetUp_WLAN()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR;
		}
		Global2Local(WLAN);

		/*MDM无需建立传输层*/
		
		/*wifi建立传输层*/
		Local2Global(WLAN);
		if(__TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}
		Global2Local(WLAN);

		/*MDM发送数据*/
		Local2Global(nMdmType);
		if((slen2=mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}
		
		/*wifi发送数据*/
		Local2Global(WLAN);
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}

		/*MDM接收数据*/
		Local2Global(nMdmType);
		memset(rbuf2, 0, sizeof(rbuf2));
		if ((rlen2=mdm_rev(rbuf2, sendpacket.len, 30))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}
		if(MemCmp(sendpacket.header, rbuf2, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}
		
		/*wifi接收数据*/
		Local2Global(WLAN);
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;		
		}
		/*校验收发数据*/
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}

		/*wifi断开传输层*/
		Local2Global(WLAN);
		g_socktype = netsetting.socktype;
		if (__TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR2;
		}

		/*MDM没有传输层,无需断开*/

		/*wifi断开连接*/
		Local2Global(WLAN);
		if(__NetDown_WLAN()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR;
		}

		/*MDM断开连接*/
		if(MDM_NetDown(nMdmType)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败",__LINE__,i,TESTITEM);
			goto ERR;
		}
		
		succ++;
		continue;
		
		ERR2:
			Local2Global(WLAN);
			g_socktype = netsetting.socktype;
			__TransDown(h_tcp);
		ERR:
			Local2Global(WLAN);
			__NetDown_WLAN();
			MDM_NetDown(nMdmType);
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试完成,已进行%d次,成功:%d次",TESTITEM,i-1,succ);
	return;

}

void systest46(void)
{
	/*private & local definition*/
	int keyin = 0,nMdmType = 0, tmpflag = 0; /*记录同步还是异步通讯*/ 
		
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
		nConfInit[0]=1;
		
		if(conf_conn_MDM()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,ETH配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		//备份mdm参数
		nConfInit[1]=1;
		nMdmType = getlinktype();
		Global2Local(nMdmType);

		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);		
		//执行交叉测试
		WLAN_MDM(nMdmType);		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
		return;
	}
	
	while (1)
	{	
		keyin = cls_show_msg("1.交叉压力\n"
							"0.WIFI配置\n"
							"9.MDM配置\n");
		switch(keyin)
		{
		case '1':
			WLAN_MDM(nMdmType);
			break;
		case '0':
			//cls_show_msg("任意键进行WLAN参数配置...");
			switch(conf_conn_WLAN())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				//备份wlan参数
				Global2Local(WLAN);
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
				//备份mdm参数
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
