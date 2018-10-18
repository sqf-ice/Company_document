/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: wlan综合测试
* file name			: 11.c
* Author 			: chensj
* version			: 
* DATE				: 20130521
* directory 		: 
* description		: wlan综合测试
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"
#if defined SP930P||defined SP930PG||defined SP630PG||defined SP830PG||defined ME31SMDM||defined SP930MDM
#include "iconvplus.h" //phoenix+平台所用的库文件为glibc iconv,与其它产品不同，为区别其它产品，区分头文件
#else
#include "iconv.h"
#endif

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"WLAN性能,压力"
#define	DEFAULT_CNT_STR	"100"	//"200"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)
#define	PACKETLIFE			(30)	//20  1000
#define	SCAN_PRESS		2000
#define	MAXWAITTIME 	60	
#define    DATASUM    1024*2400//2M多数据，单位byte

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

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
*			  		      chensj	        20130521	 	created
*****************************************************************/
//针对中端WiFi新增utf-8转GBK格式,使得从配置文件读取的utf-8格式转化后能够和扫描出来的gbk格式比对正确sull add 20171228
#if !K21_ENABLE
//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;
	
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) 
		return FAIL;
	memset(outbuf,0,outlen);
	//fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) 
		return FAIL;
	iconv_close(cd);
	return SUCC;
}
//UTF-8转为GBK码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("UTF-8","GBK",inbuf,inlen,outbuf,outlen);
}
#endif

static void create_wlan_ability_packet(Packet *packet, char *buf)
{
	/*private & local definition*/
	
	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	 packet->len = packet->orig_len= MIN(PACKMAXLEN,4096);//1024;//根据1.6规范进行修改 20131103 linwl
	packet->lifecycle = PACKETLIFE;//1000;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

#if 0  //改成单向接收和发送性能值  20180706 linying modifiy
//性能测试:通讯次数1000次,内容随机,长度1024
static void wlan_ability1(void)
{
	/*private & local definition*/
	int  i = 0, timeout = SO_TIMEO, tmpflag = 0;
	int slen = 0, rlen = 0, commsucc = 0, linksucc = 0, freesucc = 0, succ = 0;
	float commtimes = 0, preconnectTime = 0, freetimes=0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet snd_packet;
	uint h_tcp = -1;
#if K21_ENABLE
	char str[32] = {0};
#endif

	/*process body*/
	create_wlan_ability_packet(&snd_packet, buf);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=1)
		SetWifiStopflag(1);//确保做性能测试时wifi每次都调用stop去挂断
	
	while(1)
	{
		if(update_snd_packet(&snd_packet)!=SUCC)
			break;
		i++;

#if NEW_WIFI_ENABLE
		if(i==1)
			NDK_WifiStop();//新wifi在第一次测试前要做一次挂断操作,避免开机自动连接wifi引起的性能值误差
#endif
		lib_StartStopwatch();
		if(LinkUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次LinkUp失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;//出错继续,最后统计成功率20171208 
		}
		linksucc++;
		preconnectTime+=lib_ReadStopwatch();
	
		if(NetUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;
		}
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}
		
		//数据收发的时间	
		//memset(buf,0xff,sizeof(buf));
		lib_StartStopwatch();
		if((slen=sock_send(h_tcp, snd_packet.header, snd_packet.len, timeout))!=snd_packet.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, slen);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}	
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, snd_packet.len, timeout))!=snd_packet.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收失败(%d)", __LINE__, i, rlen);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}	
		commsucc++;
		commtimes+=lib_ReadStopwatch();
		
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, i);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}

		lib_StartStopwatch();
		if(NetDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetDown失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;
		}
		freesucc++;
		freetimes+=lib_ReadStopwatch();
		
		if(LinkDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次LinkDown失败", __LINE__, i);
			continue;//goto ERR;
		}
		succ++;
	}//循环拨号20次后计算拨号和挂断的时间,取平均值
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d:成功次数%d", PACKETLIFE, succ);
	cls_show_msg_record(FILENAME,FUNCNAME, "预连接%s耗时%fs\n", WlanEssid, preconnectTime/linksucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "通讯速率%fKB/s\n", (snd_packet.len*2/1024)*commsucc/commtimes);
	cls_show_msg_record(FILENAME,FUNCNAME, "free耗时%fs\n", freetimes/freesucc);
#else
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d:成功次数%d", PACKETLIFE, succ);
	memset(str,0,sizeof(str));
	ftos(preconnectTime/linksucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "预连接%s耗时%ss\n", WlanEssid, str);
	
	memset(str,0,sizeof(str));
	ftos((snd_packet.len*2/1024)*commsucc/commtimes,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "通讯速率%sKB/s\n", str);
	
	memset(str,0,sizeof(str));
	ftos(freetimes/freesucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "free耗时%ss\n", str);
#endif

ERR:
	SetWifiStopflag(tmpflag);
	return;
}
#endif

//性能测试:取预连接时间和free时间
static void wlan_prelink_free_ability(void)
{
	/*private & local definition*/
	int  i = 0, tmpflag = 0;
	int linksucc = 0, freesucc = 0, succ = 0;
	float preconnectTime = 0, freetimes=0;
	uint h_tcp = -1;
#if K21_ENABLE
	char str[32] = {0};
#endif

	/*process body*/
	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=1)
		SetWifiStopflag(1);//确保做性能测试时wifi每次都调用stop去挂断
	
	while(i<PACKETLIFE)
	{
		i++;
#if NEW_WIFI_ENABLE
		if(i==1)
			NDK_WifiStop();//新wifi在第一次测试前要做一次挂断操作,避免开机自动连接wifi引起的性能值误差
#endif
		lib_StartStopwatch();
		if(LinkUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次LinkUp失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;//出错继续,最后统计成功率20171208 
		}
		linksucc++;
		preconnectTime+=lib_ReadStopwatch();
		if(NetUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;
		}
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, i);
			NetDown();
			LinkDown();
			continue;//goto ERR;
		}

		lib_StartStopwatch();
		if(NetDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetDown失败", __LINE__, i);
			LinkDown();
			continue;//goto ERR;
		}
		freesucc++;
		freetimes+=lib_ReadStopwatch();
		if(LinkDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次LinkDown失败", __LINE__, i);
			continue;//goto ERR;
		}
		succ++;
	}//循环拨号PACKETLIFE次后计算拨号和挂断的时间,取平均值
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME,"连接总次数%d:成功次数%d", PACKETLIFE, succ);
	cls_show_msg_record(FILENAME,FUNCNAME, "预连接%s耗时%fs\n", WlanEssid, preconnectTime/linksucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "free耗时%fs\n", freetimes/freesucc);
#else
	cls_show_msg_record(FILENAME,FUNCNAME,"连接总次数%d:成功次数%d", PACKETLIFE, succ);
	memset(str,0,sizeof(str));
	ftos(preconnectTime/linksucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "预连接%s耗时%ss\n", WlanEssid, str);
	
	memset(str,0,sizeof(str));
	ftos(freetimes/freesucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "free耗时%ss\n", str);
#endif

//ERR:
	SetWifiStopflag(tmpflag);
	return;
}

//产生随机数
static int  get_wlan_random_data(Packet *packet)
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
	return  SUCC;
}

//建立单向接收包
static void create_ability_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	//中低端统一使用12k数据测试
	packet->len = packet->orig_len= PACKMAXLEN;
	packet->lifecycle =0;//PACKETLIFE;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

