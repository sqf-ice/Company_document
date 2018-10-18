/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 无线综合测试
* file name		: systest4.c
* Author 			:  chensj
* version			: V1.0
* DATE			:20130204
* directory 		: 
* description		: 无线综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130204 chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define 	UCID  4
#define	TESTITEM	"无线性能,压力"

#define	DEFAULT_CNT_STR	"100"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(200)

//#define	wireless_ability	WLM_ability
#define PACKETLIFE 30//单向接收性能压力次数

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
*			  		  chensj		   20130204  		created
*							
*****************************************************************/
#if CPU5810X_ENABLE
Network_t select_nettype(void)//选择网络模式类型20180724 liny added
{
	/*private & local definition*/
	int nKeyin = 0, nettype=-1;
	char pInfo[64] = {0};
	Network_t type[] = {second_G,third_G,fourth_G};
	
	/*process body*/
	while (1)
	{
		if(auto_flag==1)
		{
			if(GetParaInfo("network",pInfo)!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%s)", __LINE__, pInfo);
				return fourth_G;//配置文件设置失败时默认配置4G网络
			}
			if(trim_string2int(pInfo,&nettype)!=SUCC)
			{
				 cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%d)", __LINE__, nettype);
				 return fourth_G;
			}
			switch(nettype)
			{
				case 1:
				case 2:
				case 3:
					return type[nettype-1];
					break;
				case ESC:
				default:
					cls_show_msg1(2, "将设置为4G网络(默认)...");
					return fourth_G;
					break;
			}
		}
		else
		{
			nKeyin = cls_show_msg("设置网段\n"
								"1.2G网络\n"
								"2.3G网络(移动无3G)\n"
								"3.4G网络\n"
								);
		
			switch(nKeyin)
			{
				case '1':
				case '2':
				case '3':
					return type[nKeyin-'1'];
					break;
				case ESC:
					cls_show_msg1(2, "将设置为4G(默认)...");
					return fourth_G;
					break;
				default:
					 continue;
					 break;
			}
		}
	}
}
 int conf_conn_setseg(Network_t type)//根据网络模式发送AT指令20180727 liny added
{	
	int nRet=0;
	char *p = NULL,*q=NULL,*m=NULL,*style=NULL,*style1=NULL,*style2=NULL;
	char sRecvStr[100]={0};
	ST_ATCMD_PACK atm_cmd;
	EM_WLM_STATUS  pemStatus = 0;
	atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
	atm_cmd.pcAddParam=NULL;
	time_t oldtime=0;
	
	oldtime=time(NULL);
	switch(type)
	{
		case second_G:
			//发送AT指令切换2G网络
			//电信AT指令不一样，需要判断是否是电信
			atm_cmd.pcAtCmd="+CIMI";
			memset(sRecvStr,0,sizeof(sRecvStr));
			if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
			{ 
				 cls_show_msg1(g_keeptime,"line %d:获取运营商信息失败(%d)", __LINE__, nRet);
				 return FAIL;
			}
			style=strstr(sRecvStr,"46003");
			style1=strstr(sRecvStr,"46005");
			style2=strstr(sRecvStr,"46011");
			memset(sRecvStr,0,sizeof(sRecvStr));
			if(style!=NULL||style1!=NULL||style2!=NULL)//是否为电信
			{
				atm_cmd.pcAtCmd="+qcfg=\"nwscanmode\",6";//电信2G AT指令
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
				{ 
					 cls_show_msg1(g_keeptime,"line %d:切换3G网络失败(%d)", __LINE__, nRet);
					 return FAIL;
				}
			}
			else
			{
				atm_cmd.pcAtCmd="+qcfg=\"nwscanmode\",1";	
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
				{ 
					 cls_show_msg1(g_keeptime,"line %d:切换2G网络失败(%d)", __LINE__, nRet);
					 return FAIL;
				}	
			}
			atm_cmd.pcAtCmd="+COPS?";			
			memset(sRecvStr,0,sizeof(sRecvStr));
			while(1)
			{
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,1000,&pemStatus))!=NDK_OK)
				{ 
					cls_show_msg1(g_keeptime,"line %d:切换2G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				 }
				 p=strstr(sRecvStr,"\",0");//移动联通2G标志
				 q=strstr(sRecvStr,"\",100");//电信2G标志
				 if(p!=NULL||q!=NULL)
				 {
					cls_show_msg1(g_keeptime,"2G网络设置成功");
					break;
				}
				if(time(NULL)-oldtime>10)
				{
					cls_show_msg1(g_keeptime,"line %d:切换2G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				 }	
			}
			break;
		case third_G:
			//移动没有3G，电信和联通AT指令不一样，需要判断是电信还是联通
			atm_cmd.pcAtCmd="+CIMI";
			memset(sRecvStr,0,sizeof(sRecvStr));
			if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
			{ 
				 cls_show_msg1(g_keeptime,"line %d:获取运营商信息失败(%d)", __LINE__, nRet);
				 return FAIL;
			}
			style=strstr(sRecvStr,"46003");
			style1=strstr(sRecvStr,"46005");
			style2=strstr(sRecvStr,"46011");
			memset(sRecvStr,0,sizeof(sRecvStr));
			if(style!=NULL||style1!=NULL||style2!=NULL)//是否为电信
			{
				atm_cmd.pcAtCmd="+qcfg=\"nwscanmode\",7";//电信3G AT指令
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
				{ 
					 cls_show_msg1(g_keeptime,"line %d:切换3G网络失败(%d)", __LINE__, nRet);
					 return FAIL;
				}
			}
			else
			{
				atm_cmd.pcAtCmd="+qcfg=\"nwscanmode\",2";//联通3G AT指令
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
				{ 
					 cls_show_msg1(g_keeptime,"line %d:切换3G网络失败(%d)", __LINE__, nRet);
					 return FAIL;
				}
			}
			atm_cmd.pcAtCmd="+COPS?";			
			memset(sRecvStr,0,sizeof(sRecvStr));
			while(1)
			{
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,1000,&pemStatus))!=NDK_OK)
				{ 
					cls_show_msg1(g_keeptime,"line %d:切换3G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				 }
				 p=strstr(sRecvStr,"\",2");//联通3G标志
				 m=strstr(sRecvStr,"\",6");//联通3G标志
				 q=strstr(sRecvStr,"\",100");//电信3G标志
				 if(p!=NULL||q!=NULL||m!=NULL)
				{			
					cls_show_msg1(g_keeptime,"3G网络设置成功");
					break;
				}
				if(time(NULL)-oldtime>10)
				{
					cls_show_msg1(g_keeptime,"line %d:切换3G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				 }	
			}
			break;						
		case fourth_G:
			atm_cmd.pcAtCmd="+qcfg=\"nwscanmode\",3";			
			if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,2000,&pemStatus))!=NDK_OK)
			{ 
				 cls_show_msg1(g_keeptime,"line %d:切换4G网络失败(%d)", __LINE__, nRet);
				 return FAIL;
			}
			atm_cmd.pcAtCmd="+COPS?";				
			memset(sRecvStr,0,sizeof(sRecvStr));
			while(1)
			{
				if((nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,100,1000,&pemStatus))!=NDK_OK)
				{ 
					cls_show_msg1(g_keeptime,"line %d:切换4G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				 }
				 p=strstr(sRecvStr,"\",7");
				if(p!=NULL)
				{			
					cls_show_msg1(g_keeptime,"4G网络设置成功");
					break;
				}
				if(time(NULL)-oldtime>10)
				{
					cls_show_msg1(g_keeptime,"line %d:切换4G网络失败(%d)", __LINE__, nRet);
					return FAIL;
				}	
			}
			break; 
		default:
			return FAIL;
			break;
	}
	return SUCC;
  }