//单向接收性能测试20180705 liny added
void WLAN_recv_only_ability(uint h_tcp)
{
	/*private & local definition*/
	int i = 0, timeout = SO_TIMEO,  slen = 0, rlen = 0,k=0;
	float recvtime = 0,size=0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
#if K21_ENABLE
	char str[32] = {0};
#endif
	/*process body*/
	create_ability_packet(&sendpacket, buf);
	update_snd_packet(&sendpacket);
	k=sendpacket.lifecycle =DATASUM/sendpacket.len;
	sendpacket.IsDataRnd = FALSE;//缓冲区已初始过了,偷偷改变为不随机
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
	cls_show_msg_record(FILENAME, FUNCNAME, "单向接收速率:%fKB/S",size*k/recvtime);
#else
	memset(str,0,sizeof(str));
	ftos(size*k/recvtime,str);
	cls_show_msg_record(FILENAME, FUNCNAME, "单向接收速率:%sKB/S",str);
#endif
    return;
}

//单向发送性能测试20180705 liny added
void WLAN_send_only_ability(uint h_tcp)
{
	/*private & local definition*/
	int i = 0, timeout = SO_TIMEO, slen = 0, k=0;
	float sendtime = 0,size=0;
	char buf[PACKMAXLEN] = {0};
	Packet sendpacket;
#if K21_ENABLE
	char str[32] = {0};
#endif
	/*process body*/
	create_ability_packet(&sendpacket, buf);
	k=DATASUM/sendpacket.len;
	get_wlan_random_data(&sendpacket);
	cls_printf("无线单向发送性能测试中...");
	while (i<k)
	{
		i++;
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
		NDK_SysMsDelay(10);//给后台一些接收时间 10ms
    }
	size=sendpacket.len/1024;
	#if !K21_ENABLE
	cls_show_msg_record(FILENAME, FUNCNAME, "单向发送速率:%fKB/S",size*k/sendtime);
#else
	memset(str,0,sizeof(str));
	ftos(size*k/sendtime,str);
	cls_show_msg_record(FILENAME, FUNCNAME, "单向发送速率:%sKB/S",str);
#endif
    return;
}

//WiFi性能测试,数据通讯取单向通讯性能数据20180705 liny added
void wlan_ability1(void)
{
	/*private & local definition*/
	int nKey = '0';
	void (*pf[2])(uint) = {WLAN_recv_only_ability, WLAN_send_only_ability};
	uint h_tcp = -1;
	
	cls_show_msg("注意:测试单向收发性能时,需要更换成单向服务器后台,任意键继续");
		
	nKey = cls_show_msg("1.预连接和free时间\n"
		                  	"2.单向接收性能\n"
					"3.单向发送性能\n");
	switch(nKey)
	{
		case '1':
			wlan_prelink_free_ability();
			break;
		case'2':
		case'3':
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

#if !NEW_WIFI_ENABLE
//性能测试:wlan 加载性能测试，主要是考虑到开机时pos没有加载，应用程序第一次预连接时，会加载模块
//注意:free后的连接与开机首次连接的区别在于在/proc下是否会残留最后一次AP的信息
static void wlan_ability2(void)
{
	/*private & local definition*/
	ST_WIFI_PARAM init={0};
	int ret = 0;
	EM_WPA_CONSTATE state = 0;
	time_t diff = 0, oldtime = 0;
	float inittimes = 0.0;
#if K21_ENABLE
	char str[32] = {0};
	char str1[32] = {0};
#endif	

	/*process body*/
	//测试前置
	if(ENTER!=cls_show_msg("是否为机具上电后首次运行本用例?\n[确认]是[其它]否"))
	{	
		cls_show_msg("请重新开机后直接运行本用例.");
		return;
	}
		
	//setlinktype(WLAN);
	//cls_show_msg("请按AP(%s)的配置进行本端配置,任意键继续...",WlanEssid);

	// 2.本端配置
	//conf_WLAN_lnk1();
	//conf_local();

	memset(&init, 0, sizeof(init));
	init.psEthIp = netsetting.LocalIP.Str;
	init.psEthNetmask = netsetting.LocalMask.Str;
	init.psEthGateway = netsetting.LocalGateway.Str;
	init.psEthDnsPrimary = netsetting.DNSHost[0];
	init.psEthDnsSecondary = netsetting.DNSHost[1];
	init.emSecMode = linksetting.WLANEncMode;
	init.ucIfDHCP = netsetting.DHCPenable;
	init.emKeyType =  linksetting.WLANKeyType;
	init.pszKey = linksetting.WLANPwd;

	lib_StartStopwatch();//开始计时
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		cls_show_msg("line %d:测试失败ret=%d", __LINE__, ret);
		return;
	}
	inittimes=lib_ReadStopwatch();
	
	// 3.测试开始
	cls_printf("接入AP(%s)中...\n", WlanEssid);
	lib_StartStopwatch();//开始计时
	if((ret=NDK_WiFiConnect(WlanEssid, &init))!=NDK_OK)
	{
		cls_show_msg("line %d:连接%s失败!(%d)", __LINE__, WlanEssid, ret);
		NDK_WiFiShutdown();
		return;
	}

	oldtime = time(NULL);
	while(1)//do
	{
		if ((ret=NDK_WiFiConnectState(&state))!=NDK_OK||(diff=time(NULL)-oldtime)>30)//NORETURN_AFTERPREDIAL:0
		{
    		cls_show_msg1(10, "%s, line %d:连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
    		NDK_WiFiShutdown();
	    	return ;
    	}
		show_stopwatch(1, diff);
		if(state==WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
	}
	if((ret=NDK_WiFiIsConnected())!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		NDK_WiFiShutdown();
		return ;
	}
#if !K21_ENABLE
	cls_show_msg("开机首次连接%s耗时%fs\n初始化耗时%fs", WlanEssid, lib_ReadStopwatch(), inittimes);
#else
	memset(str,0,sizeof(str));
	ftos(lib_ReadStopwatch(),str);
	memset(str1,0,sizeof(str1));
	ftos(inittimes,str1);
	cls_show_msg("开机首次连接%s耗时%ss\n初始化耗时%ss", WlanEssid, str, str1);
#endif
	NDK_WiFiShutdown();
	return;
}
#else
static void wlan_ability2(void){return;}
#endif

//应开发要求，增兼容性性能测试项 sull add 20171219
static void wlan_ability3(void)
{
	/*private & local definition*/
	Packet snd_packet;
	int ret = 0, cnt = PACKETLIFE, tmpflag = 0, i = 0, slen = 0, rlen = 0, timeout = SO_TIMEO, count = 0; 
	int scansucc = 0, connsucc = 0, disconnsucc = 0, commsucc = 0, succ = 0;
	float scantimes = 0, conntimes = 0, disconntimes = 0, commtimes = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	uint h_tcp = -1;
	extern LinkStatus linkstatus;
	int sflag = 0;
#if NEW_WIFI_ENABLE
	ST_WIFI_STATUS status;
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	//char ssidout[32][33];//uint ssidoutlen = 0;//char gbk[32] = {0};
	char str[32] = {0};		
#else
	ST_WIFI_8021x_PARAM init2 ;
	ST_WIFI_PARAM init = {0};
	EM_WPA_CONSTATE state = 0;
	time_t diff = 0, oldtime = 0;
	int numList = 0;
	int initsucc = 0, downsucc = 0;
	float inittimes = 0, downtimes = 0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][128];
	ST_WIFI_APINFO ESSIDlist_info[64];
	char WlanEssid1[128] = {0};
#endif

	/*process body*/
	cls_printf("兼容性性能测试...");
	create_wlan_ability_packet(&snd_packet, buf);
	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=1)
		SetWifiStopflag(1);//确保做性能测试时wifi每次都调用stop去挂断
	//中端先把配置文件读取的utf-8格式的ssid转换成gbk格式
#if !K21_ENABLE	
	if((ret = u2g(WlanEssid, strlen(WlanEssid), WlanEssid1, sizeof(WlanEssid1))) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:utf8转换gbk失败(%d)", __LINE__, ret);
		return;
	}
#endif
	while(cnt--)
	{
		sflag = 0;
		if(update_snd_packet(&snd_packet)!=SUCC)
			break;
		count++;
#if NEW_WIFI_ENABLE
		if(count==1)
			NDK_WifiStop();//新wifi在第一次测试前要做一次挂断操作,避免开机自动连接wifi引起的性能值误差
#endif
#if NEW_WIFI_ENABLE
		//调用start函数
		lib_StartStopwatch();
		if((ret=NDK_WifiStart())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次启动连接%s失败!(%d)",  __LINE__, count, WlanEssid, ret);
			NDK_WifiStop();
			continue;
		}
		//判断连接成功状态
		if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, WlanEssid) )
		{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次连接%s超时失败!(ret=%d,status=%d,ssid=%s)", __LINE__,count,  WlanEssid, ret, status.status, status.ssid);
				NDK_WifiStop();
				continue;
		}
		connsucc++;
		conntimes += lib_ReadStopwatch();
		
		cls_printf("扫描AP中,请稍候...");
		list_scan.num = 32;
		list_scan.list = ap;
		lib_StartStopwatch();
		if (((NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan)) != NDK_OK)||list_scan.num <= 0) 
		{

			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次WIFI扫描失败", __LINE__ , count);
			NDK_WifiStop();
			continue;
		}
		for (i=0; i<list_scan.num; i++)
		{
			#if 0 //统一:如果是隐藏ap，不计算扫描时间  20180131
			if(!strcmp(ap[i].ssid,""))//如果是隐藏ap  直接传给输出buf
			{
				strcpy(ssidout[i],ap[i].ssid);
				if(!strcmp(ap[i].mac.octet,WifiMac))
				{
					scansucc++;
					scantimes + lib_ReadStopwatch();
					break;
				}
			}
			#endif
			if(WifiHiddenFlag)
				break;
			/*else
			{
				if((ret=NDK_Utf8ToGbk((uchar *)ap[i].ssid,strlen(ap[i].ssid),(uchar *)ssidout[i],&ssidoutlen))!=NDK_OK)
				{
					cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:utf8转换gbk失败(%d)", __LINE__, ret);
					continue;
				}
			}*///扫描出来的是utf-8格式,配置文件读取的也是utf-8格式,故不需要转化
			else
			{
				if(!strcmp(ap[i].ssid,WlanEssid))//选择SSID后
				{
					scansucc++;
					scantimes += lib_ReadStopwatch();
					break;
				}
			}	
		}
		if(i>=list_scan.num)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:未扫描到测试用AP请重新扫描", __LINE__);
			//NDK_WifiStop(); 扫描不成功不影响之后的测试，为了防止隐藏路由器不被扫到而无法继续之后的测试，不跳出循环  20180316 sull modify
			//continue;
			sflag = 1;  //如果扫描不成功，成功次数-1 20180320 modify
		}	