#endif

static int select_rst_flag(void)
{
	/*private & local definition*/
	int nKeyin = 0, flg[] = {RESET_PPPOPEN, RESET_PPPCLOSE, RESET_TCPOPEN, RESET_TCPCLOSE, RESET_NO};

	/*process body*/
	while (1)
	{	
		nKeyin = cls_show_msg("_选择复位类型_\n"
							"PPP:1.打开2.关闭\n"
							"TCP:3.打开4.关闭\n");
		switch(nKeyin)
		{
		case '1':
		case '2':
		case '3':
		case '4':
			return flg[nKeyin-'1'];
			break;
		case ESC:
			cls_show_msg1(2, "将不进行复位操作...");
			return RESET_NO;
			break;
		default:
			continue;
			break;
		}
	}	
}

static void wireless_dial_comm_press(int dialtype)
{
	/*private & local definition*/
	int i = 0, succ = 0, sq=0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	
	/*process body*/	
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	//数据通讯
	while(1)
	{
		NDK_WlModemGetSQ(&sq);
		if(cls_show_msg1(3, "开始第%d次短链接通讯(已成功%d次),ESC退出...当前信号量:%d", i+1, succ, sq)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;

		if(wireless_dial_comm(&sendpacket, RESET_NO)==SUCC)
			succ++;
		else
			continue;

		if(dialtype==1)//正常拨号时才要等待
			cls_show_msg1(rand()%35+5, "模块休息中,按任意键不休息继续压力...");//每次挂断之后重新连接要等待10秒,减轻绎芯片的压力
	}

	cls_show_msg_record(FILENAME,FUNCNAME,"短链接通讯压力测试完成,执行次数为%d,成功%d次", i, succ);
	return;
}

//长连接功能测试
static void ppp_outkeeptest(void)
{	
	/*private & local definition*/
	int timeout = SO_TIMEO, ret = 0, err = 0, i = 0;
	char buf[100] = {0}, rbuf[100] = {0};
	int slen = 0, rlen = 0;
	uint h_tcp = 0; 
	ST_PPP_CFG tmp_stPPPCfg;
	time_t diff = 0, oldtime = time(NULL);
	LinkType type = getlinktype();	
	
	/*process body*/
	for(i = 0;i<sizeof(buf);i++)
		buf[i]=rand()%255;

	memset(&tmp_stPPPCfg,0,sizeof(tmp_stPPPCfg));//20141201 3G模块支持后修改应用层必须传入正确拨号串，如果不串则使用上次的拨号串 
	if(type==CDMA)
		strcpy(tmp_stPPPCfg.szDailNum, "#777");
	else
		strcpy(tmp_stPPPCfg.szDailNum, "*99***1#");
	tmp_stPPPCfg.nDevType = 0;//0表示 无线,1为有线
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_KEEP;//长连接 // LCP_PPP_UNKEEPED;//短连接
	sprintf(tmp_stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(tmp_stPPPCfg.szApn, "\"%s\"", WLMAPN);
#else//低端产品不支持LCP_PPP_KEEP,LCP_PPP_UNKEEPED如果设置了应该不影响
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_KEEP;//设置不自动重连
	sprintf(tmp_stPPPCfg.szApn, WLMAPN);
#endif
	tmp_stPPPCfg.ModemDial = NULL;
	tmp_stPPPCfg.PPPIntervalTimeOut = 29;///**<维持长链接的数据包发送的时间间隔,<30S将不会进行长链接的维持*/
	strcpy(tmp_stPPPCfg.nPPPHostIP, netsetting.SvrIP.Str);//将服务器IP配置给长链接
	NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg));
	
	//测试前置
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}

	if(TransUp(&h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	//发送数据
	if((slen=sock_send(h_tcp, buf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, slen, sizeof(buf));
		return;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rlen, sizeof(buf));
		return;
	}
	if(MemCmp(buf, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
		return;	
	}
	if(TransDown(h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	
	cls_printf("请等待600秒");
	while(1)
	{
		if((diff=time(NULL)-oldtime)>600)//if(ReadStopwatch()>MAXTIMEOUT)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}
	show_stopwatch(DISABLE, 0);
	
	if((ret=WaitPPPState(PPP_STATUS_DISCONNECT, &err))!=SUCC)//<30S长链接时间小于30秒不进行长链接维护，600秒后ppp链接被踢掉
	{//先获取出错时的错误码,再关闭
		cls_show_msg1(10, "line %d:获取PHASE_NETWORK失败!(%d)", __LINE__, err);
		TransDown(h_tcp);
		NetDown();
		return;
	}
	if(TransUp(&h_tcp)==SUCC)//链接应该失败
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	TransDown(h_tcp);
	NetDown();
	cls_show_msg("超出长链接等待时间测试完成");
	return;
}

static void ppp_inkeeptest(void)
{	
	/*private & local definition*/
	int timeout = SO_TIMEO, ret = 0, err = 0, i = 0;
	char buf[100] = {0}, rbuf[100] = {0};
	int slen = 0, rlen = 0;
	uint h_tcp = 0; 
	ST_PPP_CFG tmp_stPPPCfg;
	time_t diff = 0, oldtime = time(NULL);
	LinkType type = getlinktype();
	
	/*process body*/
	for(i = 0;i<sizeof(buf);i++)
		buf[i]=rand()%255;

	memset(&tmp_stPPPCfg,0,sizeof(tmp_stPPPCfg));//20141201 3G模块支持后修改应用层必须传入正确拨号串，如果不串则使用上次的拨号串 
	if(type==CDMA)
		strcpy(tmp_stPPPCfg.szDailNum, "#777");
	else
		strcpy(tmp_stPPPCfg.szDailNum, "*99***1#");
	memset(tmp_stPPPCfg.szDailNum,0,sizeof(tmp_stPPPCfg.szDailNum));//20141201 linwl 国锋解释如果不传入拨号号码底层会根据模块类型选择默认值
	tmp_stPPPCfg.nDevType = 0;//0表示 无线,1为有线
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_KEEP;//长连接 // LCP_PPP_UNKEEPED;//短连接
	sprintf(tmp_stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(tmp_stPPPCfg.szApn, "\"%s\"", WLMAPN);
#else
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_KEEP;//低端产品不支持LCP_PPP_KEEP,LCP_PPP_UNKEEPED如果设置了应该不影响正常测试
	sprintf(tmp_stPPPCfg.szApn, WLMAPN);
#endif
	tmp_stPPPCfg.ModemDial = NULL;
	tmp_stPPPCfg.PPPIntervalTimeOut = 35;///**<维持长链接的数据包发送的时间间隔,<30S将不会进行长链接的维持*/
	strcpy(tmp_stPPPCfg.nPPPHostIP, netsetting.SvrIP.Str);//将服务器IP配置给长链接
	NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg));
	
	//测试前置
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}

	if(TransUp(&h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	//发送数据
	if((slen=sock_send(h_tcp, buf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, slen, sizeof(buf));
		return;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rlen, sizeof(buf));
		return;
	}
	if(MemCmp(buf, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
		return;	
	}
	if(TransDown(h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	
	cls_printf("请等待600秒");
	while(1)
	{
		if((diff=time(NULL)-oldtime)>600)//if(ReadStopwatch()>MAXTIMEOUT)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}
	show_stopwatch(DISABLE, 0);
	if((ret=WaitPPPState(PPP_STATUS_CONNECTED, &err))!=SUCC)//长链接每35秒发送一个维持包,所以理论上10分钟后还能继续维持着链路
	{//先获取出错时的错误码,再关闭
		cls_show_msg1(10, "line %d:获取PHASE_NETWORK失败!(%d)", __LINE__, err);
		TransDown(h_tcp);
		NetDown();
		return;
	}
	
	//ppp未挂断，TransUp应该成功
	if(TransUp(&h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	//发送数据
	if((slen=sock_send(h_tcp, buf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, slen, sizeof(buf));
		return;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rlen, sizeof(buf));
		return;
	}
	if(MemCmp(buf, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
		return;	
	}
	if (TransDown(h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransDown失败", __LINE__);
		NetDown();
		return;	
	}
	
	NetDown();
	cls_show_msg("长链接维持时间测试完成");
	return;
}


static void ppp_upkeeptest(void)
{	
	/*private & local definition*/
	int timeout = SO_TIMEO, ret = 0, err = 0, i = 0;
	char buf[100] = {0}, rbuf[100] = {0};
	int slen = 0, rlen = 0;
	uint h_tcp = 0; 
	ST_PPP_CFG tmp_stPPPCfg;
	time_t diff = 0, oldtime = time(NULL);
	LinkType type = getlinktype();
	
	/*process body*/
	for(i = 0;i<sizeof(buf);i++)
		buf[i]=rand()%255;

	memset(&tmp_stPPPCfg,0,sizeof(tmp_stPPPCfg));//20141201 3G模块支持后修改应用层必须传入正确拨号串，如果不串则使用上次的拨号串 
	if(type==CDMA)
		strcpy(tmp_stPPPCfg.szDailNum, "#777");
	else
		strcpy(tmp_stPPPCfg.szDailNum, "*99***1#");
	memset(tmp_stPPPCfg.szDailNum,0,sizeof(tmp_stPPPCfg.szDailNum));//20141201 linwl 国锋解释如果不传入拨号号码底层会根据模块类型选择默认值
	tmp_stPPPCfg.nDevType = 0;//0表示 无线,1为有线
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_UNKEEPED;//短连接
	sprintf(tmp_stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(tmp_stPPPCfg.szApn, "\"%s\"", WLMAPN);
#else//低端产品不支持LCP_PPP_KEEP,LCP_PPP_UNKEEPED如果设置了应该不影响
	tmp_stPPPCfg.nPPPFlag = LCP_PPP_UNKEEPED;//短连接
	sprintf(tmp_stPPPCfg.szApn, WLMAPN);
#endif
	tmp_stPPPCfg.ModemDial = NULL;
	//tmp_stPPPCfg.PPPIntervalTimeOut = 60;///**<维持长链接的数据包发送的时间间隔,<30S将不会进行长链接的维持*/
	
	NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg));
	
	//测试前置
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}

	if(TransUp(&h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	//发送数据
	if((slen=sock_send(h_tcp, buf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, slen, sizeof(buf));
		return;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sizeof(buf), timeout))!=sizeof(buf))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rlen, sizeof(buf));
		return;
	}
	if(MemCmp(buf, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
		return;	
	}
	if(TransDown(h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}

	cls_printf("请等待600秒");
	while(1)
	{
		if((diff=time(NULL)-oldtime)>600)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}
	show_stopwatch(DISABLE, 0);
	if((ret=WaitPPPState(PPP_STATUS_DISCONNECT, &err))!=SUCC)
	{//先获取出错时的错误码,再关闭
		cls_show_msg1(10, "line %d:获取PHASE_NETWORK失败!(%d)", __LINE__, err);
		TransDown(h_tcp);
		NetDown();
		return;
	}
	if(TransUp(&h_tcp)==SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return;
	}
	if((slen=sock_send(h_tcp, buf, sizeof(buf), timeout))<0)
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d)", __LINE__, slen);
		return;
	}
	TransDown(h_tcp);
	NetDown();
	cls_show_msg("短连接自动挂断测试完成");
	return;
}

//数据通讯,参数flag为非0时用于压力测试;参数flag为0时用于基站切换的异常测试
static void wireless_comm_press(int flag)
{	
	/*private & local definition*/
	int i = 0, last = -1, succ = 0, timeout = SO_TIMEO;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	int slen = 0, rlen = 0;
	uint h_tcp = 0; 
	
	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	//测试前置
PRE:
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		NetDown();
		cls_show_msg_record(FILENAME,FUNCNAME,"长链接通讯压力测试完成,执行次数为%d,成功%d次", i, succ);
		return;
	}
	NDK_SysSetSuspend(0);//TCP通讯前禁止休眠 20140808
	//数据通讯
	while(1)
	{

		if(cls_show_msg1(3, "开始第%d次长链接通讯(已成功%d次),ESC退出...", i+1, succ)==ESC)
			break;		
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;

		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			if(flag)
			{
				if(IsContinuous(&i, &last))
				{
					NetDown();
					goto PRE;
				}
				else
					continue;
			}
			else
				continue;
		}
		
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送数据失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
			if(flag)
			{
				if(IsContinuous(&i, &last))
				{
					NetDown();
					goto PRE;
				}
				else
					continue;
			}
			else
				continue;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收数据失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			if(flag)
			{
				if(IsContinuous(&i, &last))
				{
					NetDown();
					goto PRE;
				}
				else
					continue;
			}
			else
				continue;
		}
		if(MemCmp(sendpacket.header, rbuf, rlen))
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次校验失败", __LINE__, i);
			if(flag)
			{
				if(IsContinuous(&i, &last))
				{
					NetDown();
					goto PRE;
				}
				else
					continue;
			}
			else
				continue;
		}

		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, i);
			if(flag)
			{
				if(IsContinuous(&i, &last))
				{
					NetDown();
					goto PRE;
				}
				else
					continue;
			}
			else
				continue;
		}
		succ++;
	}
	NDK_SysSetSuspend(1);//TCP通讯前禁止休眠 20140808
	NetDown();
	cls_show_msg_record(FILENAME,FUNCNAME,"长链接通讯压力测试完成,执行次数为%d,成功%d次", i, succ);
	return;
}

static void wireless_dial_press(int dialtype)
{
	/*private & local definition*/
	int cnt = DEFAULT_CNT_VLE;
	char CntStr[8] = DEFAULT_CNT_STR;
	int i = 0, succ = 0, nRet = 0;
	uint unLen=0;
	
	/*process body*/
	cls_printf("默认压力测试次数:");

	if(g_SequencePressFlag) 
	{
		cnt = GetCycleValue();  //连续压力测试，需要手工输入循环次数的都直接设置默认值
	}
	else
	{
		if((nRet = lib_kbgetinput(CntStr,0,sizeof(CntStr)-1,&unLen,INPUTDISP_OTHER,0,INPUT_CONTRL_LIMIT))==NDK_OK&&(nRet==ESC||nRet ==NDK_ERR_TIMEOUT))
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
	}
	
	while(cnt)
	{
		if (ESC==cls_show_msg1(3, "无线拔号压力中...\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;
		i++;

		if(NetUp()!=SUCC)//如果内部的无线ppp出错,内部会停下提示
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次无线拔号失败", __LINE__, i);
			continue;
		}
		if(NetDown()!=SUCC)//如果内部的无线ppp出错,内部会停下提示
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次无线挂断失败", __LINE__, i);
			continue;
		}
		succ++;

		if(dialtype==1)//正常拨号时才要等待
			cls_show_msg1(rand()%35+5, "模块休息中,按任意键不休息继续压力...");//每次挂断之后重新连接要等待10秒,减轻绎芯片的压力
	}

	NetDown();
	cls_show_msg_record(FILENAME,FUNCNAME,"压力测试完成,已执行次数为%d,成功%d次", i, succ);
	return;
}
static void create_WLM_reset_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len = WLMPACKMAXLEN;//随意
	packet->lifecycle = 0;
	packet->forever = TRUE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

#if 0  //改成使用单向收发通讯采集性能 20180706 linying modify
static void WLM_ability(void)
{
	/*private & local definition*/
	int i = 0, timeout = SO_TIMEO;
	uint h_tcp = 0;
	float hanguptime = 0, hangdowntime = 0, writetime = 0, readtime = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet snd_packet;
#if K21_ENABLE
	char str[32] = {0}, str1[32] = {0};
#endif

	/*process body*/
	create_WLM_ability_packet(&snd_packet, buf);
	
	while(1)
	{
		if(update_snd_packet(&snd_packet)!=SUCC)
			break;
		i++;
			
		//先于NetUp调用LinkUp(避免由NetUp去调用LinkUp),以测准打开PPP的时间
		if(LinkUp()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次LinkUp失败", __LINE__, i);
			return;
		}
	
		lib_StartStopwatch();
		if(NetUp()!=SUCC)
		{
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次NetUp失败", __LINE__, i);
			return;
		}
		hanguptime += lib_ReadStopwatch();
		//至此获取到拔上网络的时间了
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次TransUp失败", __LINE__, i);
			return;
		}
		
		//数据收发的时间	
		//memset(buf,0xff,sizeof(buf));
		lib_StartStopwatch();
		sock_send(h_tcp, snd_packet.header, snd_packet.len, timeout);
		writetime += lib_ReadStopwatch();
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		lib_StartStopwatch();
		sock_recv(h_tcp, rbuf, snd_packet.len, timeout);
		readtime += lib_ReadStopwatch();
		
		if (TransDown(h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次TransDown失败", __LINE__, i);
			return;
		}
	
		lib_StartStopwatch();
		if(NetDown()!=SUCC)
		{
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次NetDown失败", __LINE__, i);
			return;			
		}
		hangdowntime += lib_ReadStopwatch();
		
		if(LinkDown()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次LinkDown失败", __LINE__, i);
			return;				
		}
	}//循环拨号20次后计算拨号和挂断的时间,取平均值
#if !K21_ENABLE
	cls_show_msg_record(FILENAME, FUNCNAME, "PPP拨号时间:%fs PPP挂断时间:%fs", hanguptime/PACKETLIFE, hangdowntime/PACKETLIFE);
	cls_show_msg_record(FILENAME, FUNCNAME, "发送/接收%dB数据耗时:%fs/%fs", snd_packet.len, writetime/PACKETLIFE, readtime/PACKETLIFE);
#else
	memset(str,0,sizeof(str));
	memset(str1,0,sizeof(str1));
	ftos(hanguptime/PACKETLIFE,str);
	ftos(hangdowntime/PACKETLIFE,str1);
	cls_show_msg_record(FILENAME, FUNCNAME, "PPP拨号时间:%ss PPP挂断时间:%ss", str, str1);
	memset(str,0,sizeof(str));
	memset(str1,0,sizeof(str1));
	ftos(writetime/PACKETLIFE,str);
	ftos(readtime/PACKETLIFE,str1);
	cls_show_msg_record(FILENAME, FUNCNAME, "发送/接收%dB数据耗时:%ss/%ss", snd_packet.len, str, str1);
#endif
	return;
}
#endif

//建立单向接收包
static void create_WLM_ability_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
#if K21_ENABLE
    packet->len = packet->orig_len= 1024;//低端无线模块每次都是分成1K，1K数据进行发送，服务器也是1K，1K回
#else
	packet->len = packet->orig_len= 1024*4;
#endif
	packet->lifecycle =PACKETLIFE;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

//产生随机数
static int  get_wlm_random_data(Packet *packet)
{
    /*private & local definition*/
	int j = 0;
	char *ptr = packet->header;
	/*process body*/
	if(packet->IsDataRnd)
	{
		for (j=0; j<packet->len; j++)
			*ptr++ = rand()%256;
	}
	return SUCC;
}
static void WLM_hang_up_down_ability(void)
{
	/*private & local definition*/
	int i = 0;
	uint h_tcp = 0;
	float hanguptime = 0, hangdowntime = 0;
	char buf[PACKMAXLEN] = {0};
	Packet snd_packet;
#if K21_ENABLE
	char str[32] = {0}, str1[32] = {0};
#endif

	/*process body*/
	create_WLM_ability_packet(&snd_packet, buf);
	
	while(1)
	{
		if(update_snd_packet(&snd_packet)!=SUCC)
			break;
		i++;
			
		//先于NetUp调用LinkUp(避免由NetUp去调用LinkUp),以测准打开PPP的时间
		if(LinkUp()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次LinkUp失败", __LINE__, i);
			return;
		}
	
		lib_StartStopwatch();
		if(NetUp()!=SUCC)
		{
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次NetUp失败", __LINE__, i);
			return;
		}
		hanguptime += lib_ReadStopwatch();
		//至此获取到拔上网络的时间了
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次TransUp失败", __LINE__, i);
			return;
		}
		
		if (TransDown(h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次TransDown失败", __LINE__, i);
			return;
		}
	
		lib_StartStopwatch();
		if(NetDown()!=SUCC)
		{
			LinkDown();
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次NetDown失败", __LINE__, i);
			return;			
		}
		hangdowntime += lib_ReadStopwatch();
		
		if(LinkDown()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:第%d次LinkDown失败", __LINE__, i);
			return;				
		}
	}//循环拨号30次后计算拨号和挂断的时间,取平均值
#if !K21_ENABLE
	cls_show_msg_record(FILENAME, FUNCNAME, "PPP拨号时间:%fs PPP挂断时间:%fs", hanguptime/PACKETLIFE, hangdowntime/PACKETLIFE);
#else
	memset(str,0,sizeof(str));
	memset(str1,0,sizeof(str1));
	ftos(hanguptime/PACKETLIFE,str);
	ftos(hangdowntime/PACKETLIFE,str1);
	cls_show_msg_record(FILENAME, FUNCNAME, "PPP拨号时间:%ss PPP挂断时间:%ss", str, str1);
#endif
	return;
}

//单向接收性能测试20180705 liny added
void WLM_recv_only_ability(uint h_tcp)
{
	/*private & local definition*/
	int i = 0, timeout = SO_TIMEO,  slen = 0, rlen = 0;
	float recvtime = 0,size=0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
#if K21_ENABLE
	char str[32] = {0};
#endif
	/*process body*/
	create_WLM_ability_packet(&sendpacket, buf);
	update_snd_packet(&sendpacket);
	sendpacket.IsDataRnd = FALSE;//缓冲区已初始过了,偷偷改变为不随机
	sendpacket.lifecycle = PACKETLIFE;
	if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__,slen, sendpacket.len);
		NetDown();
		LinkDown();
		return;
	}
	cls_printf("无线单向接收性能测试中...");
    	while(1)
	{
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		lib_StartStopwatch();
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收失败(实际%d, 预期%d)", __LINE__,i, rlen, sendpacket.len);
			NetDown();
			LinkDown();
			return;
		}
		recvtime += lib_ReadStopwatch();
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次校验失败", __LINE__,i);
			NetDown();
			LinkDown();
			return;
		}	
    }
	size=sendpacket.len/1024;
#if !K21_ENABLE
	cls_show_msg_record(FILENAME, FUNCNAME, "单向接收速率:%fKB/S",size*PACKETLIFE/recvtime);
#else
	memset(str,0,sizeof(str));
	ftos(size*PACKETLIFE/recvtime,str);
	cls_show_msg_record(FILENAME, FUNCNAME, "单向接收速率:%sKB/S",str);
#endif
    return;
}