#else
		//中端wifi打开时间
		lib_StartStopwatch();
		if ((ret=NDK_WiFiInit()) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次WIFI设备初始化失败(%d)", __LINE__,count , ret);
			NDK_WiFiShutdown();
			continue;
		}
		initsucc++;
		inittimes += lib_ReadStopwatch();

		memset(ESSIDlist_info,0,sizeof(ESSIDlist_info));
		for (i=0; i<64; i++)
			ESSIDlist[i]=ESSIDlist1[i];

		//扫描AP
		cls_printf("扫描AP中,请稍候...");
		lib_StartStopwatch();
		if ((NDK_WiFiGetNetList(ESSIDlist, &numList) != NDK_OK) || numList <= 0) 
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次WIFI扫描失败", __LINE__ , count);
			NDK_WiFiShutdown();
			continue;
		}
		for (i=0; i<numList; i++)
		{
			if(!strcmp(ESSIDlist[i],""))  //统一:如果是隐藏ap，不计算扫描时间  20180131
				break;
			else
			{
				//ret=NDK_WiFiSignalCover(ESSIDlist1[i], &signal);//主要测试扫描时间，不需要获取信号值影响时间  20180129
				if(!strcmp(ESSIDlist[i],WlanEssid1))
				{
					scansucc++;
					scantimes += lib_ReadStopwatch();
					break;
				}
			}
		}
		if(i>=numList)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次未扫描到测试用AP请重新扫描", __LINE__, count);
			//NDK_WiFiShutdown(); 扫描不成功不影响之后的测试，为了防止隐藏路由器不被扫到而无法继续之后的测试，不跳出循环  20180316 sull modify
			//continue;
			sflag = 1;  //如果扫描不成功，成功次数-1 20180320 modify
		}

		init.ucIfDHCP = netsetting.DHCPenable;
		init.psEthIp = netsetting.LocalIP.Str;
		init.psEthNetmask = netsetting.LocalMask.Str;
		init.psEthGateway = netsetting.LocalGateway.Str;
		init.psEthDnsPrimary = netsetting.DNSHost[0];
		init.psEthDnsSecondary = netsetting.DNSHost[1];
		init.pszKey = linksetting.WLANPwd;
		init.emKeyType = linksetting.WLANKeyType;
		init.emSecMode = linksetting.WLANEncMode;
		
		cls_printf("接入AP(%s)中...\n", WlanEssid1);
		if(WlanEncMode==WIFI_NET_SEC_WPA_EAP)
		{
			init2.comm_param = init;
			init2.pszIdentity = linksetting.WLAN8021XIdentity;
			init2.emEap = linksetting.WLAN8021XEapMode;
			init2.pszca_cert = NULL;

			lib_StartStopwatch();
			if((ret=NDK_WiFiConnect_8021x(WlanEssid, &init2))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次连接%s失败!(%d)", __LINE__, count, WlanEssid, ret);
				NDK_WiFiDisconnect();
				NDK_WiFiShutdown();
				continue;
			}
		}
		else
		{
			lib_StartStopwatch();
			if((ret=NDK_WiFiConnect(WlanEssid, &init))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次连接%s失败!(%d)", __LINE__, count, WlanEssid, ret);
				NDK_WiFiDisconnect();
				NDK_WiFiShutdown();
				continue;
			}
		}
			
		oldtime = time(NULL);
		while(1)//do
		{
			if ((ret=NDK_WiFiConnectState(&state))!=NDK_OK||(diff=time(NULL)-oldtime)>30)//NORETURN_AFTERPREDIAL:0
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次连接%s失败!(%d,%d)", __LINE__, count, WlanEssid, ret, state);
				NDK_WiFiDisconnect();
				NDK_WiFiShutdown();
				break;
			}
			show_stopwatch(1, diff);
			if(state==WIFI_WPA_CONSTATE_CONTEXT)
				break;
			else
				NDK_SysMsDelay(10);//稳定一段时间再看看
		}
		if((ret=NDK_WiFiIsConnected())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次连接%s失败!(%d)", __LINE__, count, WlanEssid, ret);
			NDK_WiFiDisconnect();
			NDK_WiFiShutdown();
			continue;
		}
		connsucc++;
		conntimes += lib_ReadStopwatch();

#endif
		linkstatus=LINKUP;
		if((ret = NetUp()) != SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次建立网络层失败!(%d)",  __LINE__, count, ret);
			LinkDown();
			continue;
		}
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, count);
			NetDown();
			LinkDown();
			continue;
		}
		
		//数据收发的时间	
		lib_StartStopwatch();
		if((slen=sock_send(h_tcp, snd_packet.header, snd_packet.len, timeout))!=snd_packet.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, count, slen);
			NetDown();
			LinkDown();
			continue;
		}	
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, snd_packet.len, timeout))!=snd_packet.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收失败(%d)", __LINE__, count, rlen);
			NetDown();
			LinkDown();
			continue;
		}	
		commsucc++;
		commtimes+=lib_ReadStopwatch();

		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, count);
			NetDown();
			LinkDown();
			continue;
		}
#if NEW_WIFI_ENABLE
		//挂断操作
		if(g_wifistopflag == 1)
		{
			lib_StartStopwatch();
			if((ret=NDK_WifiStop())!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开%s失败!(%d)", __LINE__, count,WlanEssid, ret);
				NDK_WifiStop();
				continue;
			}
			disconnsucc++;
			disconntimes += lib_ReadStopwatch();
		}
		else
			;//关闭
#else
		cls_printf("断开AP(%s)连接中...\n", WlanEssid1); 
		lib_StartStopwatch();
		if((ret=NDK_WiFiDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开%s失败!(%d)", __LINE__, count,WlanEssid, ret);
			NDK_WiFiShutdown();
			continue;
		}
		disconnsucc++;
		disconntimes += lib_ReadStopwatch();

		lib_StartStopwatch();
		if((ret=NDK_WiFiShutdown())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开%s失败!(%d)", __LINE__, count,WlanEssid, ret);
			NDK_WiFiShutdown();
			continue;
		}
		downsucc++;
		downtimes += lib_ReadStopwatch();
#endif
		if(!sflag)
			succ++;
		linkstatus = LINKDOWN;
	}
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d:成功次数%d\n", PACKETLIFE, succ);
	cls_show_msg_record(FILENAME,FUNCNAME, "打开%s时间%fs\n", WlanEssid1, inittimes/initsucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "扫描%s时间%fs\n", WlanEssid1, scantimes/scansucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "连接%s时间%fs\n", WlanEssid1, conntimes/connsucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "通讯速率%fKB/s\n", (snd_packet.len*2/1024)*commsucc/commtimes);
	cls_show_msg_record(FILENAME,FUNCNAME, "断开%s时间%fs\n", WlanEssid1, disconntimes/disconnsucc);
	cls_show_msg_record(FILENAME,FUNCNAME, "关闭%s时间%fs\n", WlanEssid1, downtimes/downsucc);
#else
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d:成功次数%d", PACKETLIFE, succ);
	memset(str,0,sizeof(str));
	ftos(scantimes/scansucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "扫描%s时间%ss\n", WlanEssid, str);
	
	memset(str,0,sizeof(str));
	ftos(conntimes/connsucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "连接%s时间%ss\n", WlanEssid, str);

	memset(str,0,sizeof(str));
	ftos((snd_packet.len*2/1024)*commsucc/commtimes,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "通讯速率%sKB/s\n", str);
	
	memset(str,0,sizeof(str));
	ftos(disconntimes/disconnsucc,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "断开%s时间%ss\n", WlanEssid, str);
#endif
	return;
}

static void wlan_ability(void)
{
	/*private & local definition*/
	int key=0;
	
	/*process body*/
	while (1)
	{
		key = cls_show_msg("1.普通性能\n"
							"2.首次连接性能");
		
		switch(key)
		{
		case '1':
			wlan_ability1();
			break;
		case '2':
			wlan_ability2();
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

static void wlan_comm_press(void)
{
	/*private & local definition*/
	int nKey = '0';
	void (*pf[3])(uint) = {send_recv_press, recv_only_press, send_only_press};
	uint h_tcp = -1;
	
	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++nKey == '2' ) /*此处只做双向收发是因为双向和单向的后台不一样,对于2、3需要另外测试*/
			{
				return ;
			}
			cls_show_msg1(3, "要测试单向发送、单向接收压力需关闭连续压力开关进行");
		}
		else
		{
			nKey = cls_show_msg("_WLAN收发压力_\n"
								"1.双向收发压力\n"
								"2.单向接收压力\n"
								"3.单向发送压力\n");
		}
		
		switch(nKey)
		{
		case '1':
		case '2':
		case '3':
			if(NetUp()!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
				return;
			}
			
			if(TransUp(&h_tcp)!=SUCC)
			{
				NetDown();
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:TransUp失败", __LINE__);
				return;
			}
			pf[nKey-'1'](h_tcp);
			TransDown(h_tcp);
			NetDown();
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

static void wlan_cnnt_press(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0;
	uint h_tcp = -1;
	
	/*process body*/
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}
	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次连接压力(已成功%d次),按ESC退出", i+1, succ_count)==ESC)
			break;

		if(g_SequencePressFlag&&(GetCycleValue()==i))  //自动压力测试次数达到后退出
			break;
		
		i++;
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransDown失败", __LINE__);
			continue;
		}
		succ_count++;
	}
	
	NetDown();
	
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯次数:%d,成功次数:%d", i, succ_count);
	return;
}

static void cnnt_comm_press(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0, j = 0, loop = 0, relaykey = 0;
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
	uint h_tcp = -1;
#if NEW_WIFI_ENABLE	
	ST_WIFI_AP_INFO stApInfo;
#endif	
	/*process body*/
	relaykey = cls_show_msg1(5,"压力中是否要有间隔:[ENTER]是 [其他]否")==ENTER?TRUE:FALSE;
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}
	
BEGIN:	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次建链/通讯混合压力(已成功%d次),本次将收发%d回", i+1, succ_count, (loop=(rand()%10+1)))==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;

		
		if(relaykey==TRUE) 
		{
			if(i%5==0)
			{	
				j = rand();
				if(j%3==1)
					cls_show_msg1(120,"等待120秒");
				else if(j%3==2)
					cls_show_msg1(180,"等待180秒");
				else //if(j%3==0)
					cls_show_msg1(240,"等待240秒");
			}
		}
		
		//UP
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		
		for(j=0; j<loop; j++)
		{
		//发送数据
#if NEW_WIFI_ENABLE
		if(NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP, &stApInfo)!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:获取已连接的AP信息失败", __LINE__)==ESC)
				break;
			else
				continue;
		}
#endif
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
			goto BEGIN;//continue;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
			goto BEGIN;//continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验失败", __LINE__);
			goto BEGIN;//continue;
		}
		}
		
		//DOWN
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransDown失败", __LINE__);
			continue;
		}
		succ_count++;
	}
	
	NetDown();

	cls_show_msg_record(FILENAME,FUNCNAME,"通讯次数:%d,成功次数:%d", i, succ_count);
	return;
}