//单向发送性能测试20180705 liny added
void WLM_send_only_ability(uint h_tcp)
{
	/*private & local definition*/
	int i = 0, timeout = SO_TIMEO, slen = 0;
	float sendtime = 0,size=0;
	char buf[PACKMAXLEN] = {0};
	Packet sendpacket;
#if K21_ENABLE
	char str[32] = {0};
#endif
	/*process body*/
	create_WLM_ability_packet(&sendpacket, buf);
	cls_printf("无线单向发送性能测试中...");
	while (i<PACKETLIFE)
	{
		i++;
		get_wlm_random_data(&sendpacket);
        lib_StartStopwatch();
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(实际%d, 预期%d)", __LINE__, i,slen, sendpacket.len);
		    	NetDown();
			LinkDown();
			return;
		}
		sendtime += lib_ReadStopwatch();
		NDK_SysDelay(20);//给后台一些接收时间2s(低端发送需要2s延时)
    }
	size=sendpacket.len/1024;
#if !K21_ENABLE
	cls_show_msg_record(FILENAME, FUNCNAME, "单向发送速率:%fKB/S",size*PACKETLIFE/sendtime);
#else
	memset(str,0,sizeof(str));
	ftos(size*PACKETLIFE/sendtime,str);
	cls_show_msg_record(FILENAME, FUNCNAME, "单向发送速率:%sKB/S",str);
#endif
    return;
}

//优化性能测试，取单向通讯性能数据20180705 liny added
void WLM_ability(void)
{
	/*private & local definition*/
	int nKey = '0';
	void (*pf[2])(uint) = {WLM_recv_only_ability, WLM_send_only_ability};
	uint h_tcp = -1;

	cls_show_msg("注意:测试单向收发性能时,需要更换成单向服务器后台,任意键继续");
	
	nKey = cls_show_msg("1.拨号和挂断时间\n"
						"2.单向接收性能\n"
						 "3.单向发送性能\n");
	switch(nKey)
	{
		case '1':
			WLM_hang_up_down_ability();
			break;
		case '2':
		case '3':
			if(LinkUp()!=SUCC)
		        {
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:LinkUp失败", __LINE__);
			        return;
		        }
		        if(NetUp()!=SUCC)
		        {
			        LinkDown();
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:NetUp失败", __LINE__);
			        return;
		        }
		        if(TransUp(&h_tcp)!=SUCC)
		        {
			        NetDown();
			        LinkDown();
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:TransUp失败", __LINE__);
			        return;
		        }
			pf[nKey-'2'](h_tcp);
			if (TransDown(h_tcp)!=SUCC)
		        {
			        NetDown();
			        LinkDown();
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:TransDown失败", __LINE__);
			        return;
		        }
		        if(NetDown()!=SUCC)
		        {
			        LinkDown();
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:NetDown失败", __LINE__);
			        return;			
		        }		
		        if(LinkDown()!=SUCC)
		        {
			        cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:LinkDown失败", __LINE__);
			        return;				
		        }
			break;
		case ESC:
			return;
			break;
		default:
			break;		
			
		}
}