static void wlan_updown_press(void)
{
	/*private & local definition*/
	int i = 0, succ = 0;
	uint h_tcp = -1;
	
	/*process body*/
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次建链压力(已成功%d次)", i+1, succ)==ESC)
			break;

		if(g_SequencePressFlag&&(GetCycleValue()==i))  //自动压力测试次数达到后退出
			break;
		
		i++;
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			NetDown();
			continue;
		}
		if(TransUp(&h_tcp)!=SUCC)//让POS与服务器实际发生一下连接,以测试以太网开关对连接的影响
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			NetDown();
			continue;
		}
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, i);
			NetDown();
			continue;
		}		
		if(NetDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetDown失败", __LINE__, i);
			continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"压力次数:%d,成功次数:%d", i, succ);
	return;
}

#if !CPU5810X_ENABLE
static void ping_press(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, i = 0, cnt = DEFAULT_CNT_VLE;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen=0;

	/*process body*/
	if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
	{
		cnt = GetCycleValue();
	}
	else
	{
		cls_printf("默认压力测试次数:");
		if ((ret= lib_kbgetinput(CntStr,2, sizeof(CntStr)-1, &unLen,2, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
	}
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}

	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "ping压力测试中\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;
		i++;

		switch(ret=NDK_NetPing(netsetting.SvrIP.Str,30))
		{
		case 0:
			succ++;
			break;
		default:
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "错误返回%d", ret);
			break;
		}
		
	}
	NetDown();

	cls_show_msg_record(FILENAME,FUNCNAME,"ping压力测试%d次,成功%d次", i, succ);
	return;
} 
#else
static void ping_press(void)
{
	cls_show_msg_record(FILENAME,FUNCNAME,"不支持ping压力测试");
	return;
}
#endif

static void scan_press(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = SCAN_PRESS, k = 0;
	char CntStr[8] = "2000";
	uint unLen = 0;
#if NEW_WIFI_ENABLE
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	list_scan.list = ap;
#else
	int j = 0, i = 0, numList = 0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];
#endif

	/*process body*/
	cls_printf("默认压力测试次数:");
	if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
	{
		cnt = GetCycleValue();
	}
	else
	{
		if ((ret= lib_kbgetinput(CntStr,2, sizeof(CntStr)-1, &unLen,2, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
			cnt = SCAN_PRESS;
		else
			cnt = atoi(CntStr);
	}
#if NEW_WIFI_ENABLE
	//测试前置:
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}
	k=0;
	while(1)
	{
		if(cls_show_msg1(2, "Wifi扫描压力测试中\n总共%d次,还剩%d次,成功%d次,ESC退出",cnt,cnt-k,succ)==ESC)
			break;
		if(k++==cnt)
			break;
		memset(ap,0,sizeof(ap));
		list_scan.num = 10; 
		if ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan)) != NDK_OK) 
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME,3,"line %d:%s第%d次测试失败,ESC退出测试,其他键继续扫描", __LINE__, TESTITEM, k)==ESC)
				return;
		}	
		succ++;
	}
	if(NetDown()!=SUCC)
	{
		cls_show_msg("line %d:NetDown失败", __LINE__);
		return;
	}
#else
	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];
	//测试前置:WIFI设备初始化
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:%s测试失败ret=%d", __LINE__, TESTITEM, ret);
		return;
	}

	k=0;
	while(1)
	{
		cls_printf("Wifi扫描压力测试中\n总共%d次,还剩%d次,成功%d次",cnt,cnt-k,succ);
		if(k++==cnt)
			break;
		for (j=0; j<3; j++)
		{
			if (NDK_WiFiGetNetList(ESSIDlist, &numList) == NDK_OK)
			{
#if 0
				if (numList > 0) 
				{
					for (i=0; i<numList; i++)
						cls_show_msg1(g_keeptime, "共扫描到%d个AP,line:%d,AP[%d]=%s", numList,__LINE__, i, ESSIDlist1[i]);
					break;
				}
#else
				cls_show_msg1(1, "本次共扫描到%d个ap", numList);
#endif
				break;
			}
			sleep(1);
		}
		if (j == 3) /* 连续3次扫描失败则认为是异常*/
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,3,"line %d:%s第%d次测试失败,ESC退出测试,其他键继续扫描", __LINE__, TESTITEM,k)==ESC)
				return;
		}
		succ++;
	}
	NDK_WiFiShutdown();
#endif	
	cls_show_msg_record(FILENAME,FUNCNAME,"wifi扫描压力测试%d次,成功%d次", k-1, succ);
	return;
} 

//设计说明:考虑到支持wifi的机器可能不再有以太网口,则ping一类的协议就不能通过以太网综合测试来进行,故在本用例中也应有ping的测试
static void wlan_press(void)
{
	/*private & local definition*/
	int nKey='0';
	
	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++nKey == '7' ) //对应下面的菜单数+1，菜单增加此处也要对应的增加
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"%s连续压力测试结束",TESTITEM);
				return ;
			}
			if(cls_show_msg1(3, "即将进行连续压力测试,ESC退出")==ESC)
				return;
		}
		else
		{
			nKey = cls_show_msg("1.数传2.连接\n"
								"3.连接+数传\n"
								"4.建链压力\n"
								"5.ping压力 6.扫描");
		}
		switch(nKey)
		{
		case '1':
			wlan_comm_press();
			break;
		case '2':
			wlan_cnnt_press();
			break;
		case '3':
			cnnt_comm_press();
			break;
		case '4':
			wlan_updown_press();
			break;
		case '5':
			ping_press();
			break;
		case '6':
			scan_press();
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

//设计说明:考虑到第2个AP可能没有可用的TCP/UDP后台,本交叉测试不做传输层的连接动作
void wlan_cross_AP(void)
{
#if NEW_WIFI_ENABLE
	/*private & local definition*/
	int ret = -1, retime = -1, i = 0, succ = 0;
	ST_WIFI_STATUS status;
	time_t oldtime;

	/*process body*/
	//设置重连时间
	retime = 5000;//5000ms  
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		cls_show_msg("line %d: 设置重连失败(%d)", __LINE__, ret);
		return;
	}
	if(NetUp()!=SUCC)
	{
		cls_show_msg( "line %d:NetUp失败", __LINE__);
		NetDown();
		return;
	}
	i=0;
	while(1)
	{
		if(cls_show_msg1(2, "正在进行第%d次接入%s(已成功%d次),ESC退出", i+1, WlanEssid, succ)==ESC)
			break;
		if(i++==10)
			break;
		cls_show_msg("请关闭测试路由器后任意键继续");
		/*if((ret = lib_getwifistatus(&status)) != NDK_OK)
		{
			cls_show_msg("line %d:获取wifi状态失败", __LINE__, ret);
			return;
		}*/
		//不调用封装函数的原因是关闭路由器后底层并不能立即反应,那么在获取状态时还会获取到成功的状态，为了避免影响测试结果
		oldtime=time(NULL);
		while(1)
		{
			if((time(NULL)-oldtime)>30)
			{
				cls_show_msg1(3,"未获取到正确的状态,超时退出");
				break;
			}
			if((ret = NDK_WifiStatus(&status)) != NDK_OK)
			{
				cls_show_msg("line %d:获取 wifi状态失败(%d)", __LINE__, ret);
				return;
			}
			if(status.status == WIFI_WLAN_STATUS_FAIL)
				break;
		}
		if(status.status != WIFI_WLAN_STATUS_FAIL)
		{
			cls_show_msg("line %d: 获取状态失败(%d)", __LINE__, status.status);
			return;
		}
		cls_show_msg("请打开测试路由器10s后任意键继续");
		/*if((ret = lib_getwifistatus(&status)) != NDK_OK)
		{
			cls_show_msg("line %d:获取wifi状态失败", __LINE__, ret);
			return;
		}*/
		//不调用封装函数的原因是为了避免按键继续后可能会获取到失败状态 影响测试结果
		oldtime=time(NULL);
		while(1)
		{
			if((time(NULL)-oldtime)>30)
			{
				cls_show_msg1(3,"未获取到正确的状态,超时退出");
				break;
			}
			if((ret = NDK_WifiStatus(&status)) != NDK_OK)
			{
				cls_show_msg("line %d:获取 wifi状态失败(%d)", __LINE__, ret);
				return;
			}
			if(status.status == WIFI_WLAN_STATUS_SUCC)
				break;
		}
		if(status.status != WIFI_WLAN_STATUS_SUCC )
		{
			cls_show_msg("line %d: 获取状态失败(%d)", __LINE__, status.status);
			return;
		}
		succ++;
	}
	if(NetDown()!=SUCC)
	{
		cls_show_msg("line %d:NetDown失败", __LINE__);
		return;
	}
	cls_show_msg("跨AP测试通过...");
	//恢复默认值
	retime = -1;  
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		cls_show_msg("line %d: 设置重连失败(%d)", __LINE__, ret);
		return;
	}