static void wireless_press(void)
{
	/*private & local definition*/
	int nKeyin = '0', dialtype = 0;
	
	/*process body*/
	if(cls_show_msg1(5, "选择拨号压力类型:\n[确认]普通型\n[其它]增强型")==ENTER)
		dialtype = 1;//正常拨号
	else
		dialtype = 0;//异常拨号

	while(1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++nKeyin == '4' ) //对应下面的菜单数+1，菜单增加此处也要对应的增加
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"无线压力测试结束");
				return ;
			}
			if(cls_show_msg1(3, "即将进行连续压力测试,ESC退出")==ESC)
				return;
		}
		else
		{
			nKeyin = cls_show_msg("1.拨号压力\n2.长链接压力\n3.短链接压力");//长短链接是对PPP而言的
		}
		switch(nKeyin)
		{
		case '1':
			wireless_dial_press(dialtype);
			break;
		case '2':
			wireless_comm_press(1);
			break;
		case '3':
			wireless_dial_comm_press(dialtype);
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

static void wireless_reset(void)
{
	/*private & local definition*/
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	int rst_flag = RESET_NO;
	
	/*process body*/
	if((rst_flag=select_rst_flag())==RESET_NO)
		return;
	
	create_WLM_reset_packet(&sendpacket, buf);
	update_snd_packet(&sendpacket);

	wireless_dial_comm(&sendpacket, rst_flag);
	cls_show_msg("无线复位测试失败");//nener reach
	return;
}

static void wireless_PPP2AT(void)
{
	/*private & local definition*/
	int sq = 0, mux_support = 0;
	LinkType type = getlinktype();
	
	/*process body*/
	//这个配置较为独立,故暂放于此处
	mux_support = (cls_show_msg("无线模块支持多路复用?\n[确认]支持[其它]不支持")==ENTER)?TRUE:FALSE;
	
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}
	cls_printf("已处于数通态,尝试切换到AT命令态中,请耐心等待...");

	if((mux_support!=TRUE||NDK_WlModemGetSQ(&sq)!=NDK_OK||!(((type==GPRS||type==CDMA)&&sq<=SQ_2G_MAX&&sq>=SQ_2G_MIN)||(type==TD&&sq<=SQ_3G_MAX&&sq>=SQ_3G_MIN)))
	&&(mux_support!=FALSE||NDK_WlModemGetSQ(&sq)==NDK_OK))//信号值会取到最大值，故需加上= 20171013 modify by  linying
	{
		cls_show_msg("line %d:多路复用测试失败,信号值为%d", __LINE__,sq);
		NetDown();
		return;
	}

	if(NetDown()!=SUCC)
	{
		cls_show_msg("line %d:NetDown失败", __LINE__);
		return;
	}

	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}
	if(NetDown()!=SUCC)
	{
		cls_show_msg("line %d:NetDown失败", __LINE__);
		return;
	}
	
	cls_show_msg("PPP->AT测试成功");
	return;
}

//20140613 zhengdc 修改增加浅休眠、深休眠，自动休眠和函数休眠 两两组合情况下无线pppopen后功能验证
static void wireless_sleep(void)
{
	/*private & local definition*/
	int i = 2, nRet = -1, timeout = SO_TIMEO, errcode = 0, count = 0;
	int sleepflag = 0;
	uint h_tcp = 0; 
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	int slen = 0, rlen = 0;
	EM_PPP_STATUS pppstatus;
	
	/*process body*/
#if CPU5810X_ENABLE
	//保护动作:5810平台若不关掉计时器会使休眠自动唤醒
	uint a = 0;
	NDK_SysStopWatch(&a);
#endif

	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);

#if !K21_ENABLE
	while(1)
	{
		sleepflag=cls_show_msg("请选择休眠类型\n1.深休眠\n2.浅休眠(短时间内唤醒)\n3.浅休眠90分钟以上");//注意选择的休眠模式要和设置中一致
		
		if((sleepflag=='1')||(sleepflag=='2')||(sleepflag=='3'))
			break;		
	}
#else
	while(1)
	{
		sleepflag=cls_show_msg("请选择休眠类型\n2.浅休眠\n3.浅休眠90分钟以上");//注意选择的休眠模式要和设置中一致
		
		if((sleepflag=='2')||(sleepflag=='3'))
			break;		
	}