#else
	/*private & local definition*/
	LinkSetting linksetting1, linksetting2;
	NetworkSetting netsetting1, netsetting2;
	int total1 = 0, succ1 = 0, total2 = 0, succ2 = 0, *total = NULL, *succ = NULL;

	/*process body*/
	// 1.备份第1个AP(第1个AP理应在之前的配置中已配置过了)
	memcpy(&linksetting1, &linksetting, sizeof(LinkSetting));
	memcpy(&netsetting1, &netsetting, sizeof(NetworkSetting));
	
	// 2.配置第2个AP,也进行备份(也从全局设置导出到本函数的局部设置)
	cls_show_msg("已配置%s为第1个AP,任意键进入第2个AP的配置...", linksetting1.WLANEssid);
	//手工配置链路参数
	g_conf_flag = 0;//为避免之前使用过配置文件,这里要将开关设置成手动配置
	if(conf_link(WLAN)!=SUCC)
	{
		cls_show_msg("line %d:配置链路参数失败", __LINE__);
		goto STEP4;
	}
	//手工配置本地网络参数
	if(conf_local()!=SUCC)
	{
		cls_show_msg("line %d:配置本地网络参数失败", __LINE__);
		goto STEP4;
	}
	memcpy(&linksetting2, &linksetting, sizeof(LinkSetting));
	memcpy(&netsetting2, &netsetting, sizeof(NetworkSetting));

	// 3.跨AP测试(测试前需要将备份的AP导入全局设置)
	while (1)
	{
		if((total1+total2)%2)//if(rand()%2)
		{
			total = &total1;
			succ = &succ1;
			memcpy(&linksetting, &linksetting1, sizeof(LinkSetting));
			memcpy(&netsetting, &netsetting1, sizeof(NetworkSetting));
		}
		else
		{
			total = &total2;
			succ = &succ2;
			memcpy(&linksetting, &linksetting2, sizeof(LinkSetting));
			memcpy(&netsetting, &netsetting2, sizeof(NetworkSetting));
		}
		
		if(cls_show_msg1(2, "正在进行第%d次接入%s(已成功%d次)", *total+1, WlanEssid, *succ)==ESC)
			break;
		++*total;
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NetUp失败", __LINE__, *total);
			NetDown();
			continue;
		}
		if(NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NetDown失败", __LINE__, *total);
			continue;
		}
		++*succ;
	}
	
	cls_show_msg("接入AP1(%s)次数:%d\n成功次数:%d", linksetting1.WLANEssid, total1, succ1);
	cls_show_msg("接入AP2(%s)次数:%d\n成功次数:%d", linksetting2.WLANEssid, total2, succ2);

STEP4:
	// 4.恢复第1个AP为本综合测试的配置AP
	memcpy(&linksetting, &linksetting1, sizeof(LinkSetting));
	memcpy(&netsetting, &netsetting1, sizeof(NetworkSetting));
	
#endif	
	return;
}