#endif
	
	while(i)
	{
		if(NetUp()!=SUCC)
		{
			cls_show_msg("line %d:NetUp失败", __LINE__);
			return;
		}
		if(i==2)
		{
			cls_show_msg("网络层已建立!任意键进入休眠,一段时间后请按键唤醒.");
#if K21_ENABLE//为避免低端平台产品有休眠定时关机的默认设置，设置休眠不关机
			NDK_SysSetShutdownDelay(0);
			sleep(1);
#endif
#if ANDIROD_ENABLE
			if((nRet=NDK_SysGoSuspend_Extern()) != NDK_OK)
#else
			if((nRet=NDK_SysGoSuspend()) != NDK_OK)
#endif
			{
				cls_show_msg("line %d:休眠失败(%d)", __LINE__, nRet);
				NetDown();
				return;
			}
		}
		else
			cls_show_msg("网络层已建立!等待pos自动进入休眠，休眠后请按键唤醒");
        if(cls_show_msg("休眠唤醒后，是否立刻出现该界面，是按[Enter],否按[其他]") != ENTER)//出现了唤醒延迟，底层发送AT指令延迟阻塞的现象，导致界面刷新延迟，增加现象判断，20170621 linying added
		{
			cls_show_msg("line %d:休眠唤醒后，界面刷新失败", __LINE__);
			NetDown();
			return;
		}
		sleep(1);//国锋解释唤醒后需要一点时间无线模块才可以返回正确的状态，应用也有一段操作时间
		if((nRet=NDK_PppCheck(&pppstatus, &errcode))!=NDK_OK)
		{
			cls_show_msg("line %d:获取PPP状态失败(%d)", __LINE__, nRet);
			NetDown();
			return;
		}
		
		if(sleepflag=='1')//深休眠ppp会被断开，需要重新连接，浅休眠ppp保持着 可以直接进行tcp
		{
			if(pppstatus!=PPP_STATUS_DISCONNECT)//判断状态是否为未连接状态PPP_STATUS_DISCONNECT
			{
				cls_show_msg("line %d:ppp状态不正确,测试失败(%d)", __LINE__,pppstatus);
				NetDown();
				return;
			}
			cls_printf("已退出休眠!重新进行PPP连接中...");
			//深休眠唤醒后,预期无线模块经历下电再上电的过程,故PPP应已断开
			set_linkstatus_down();
			set_netstatus_down();
			//重新进入PPP连接/断开应正常
			if(NetUp()!=SUCC)
			{
				cls_show_msg("line %d:NetUp失败", __LINE__);
				return;
			}
		}
		else if(sleepflag=='2')//浅休眠模式PPP状态应该任然是已连接PPP_STATUS_CONNECTED状态
		{
			if(pppstatus!=PPP_STATUS_CONNECTED)
			{
				cls_show_msg("line %d:ppp状态不正确,测试失败(%d)", __LINE__,pppstatus);
				NetDown();
				return;
			}
		}
		else if(sleepflag=='3')//测试休眠90分钟以上，状态可能断掉也可能未断掉，
		{
			if((pppstatus!=PPP_STATUS_CONNECTED)&&(pppstatus!=PPP_STATUS_DISCONNECT)&&(pppstatus!=PPP_STATUS_DISCONNECTING))
			{
				cls_show_msg("line %d:ppp状态不正确,测试失败(%d)", __LINE__,pppstatus);
				NetDown();
				return;
			}
			if(pppstatus==PPP_STATUS_DISCONNECTING)//如果底层正在挂断，则继续获取状态直到获取到已经挂断的状态
			{
				count=0;
				while(1)
				{
					sleep(1);
					if((nRet=NDK_PppCheck(&pppstatus, &errcode))!=NDK_OK)
					{
						cls_show_msg("line %d:获取PPP状态失败(%d)", __LINE__, nRet);
						NetDown();
						return;
					}
					if(pppstatus==PPP_STATUS_DISCONNECT)
						break;	
					count++;
					if(count>=30)//30秒获取状态时间
					{
						cls_show_msg("line %d:长时间未获取到挂断状态(%d)", __LINE__, count);
						NetDown();
						return;
					}
				}
			}
			if(pppstatus==PPP_STATUS_DISCONNECT)//PPP如果断开 则需要重连
			{
				//深休眠唤醒后,预期无线模块经历下电再上电的过程,故PPP应已断开
				set_linkstatus_down();
				set_netstatus_down();
				//重新进入PPP连接/断开应正常
				if(NetUp()!=SUCC)
				{
					cls_show_msg("line %d:NetUp失败", __LINE__);
					return;
				}
			}
			else//未断开则不做操作，直接连接TCP
				cls_show_msg("pppstatus=%d", pppstatus);
		}
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg("line %d:TransUp(%d)", __LINE__, h_tcp);
			NetDown();
			return;
		}
		
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg("line %d:数据发送失败，预期%d,实际%d", __LINE__, sendpacket.len,slen);
			NetDown();
			return;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg("line %d:数据接收失败，预期%d,实际%d", __LINE__, sendpacket.len,rlen);
			NetDown();
			return;
		}
		if(MemCmp(sendpacket.header, rbuf, rlen))
		{
			TransDown(h_tcp);
			cls_show_msg("line %d:休眠失败", __LINE__);
			NetDown();
			return;
		}

		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg("line %d:休眠失败", __LINE__);
			NetDown();
			return;
		}
		if(NetDown()!=SUCC)
		{
			cls_show_msg("line %d:NetDown失败", __LINE__);
			return;
		}
#if !K21_ENABLE
		i--;
#else
		i=i-2;//低端产品测试完函数休眠直接退出,不支持自动休眠功能
#endif
	}
	cls_show_msg("无线休眠测试成功");
	return;
}

#if K21_ENABLE
int set_ppp_autoconnect(void)
{	
	int ret = -1;
	ST_PPP_CFG stPPPCfg;
	
	if(cls_show_msg("是否设置ppp自动重连,是[Enter],否[其他]")==ENTER)
	{
		stPPPCfg.nPPPFlag = LCP_PPP_AUTOCONNECT ; //自动重连
		sprintf(stPPPCfg.szApn, WLMAPN);
		if((ret=NDK_PppSetCfg(&stPPPCfg, 0))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d:PPP自动重连设置失败",__LINE__);
			return FAIL;
		}
	}
	else
	{
		stPPPCfg.nPPPFlag = LCP_PPP_UNAUTOCONNECT; //不自动重连
		sprintf(stPPPCfg.szApn, WLMAPN);
		if((ret=NDK_PppSetCfg(&stPPPCfg, 0))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d:PPP不自动重连设置失败",__LINE__);
			return FAIL;
		}
	}

	return SUCC;
}
#endif

static void wireless_abnormity(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while (1)
	{
		keyin = cls_show_msg("1.复位测试\n"
			              	"2.PPP->AT\n"
							"3.休眠测试\n"
							"4.连接异常\n"
							"5.基站切换测试");
		switch (keyin)
		{
		case '1':
			wireless_reset();
			break;
		case '2':
			wireless_PPP2AT();//目前只有BGS2能够支持多路复用
			break;			
		case '3':
			wireless_sleep();
			break;
		case '4':
			tcp_abnormal();
			cls_show_msg("请配置未开启后台的服务器地址,应不能连通!");
			break;
		case '5':
#if K21_ENABLE
			if(set_ppp_autoconnect()!=SUCC)
				return;
#endif
			wireless_comm_press(0);
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

static void keeplinetest(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while (1)
	{
		keyin = cls_show_msg("1.维持长链接\n"
			              		"2.不维持长链接\n"
							"3.短链接\n"
							);
		switch (keyin)
		{
		case '1':
			ppp_inkeeptest();
			break;
		case '2':
			ppp_outkeeptest();
			break;			
		case '3':
			ppp_upkeeptest();
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

void systest4(void)
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	
#if CPU5810X_ENABLE
	int flag_4g=0,ret=-1;
	Network_t type = fourth_G;
	char *p=NULL;
	char szOutbuf[1024] = {0};
	ST_ATCMD_PACK STatcmdpack = {0};
	EM_WLM_STATUS emStatus = 0;
	//判断是否是EC20模块
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGMR;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((ret = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:获取模块版本失败%d)", __LINE__, ret);
		RETURN;
	}
	if((p=strstr(szOutbuf,"EC20"))!=NULL)
	    	flag_4g=1;
	else
		flag_4g=0;
#endif

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
#if CPU5810X_ENABLE
		if(flag_4g==1)
		{
			NDK_NetAddRouterTable(COMM_TYPE_PPP,"0.0.0.0");//防止之前测试人员跑完wifi后再跑无线，没有切换路由导致网络模式切换失败 20180808 liny added
			type=select_nettype();
			if(conf_conn_setseg(type)!=SUCC)
			{
			    cls_show_msg1(2, "网络模式切换失败!");
			    return;
			}
		}
#endif		
		if(conf_conn_WLM(TRUE)!=SUCC)
			return;
		g_CycleTime = 100;

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行压力测试
		wireless_press();

		//执行性能测试
		//wireless_ability();//改成单向收发性能后需要更换后台,无法自动获取性能值
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	else
	{
	    while (1)
	    {	
		    keyin = cls_show_msg("1.无线压力\n"
							"2.无线性能\n"
							"3.异常 0.配置\n"
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
							"4.长短链测试"
#endif							
							);
		    switch(keyin)
		    {
		    case '1':
			    wireless_press();
			    break;
		    case '2':
			    WLM_ability();
			    break;
		    case '3':
			    wireless_abnormity();
			    break;	
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
		    case '4':
			    keeplinetest();
			    break;
#endif
		    case '0':
#if CPU5810X_ENABLE
			    if(flag_4g==1)
			    {
				    NDK_NetAddRouterTable(COMM_TYPE_PPP,"0.0.0.0");//防止之前测试人员跑完wifi后再跑无线，没有切换路由导致网络模式切换失败 20180808 liny added
			        type=select_nettype();
			        if(conf_conn_setseg(type)!=SUCC)
			        {
			           cls_show_msg1(2, "网络模式切换失败!");
				       break;
			        }
		        }
#endif
			    switch(conf_conn_WLM(TRUE))
			    {
			    case SUCC:
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
		    case ESC:
			    return;
			    break;
		    default:
			    continue;
			    break;
		    }
	    }
    }
	
	return;
}