//目前是在进入(浅/深)休眠时,均会进行wlan_free来释放资源
static void wlan_sleep(void)
{
	/*private & local definition*/
	extern NetStatus netstatus;
	extern LinkStatus linkstatus;
	int nRet = -1, i = 2;
	uint h_tcp = -1;
	
	/*process body*/
#if NEW_WIFI_ENABLE 
	ST_WIFI_STATUS status;
	int retime = 1;
	time_t oldtime;

#if CPU5810X_ENABLE
	//保护动作:5810平台若不关掉计时器会使休眠自动唤醒
	uint a = 0;
	NDK_SysStopWatch(&a);
#endif

	//设置重连时间
	retime = 5000;//5000ms  
	if((nRet = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		cls_show_msg("line %d: 设置重连失败(%d)", __LINE__, nRet);
		return;
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
			cls_show_msg("网络层已建立!任意键进入休眠,一段时间(覆盖短时间和长时间)后请按键唤醒.");
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
		
		cls_printf("已退出休眠!重新进行wlan连接中...");
		//进入休眠后,预期WLAN模块已被free,故连接应已断开
		netstatus = NETDOWN;
		linkstatus = LINKDOWN;
		//重新进入wifi连接/断开应正常
#if NEW_WIFI_ENABLE
		//新wifi能够自动重连并成功
		//根据黄景辉描述若是长时间休眠后唤醒需要时间进行重连,故改成循环获取所需状态 20180718 modify
		oldtime=time(NULL);
		while(1)
		{
			if((nRet = NDK_WifiStatus(&status)) != NDK_OK)
			{
				cls_show_msg("line %d:获取wifi状态失败", __LINE__, nRet);
				return;
			}
			if(status.status == WIFI_WLAN_STATUS_SUCC)
				break;
			if((time(NULL)-oldtime)>MAXWAITTIME)//60s超时
			{
				cls_show_msg("line %d: wifi重连失败,超时退出(status=%d)", __LINE__, status.status);
				return;
			}
		}
#else
		if(NetUp()!=SUCC)
		{
			cls_show_msg("line %d:NetUp失败", __LINE__);
			return;
		}
#endif
//增加通讯 
		if(TransUp(&h_tcp)!=SUCC)
		{
			NetDown();
			cls_show_msg("line %d:TransUp失败", __LINE__);
			return;
		}
		//可以做10次通讯测试即可
		send_recv_press(h_tcp);
		TransDown(h_tcp);
		if(NetDown()!=SUCC)
		{
			cls_show_msg("line %d:NetDown失败", __LINE__);
			return;
		}
		i--;
	}

	cls_show_msg("WLAN休眠测试成功");
	return;
}

//选择复位的位置
static int select_rst_flag(void)
{
	/*private & local definition*/
	int nKeyin = 0, flg[] = {RESET_PPPOPEN, RESET_PPPCLOSE, RESET_TCPOPEN, RESET_TCPCLOSE, RESET_NO};

	/*process body*/
	while (1)
	{	
		nKeyin = cls_show_msg("_选择复位类型_\n"
							"WLAN:1.打开2.关闭\n"
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

static void create_WLAN_reset_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len = PACKMAXLEN;//随意
	packet->lifecycle = 0;
	packet->forever = TRUE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

//wifi通讯函数
int wlan_dial_comm(Packet *sendpacket, int flag)
{
	/*private & local definition*/
	uint h_tcp = -1;
	char rbuf[PACKMAXLEN] = {0};
	int slen = 0, rlen = 0, ret = 0, timeout = SO_TIMEO;
	
	/*process body*/
	//测试前置
	if((ret=NetUp())!=SUCC)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:NetUp失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_PPPOPEN)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "WLAN已连接(TCP未打开)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	if((ret=TransUp(&h_tcp))!=SUCC)
	{
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:TransUp失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_TCPOPEN)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "TCP已打开(WLAN已连接)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	//发送数据
	if((slen=sock_send(h_tcp, sendpacket->header, sendpacket->len, timeout))!=sendpacket->len)
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:发送数据失败(实际%d, 预期%d)", __FILE__, __LINE__, slen, sendpacket->len);
		return FAIL;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sendpacket->len, timeout))!=sendpacket->len)
	{
		TransDown(h_tcp);//TransDown(netsetting.socktype, h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:接收数据失败(实际%d, 预期%d)", __FILE__, __LINE__, rlen, sendpacket->len);
		return FAIL;
	}
	if(MemCmp(sendpacket->header, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:校验失败", __FILE__, __LINE__);
		return FAIL;
	}	
	if ((ret=TransDown(h_tcp))!=SUCC)
	{
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:TransDown失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_TCPCLOSE)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "TCP已关闭(WLAN未关闭)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	NetDown();
	if(flag==RESET_PPPCLOSE)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "WLAN已关闭(TCP已关闭)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	return SUCC;
}

//wifi复位函数
static void wlan_reset(void)
{
	/*private & local definition*/
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	int rst_flag = RESET_NO;
	
	/*process body*/
	if((rst_flag=select_rst_flag())==RESET_NO)
		return;
	
	create_WLAN_reset_packet(&sendpacket, buf);
	update_snd_packet(&sendpacket);

	wlan_dial_comm(&sendpacket, rst_flag);
	cls_show_msg("WIFI复位测试失败");//nener reach
	return;
}

//设计说明:本用例设计前提是AP信号强度和距离成反比，距离越近信号越强，不考虑设备差异情况
void wlan_same_AP(void)
{
	/*private & local definition*/	
	char szIPaddr[16] = {0}, tmp_szIPaddr[16] = {0};
	time_t oldtime = 0;

	/*process body*/
	cls_show_msg("请保持状态栏开启，并准备两个同名的AP(相同的加密模式和密码，不同的IP网段),将两个AP保持一定的距离(20米)后按任意键继续");
	//手工配置链路参数
	if(conf_link(WLAN)!=SUCC)
	{
		cls_show_msg("line %d:配置链路参数失败", __LINE__);
		return;
	}
	//本测试为了能够区分连接到的AP所属的网段采用动态分配方式获取IP
	netsetting.DHCPenable = ENABLE;

	// 3.跨AP测试(测试前需要将备份的AP导入全局设置)
	cls_show_msg("请靠近其中一个AP后按任意键开始连接AP");
	if(NetUp()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetUp失败", __LINE__);
		return;
	}
	memset(szIPaddr,0,sizeof(szIPaddr));	
	while (1)
	{		
		oldtime=time(NULL);
		while(1)
		{
			if((time(NULL)-oldtime)>MAXWAITTIME)
			{
				cls_show_msg1(g_keeptime,"超时(60s)未取到不同IP退出");
				break;	
			}
			strcpy(tmp_szIPaddr,szIPaddr);
			NDK_NetGetAddr(COMM_TYPE_WIFI, szIPaddr, NULL, NULL, NULL);
			if(strcmp(tmp_szIPaddr,szIPaddr))
				break;
			
		}
		if(cls_show_msg("已经成功连接到AP:%s,本地IP地址为:%s,该IP是否为较近的AP所在网段:[是]按确认，否则按其它键", WlanEssid, szIPaddr)!=ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:同名AP测试失败", __LINE__);
			NetDown();//出错后挂断从新连接
			NetUp();
			continue;
		}
		
		if(cls_show_msg("请移动POS离开已连接的AP的信号范围，当状态栏上AP信号消失后靠近另一个同名AP,等状态栏出现AP信号后按任意键,取消键退出测试")==ESC)
			break;
	}
	
	if(NetDown()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetDown失败", __LINE__);
		return;
	}
	
	cls_show_msg("同名AP测试完成");
	return;
}

static void wlan_abnormity(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while (1)
	{
		keyin = cls_show_msg("1.跨AP  2.复位\n" 	//隐藏AP
							"3.休眠\n"				// 6.同名\n,暂时屏蔽该用例,改成使用4.建链压力进行测试
							"4.连接异常\n"
							"5.禁止DHCP\n");
		switch (keyin)
		{
		case '1':
			wlan_cross_AP();
			break;
		case '2':
			wlan_reset();
			break;
		case '3':
			if(ENTER==cls_show_msg("请分别在管理中设置深/浅休眠模式后进行测试,[ENTER]继续,[其它]放弃"))
				wlan_sleep();//WLAN在深，浅休眠模式下都应该要进行自动休眠和函数休眠的功能验证 20140615 linwl
			break;
		case '4':
			cls_show_msg("请配置未开启后台的服务器地址,应不能连通!");
			break;
	//	case '4':
			//cls_show_msg("请将一个AP设置成隐藏SSID,应扫描不到该SSID,但应该能够连接该AP");//移植到单元测试用例中linwl20130614
	//		break;
		case '5':
			cls_show_msg("请将一个AP的DHCP是设置成禁止的，使用静态连接应该能够成功!");
			break;
	//	case '6':
	//		wlan_same_AP();
	//		break;
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

void AP_Compatibility_test(void)
{
	int ApNUM = 0, i = 0, dhcpflag = 0, cnt = 0;
	char localip[16] = {0}, localmask[16] = {0}, localgateway[16] = {0};
	char pInfo[64] = {0}, tmp[64] = {0};

	setlinktype(WLAN);
	//获取待测试的AP数量,对应配置文件中的APNUM
	if(GetParaInfo("APNUM",pInfo)!=SUCC)
		return;
	if(trim_string2int(pInfo,&ApNUM)!=SUCC)
		return;
	//增加从配置文件获取测试次数 20171101 add by jym
	if(GetParaInfo("CycleTime",pInfo)!=SUCC)
		return;
	if(trim_string2int(pInfo,&cnt)!=SUCC)
		return;
	//设置自动化测试标志,并控制测试次数为30 (次数从配置文件读)
	g_CycleTime = cnt;//30;
	g_conf_flag = 1;
	g_SequencePressFlag = 1;
	g_keeptime = 3;
	g_PackLen = 4096; //增加包的初始长度,以免测试时没有设置直接进行时使用的数据长度为0, 20180612 modify

	//设置服务器地址和端口
	if(conf_server()!=SUCC)
 		return;
	//设置socket协议
	if(conf_trans()!=SUCC)
		return;
	//低端需要先设置数据转发使用的通讯接口 20171114
#if  CPU5810X_ENABLE||defined ME50NX ||defined ME68 
	NDK_NetAddRouterTable(COMM_TYPE_WIFI,"0.0.0.0");//netsetting.SvrIP.Str
#endif
	for(i=1;i<=ApNUM;i++)
	{
		//设置AP链接参数:SSID,PWD,ENCMODE等
		if(use_wifi_conf(i)!=SUCC)//如果配置文件中删除了一个不存在的路由后可以继续往下执行20170927
			continue;
		
		//从配置文件获取DHCP标志
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "WifiDHCPenable%d", i);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return;
		if(trim_string2int(pInfo,&dhcpflag)!=SUCC)
			return;
		if(dhcpflag)//为1 使能 使用自动分配本地地址
		{
			netsetting.DHCPenable = ENABLE;
		}
		else//为0需要从配置文件中读取静态本地地址
		{
			//获取静态IP
			netsetting.DHCPenable = DISABLE;
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp, "WifiLocalIP%d", i);
			if(GetParaInfo(tmp,pInfo)!=SUCC)
				return;
			if(trim_string(pInfo,localip)!=SUCC)
				return;
			strcpy(netsetting.LocalIP.Str,localip);
			//获取子网掩码
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp, "WifiLocalMask%d", i);
			if(GetParaInfo(tmp,pInfo)!=SUCC)
				return;
			if(trim_string(pInfo,localmask)!=SUCC)
				return;
			strcpy(netsetting.LocalMask.Str,localmask);
			//获取网关
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp, "WifiLocalGatway%d", i);
			if(GetParaInfo(tmp,pInfo)!=SUCC)
				return;
			if(trim_string(pInfo,localgateway)!=SUCC)
				return;
			strcpy(netsetting.LocalGateway.Str,localgateway);
			netsetting.LocalIP.value.data = INET_ADDR(netsetting.LocalIP.Str);
			netsetting.LocalMask.value.data = INET_ADDR(netsetting.LocalMask.Str);
			netsetting.LocalGateway.value.data = INET_ADDR(netsetting.LocalGateway.Str);
			cls_show_msg1(1,"本地网络参数配置成功");
		}

		//服务器IP
		//socket协议
		
#if NEW_WIFI_ENABLE
		//保存全局参数到AP列表文件中,同时设置好列表
		if(set_addedapinfo_fromgloabvar()!= SUCC)
		{
			cls_show_msg1(10, "%s, line %d:AP列表设置失败!(%d)", __FILE__, __LINE__);
			return;
		}
#endif
		if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"将进行AP%d:%s的兼容性测试,其结果如下:", i, WlanEssid)==ESC)
			return;
		//wlan_comm_press();//原来的测试项是双向收发压力和连接+数传,现改成是连接+数传和建链压力
		cnnt_comm_press();//连接+数传
		wlan_updown_press();//建链压力
		wlan_ability3();//应开发需求使用更详细的性能测试项进行性能值采集 20171220
	}
	
	return;
}

//漫游用例
void wlan_roam()
{
	/*private & local definition*/
	uint h_tcp = -1;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	int i = 0,  timeout = SO_TIMEO, cnt = 0, num = 0, ret = 0;
	uint slen = 0, rlen = 0;
	Packet sendpacket;
#if NEW_WIFI_ENABLE
	ST_WIFI_AP_INFO succ_ap;
#else
	char szMac[64] = {0};
	int j=0;
#endif

	/*process body*/
	/*漫游环境:准备好多台路由器(四台左右)，设置为相同的AP和密码，放置于四个不同的地方，每两台路由器之间距离远一些，大概2-3米*/
	if((cls_show_msg1(30, "请确保已搭建好漫游环境，搭建好后按任意键进行测试，否则按ESC退出")) == ESC)
		return;
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	cnt = sendpacket.lifecycle;
	num = cnt;

	/*process body*/
	if(NetUp()!=SUCC)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:NetUp失败", __LINE__);
		return;
	}
	if(TransUp(&h_tcp)!=SUCC)
	{
			NetDown();
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransUp失败", __LINE__);
			return;
	}
	while(cnt--)
	{
		//修改为能够一直查看mac地址，不让屏幕刷新掉mac地址信息  20180302 sull modify
		NDK_ScrPrintf("通讯成功%d次，退出按ESC",i);
		NDK_KbHit(&ret);
		if(ret == ESC)
			break;
		//增加需求:要能够查看mac地址信息,中端无实时获取mac地址的接口，无法满足需求
#if NEW_WIFI_ENABLE
		memset(&succ_ap,0,sizeof(succ_ap));
		if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_CONNECT_AP,&succ_ap))!=NDK_OK) || strcmp(succ_ap.ssid,WlanEssid) )
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:连接上错误的AP(ret = %d, ap = %s)", __LINE__, ret, succ_ap.ssid);
			continue;
		}
		cls_printf("当前mac地址:%02x:%02x:%02x:%02x:%02x:%02x", succ_ap.mac.octet[0], succ_ap.mac.octet[1], succ_ap.mac.octet[2], succ_ap.mac.octet[3], succ_ap.mac.octet[4], succ_ap.mac.octet[5]);
#endif
		//不允许通讯过程中断
		if(update_snd_packet(&sendpacket)!=SUCC)
			continue;
		 //修改为能够一直查看mac地址，不让屏幕刷新掉mac地址信息  20180302 sull modify
		//发送数据
		if(((ret=NDK_TcpWrite(h_tcp, sendpacket.header, sendpacket.len, timeout, &slen))!=NDK_OK) || (slen != sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送数据失败(ret = %d, 实际%d, 预期%d)", __LINE__, i, ret, slen, sendpacket.len);
			TransDown(h_tcp);
			TransUp(&h_tcp);
			continue;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if(((ret=NDK_TcpRead(h_tcp, rbuf, sendpacket.len, timeout, &rlen))!=NDK_OK) || (rlen != sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收数据失败(ret = %d, 实际%d, 预期%d)", __LINE__, i, ret, rlen, sendpacket.len);
			TransDown(h_tcp);
			TransUp(&h_tcp);
			continue;
		}	
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次校验失败", __LINE__, i);
			TransDown(h_tcp);
			TransUp(&h_tcp);
			continue;
		}
		i++;
	}
	TransDown(h_tcp);
	NetDown();
	cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "wifi漫游通讯总次数%d次,成功%d次", num, i);
	return; 
}

void systest11(void)
{
	/*private & local definition*/
	int nKey = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_WLAN()!=SUCC)
			return;
		
		g_CycleTime = 100;
		//自动化测试内容
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行压力测试
		wlan_press();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	else
	{
		while (1)
		{
			nKey = cls_show_msg("1.压力 2.性能\n"
							    "3.异常 4.兼容性\n"
							    "5.漫游 0.配置\n");
			switch(nKey)
			{
			case '1':
				wlan_press();
				break;
			case '2':
				wlan_ability();
				break;
			case '3':
				wlan_abnormity();
				break;
			case '4':
				AP_Compatibility_test();
				break;
			case '5':
				wlan_roam();
				break;
			case '0':
				switch(conf_conn_WLAN())
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
				cls_show_msg("请将测试用AP的DHCP分别设置成禁止和开启，分别使用静态和动态连接应该能够成功!");
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

