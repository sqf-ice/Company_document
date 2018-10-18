
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 测试程序层次函数
* file name		: layer.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20101217
* directory 		: .\SRC\main\
* description		: 
* related document	: 
* 
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	MAXTIMEOUT		(60)		//for UP/DOWN的超时时间
#define	TIME4KEEPSTATE	(2)		//确认状态保持的次数,不需要多次确认时,将此值设为1即可
#define	MAXTIMEOUT_PING	(5)

#define SCERTF  "cacert.pem"

#define WM_PPP_MSGSTRLEN		16
#define PPP_HANDLE_16300	1
#define WM_PPP_STATE_SUM_16300	9
#define WM_PPP_ERROR_SUM	5

enum{
	WM_PPP_NULL_16300,			//初始态
#define WM_PPP_ERROR_STARTSTATE	WM_PPP_NULL_16300	
	WM_PPP_CONNECT,
	WM_PPP_PROTOCOL,
#define WM_PPP_ERROR_STOPSTATE	WM_PPP_PROTOCOL		//含该状态		
	WM_PPP_WELCOME,
	WM_PPP_USERNAME,
	WM_PPP_PASSWORD,
	WM_PPP_MODE,
	WM_PPP_ENTHERNET,
	WM_PPP_SYSTEM,
	WM_PPP_IPMTU,				//9种正常状态
	WM_PPP_ERROR_NO_DIALTONE,
	WM_PPP_ERROR_NO_CARRIER,
	WM_PPP_ERROR_BUSY,
	WM_PPP_ERROR_UNUMBER,
	WM_PPP_ERROR_PRONONE		//5种出错状态
};

typedef struct {
	unsigned int state;
	unsigned char statestr[WM_PPP_MSGSTRLEN];
}pppdial_state;

typedef struct {
	unsigned int state_sum;
	unsigned int state;
	unsigned int error_start;
	unsigned int error_stop;
	unsigned int error_sum;
	pppdial_state *pstate_str;
	pppdial_state *perror_str;
}pppdialstatetype;

/*------------global variables definition-----------------------*/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
static uint g_socket = 0;//用于关闭时使用
sock_t g_socktype = SOCK_DEFAULT;//用于关闭时使用
#endif
//用全局变量模拟实现资源互斥
#if 0	//SDLC_ENABLE||ASYN_ENABLE
static ModemStatus	mdmstatus	= INITIAL;
#endif
//以下两变量实际上也是4个内部"网络"函数是否调用过的标志
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
/*static*/ LinkStatus linkstatus 	= LINKDOWN;
/*static*/ NetStatus netstatus	= NETDOWN;
static TransStatus	transstatus	= TRANSDOWN;
NetworkSetting netsetting;
int TCPRESETFLAG = FALSE;//tcp 强制挂断标志位，默认协议挂断 20140422 linwl
#endif
LinkSetting linksetting;

/*---------------functions declaration--------------------------*/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
static int __LinkDown(void);
#endif

/*---------------functions definition---------------------------*/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
int serial_clrportbuf_all(EM_PORT_NUM port)
{
	/*private & local definition*/
	//int ret = 0;

	/*process body*/
	while(NDK_PortTxSendOver(port)!=NDK_OK);//总有发完的时候//ret = clrportbuf(port, 0);
	//ret1 = clrportbuf(port, 1);
	return NDK_PortClrBuf(port);//return (ret==SUCC && ret1==SUCC)?SUCC:FAIL;
}
#endif

void init_linksetting(void)
{
	/*private & local definition*/
	
	/*process body*/
	memset(&linksetting, 0, sizeof(linksetting));

	//逐一初始化是通用方法
#if defined ME11||defined ME15||defined ME20||defined ME15CHX||defined ME15B||defined ME66
	if(COMNUM==0)
		g_com = PORT_NUM_USB;//ME11需初始化成USB串口，否则打开0xff的串口会引起USB串口工作异常 20140520 
	else
		g_com = PORT_NUM_COM1;//测试USB串口,使用 物理串口
#elif ANDIROD_ENABLE||defined ME15C //||defined N900
		g_com = PORT_NUM_COM2;//N900只有COM2
//#elif defined IM81AZ
//		g_com = PORT_NUM_COM1;//IM81AZ有COM1
#else
	g_com = INVALID_COM;//zhangnw20060410 add
#endif
	send_flag = TRUE;//zhangnw20060831 add for 是否上送的标志
	setlinktype(NONE);//linktype = NONE;
	
	strcpy(BtPin, DEFAULT_BT_PIN);
	strcpy(BtName, DEFAULT_BT_NAME);
#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)//ME15C为节省应用空间不编译不用的函数
	strcpy(g_bps, DEFAULT_BPS);//g_bps = DEFAULT_BPS;
	MDMPatchType = MDM_PatchType5;
	strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);
	strcpy(WLMAPN, DEFAULT_WLM_APN);
	strcpy(PPPUSR, DEFAULT_PPP_USR);
	strcpy(PPPPWD, DEFAULT_PPP_PWD);
#if DOUBLESIM_ENABLE
	SIMSLOT = SIM_SLOT1;
#endif
	g_devicecom = PORT_NUM_COM1;
	strcpy(g_devicecombps, DEFAULT_BPS);//g_devicecombps = DEFAULT_BPS;
	WlanEncMode = 0;//NOENCRYPT;
	WlanKeyType = 0;//NOKEY;
	strcpy(WlanPwd, DEFAULT_WLAN_PWD);
	strcpy(WlanEssid, DEFAULT_WLAN_ESSID);
	Wlan8021XEapMode = 0;
	strcpy(Wlan8021XIdentity, DEFAULT_WLAN_8021XIDENTITY);	

	//wifi mac和设置标志位初始化
	memset(WifiMac, 0, sizeof(WifiMac));//初始化成不使用MAC
	WifiMacFlag = 0;

	//wifi ap 新增初始化全局参数
	WifiApShareDev = 0;//WIFIAP_SHARE_DEV_ETH;
	WifiApSecMode = 4;//WIFI_NET_SEC_WPA2;
	strcpy(WifiApSsid, DEFAULT_WLANAP_SSID);
	WifiApHidden = 0;
	strcpy(WifiApKey, DEFAULT_WLANAP_KEY);
#endif	
	return;
}

//linktype数据的操作方法
void setlinktype(LinkType type){linktype = type;}
LinkType getlinktype(void){return linktype;}

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)//ME15C为节省应用空间不编译不用的函数
void init_netsetting(void)
{
	/*private & local definition*/
	
	/*process body*/
	memset(&netsetting, 0, sizeof(netsetting));

	//逐一初始化是通用方法
	//zhangnw201105modify 改用INET_ADDR,避免考虑字节序的问题
	strcpy(netsetting.SvrIP.Str, DEFAULT_SVR_IP_STR);
	netsetting.SvrIP.value.data = INET_ADDR(DEFAULT_SVR_IP_STR);//DEFAULT_SVR_IP_VLE;
	netsetting.SvrTcpPort = DEFAULT_SVR_TCP_PORT;
	netsetting.SvrUdpPort = DEFAULT_SVR_UDP_PORT;
	strcpy(netsetting.LocalIP.Str, DEFAULT_LOCAL_IP_STR);
	netsetting.LocalIP.value.data = INET_ADDR(DEFAULT_LOCAL_IP_STR);//DEFAULT_LOCAL_IP_VLE;
	netsetting.LocalPort = 0;
	strcpy(netsetting.LocalMask.Str, DEFAULT_LOCAL_MASK_STR);
	netsetting.LocalMask.value.data = INET_ADDR(DEFAULT_LOCAL_MASK_STR);//DEFAULT_LOCAL_MASK_VLE;
	strcpy(netsetting.LocalGateway.Str, DEFAULT_LOCAL_GATEWAY_STR);
	netsetting.LocalGateway.value.data = INET_ADDR(DEFAULT_LOCAL_GATEWAY_STR);//DEFAULT_LOCAL_GATEWAY_VLE;
	netsetting.socktype = SOCK_NDKTCP;
#if 0
	netsetting.SSLHsOpt = HANDSHAKE_SSLv2;
	netsetting.SSLAuthOpt = SSL_AUTH_NONE;
	netsetting.SSLFileFmt = FILE_PEM;
#endif
	//维护一张与下层一致的DNS表
	strcpy(netsetting.DNSHost[0], DNS1);
	strcpy(netsetting.DNSHost[1], DNS2);
	strcpy(netsetting.DNSHost[2], DNS3);
	netsetting.DHCPenable = DISABLE;

	//wifi ap 新增ip
	strcpy(netsetting.WifiApIp.Str, DEFAULT_WIFIAP_IP_STR);
	netsetting.WifiApIp.value.data = INET_ADDR(DEFAULT_WIFIAP_IP_STR);
	return;
}

//以下2函数在需要时可以把状态置为DOWN,一般情况下不需要使用
void set_linkstatus_down(void){linkstatus = LINKDOWN;}
void set_netstatus_down(void){netstatus = NETDOWN;}

ENDIAN_t GetCurrentEndianType(void)
{
#if 0
	/*private & local definition*/
	Value value;

	/*process body*/
	value.data = 0x01;
	return (ENDIAN_t)((value.asc[0]==0x01)?LITTLEENDIAN:BIGENDIAN);
#else
	return ENDIAN;
#endif
}

#if 0
ulong INET_ADDR(char *a)
{
	/*private & local definition*/
	char *p = a;
	uchar data[4] = {0};
	int cnt = 0;

	/*process body*/
	for(; cnt<sizeof(data); cnt++)
	{
		data[cnt] = atoi(p);
		if((p=strchr(p, '.'))==NULL&&cnt!=3)
			return 0;
		else
			p++;
	}
	
	if(LITTLEENDIAN==GetCurrentEndianType())
		return ((data[0]<<24)+(data[1]<<16)+(data[2]<<8)+data[3]);
	else
		return ((data[3]<<24)+(data[2]<<16)+(data[1]<<8)+data[0]);
}
#else //使用NDK接口实现
#if !K21_ENABLE
ulong INET_ADDR(char *a)
{
	/*private & local definition*/
	ulong ret = 0;

	/*process body*/
	return (NDK_PppAddrChange(a, &ret)==NDK_OK)?ret:0;
}
#else
ulong INET_ADDR(char *a){return 0;}
#endif
#endif

void INET_NTOA(ulong n, char *a)
{
	/*private & local definition*/

	/*process body*/
	if(LITTLEENDIAN==GetCurrentEndianType())
		sprintf(a, "%u.%u.%u.%u", *((uchar *)&n+3), *((uchar *)&n+2), *((uchar *)&n+1), *((uchar *)&n+0));
	else
		sprintf(a, "%u.%u.%u.%u", *((uchar *)&n+0), *((uchar *)&n+1), *((uchar *)&n+2), *((uchar *)&n+3));
}

unsigned short HTONS(unsigned short data)
{
	/*private & local definition*/
	
	/*process body*/
#if 0	//defined GP710
	if(netsetting.socktype==SOCK_STTCP||netsetting.socktype==SOCK_STUDP)
		return htons(data);
	else
		return data;
#else
	return data;
#endif
}

unsigned long HTONL(unsigned long data)
{
	/*private & local definition*/
	
	/*process body*/
#if 0
	return htonl(data);
#else
	return data;
#endif
}

void create_0203pack(char *buf, int tlen)
{
	/*private & local definition*/

	/*process body*/
	buf[0] = STX;//0x02;
	buf[1] = Data2BCD(tlen/100);
	buf[2] = Data2BCD(tlen%100);
	buf[tlen+3] = ETX;//0x03;
	buf[tlen+4] = MakeLRC((uchar *)buf+1, tlen+3);//从len域到0x03开始算LRC
}

#if SDLC_ENABLE||ASYN_ENABLE
//设计int型返回值,可包含2种意义
int mdm_detectCONNECT(void)
{
	/*private & local definition*/
	EM_MDMSTATUS sta = 0; 
	int times = 0, ret = 0;
	time_t diff = 0, oldtime = 0;

	/*process body*/
	oldtime = time(NULL);
	sleep(3);//让MDM状态稳定后再取也不迟,否则像GP710拔后直接取状态可能拔不上

	cls_printf("检测MODEM状态中,请稍候...");
	while(1)//do
	{
		if ((ret=NDK_MdmCheck(&sta))!=NDK_OK||sta<MDMSTATUS_NORETURN_AFTERPREDIAL||(diff=time(NULL)-oldtime)>MAXTIMEOUT)//NORETURN_AFTERPREDIAL:0
		{
			//无论ret如何都挂断一下if(ret>=NORETURN_AFTERPREDIAL)
			//NDK_MdmHangup();//外部调用者自己做
			break;//return ret;
		}
		show_stopwatch(ENABLE, diff);
		if(sta==MDMSTATUS_CONNECT_AFTERPREDIAL)//CONNECT_AFTERPREDIAL:2
		{
			if(++times==TIME4KEEPSTATE)
				break;
			else
				NDK_SysDelay(10);//稳定一段时间再看看
		}
	}//while(ret!=CONNECT_AFTERPREDIAL);//CONNECT_AFTERPREDIAL:2
	
	show_stopwatch(DISABLE, 0);
	return (ret==NDK_OK)?sta:ret;//预期返回CONNECT_AFTERPREDIAL //此处不宜返回SUCC的,与NORETURN_AFTERPREDIAL等值
}

static int mdm_dial_nocheck(char *phonenum)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type = getlinktype();

	/*process body*/
	if(type==SYNC||type==ASYN)
		cls_printf("%s步猫拨%s中...\n", (type==SYNC)?"同":"异", phonenum);//封装函数带上提示信息以区别于原始API
	else
		return FAIL;
	
	if((ret=NDK_MdmDial(phonenum))!=NDK_OK)//内部目前只使用TONE方式,而不使用PLUS方式的
	{
		cls_show_msg1(10, "%s, line %d:NDK_MdmDial失败!(%d)", __FILE__, __LINE__, ret);
		//NDK_MdmHangup();//外部调用者自己做
		return ret;//FAIL;
	}
	
	return SUCC;
}

int mdm_dial(char *phonenum)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type = getlinktype();

	/*process body*/
	if(type==SYNC||type==ASYN)
		cls_printf("%s步猫拨%s中...\n", (type==SYNC)?"同":"异", phonenum);//封装函数带上提示信息以区别于原始API
	else
		return FAIL;
	
	if((ret=NDK_MdmDial(phonenum))!=NDK_OK)//内部目前只使用TONE方式,而不使用PLUS方式的
	{
		cls_show_msg1(10, "%s, line %d:NDK_MdmDial失败!(%d)", __FILE__, __LINE__, ret);
		//NDK_MdmHangup();//外部调用者自己做
		return FAIL;
	}

	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		cls_show_msg1(10, "%s, line %d:检测到异常状态(%d)", __FILE__, __LINE__, ret);
		return FAIL;
	}
	
	return SUCC;
}

int mdm_hangup(void)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type = getlinktype();

	/*process body*/
	if(type==SYNC||type==ASYN)
		cls_printf("%s步猫挂机中...\n", (type==SYNC)?"同":"异");//封装函数带上提示信息以区别于原始API
	else
		return FAIL;
	
	ret = NDK_MdmHangup();
	if(type==SYNC)
		sleep(SLEEP_SDLC_HANGUP);//这里的延时不是因为POS本身的需要,而是测试设备(同步网控器)挂断慢,是给它的时间

	return ret;//FAIL;
}
#else
int mdm_detectCONNECT(void){return MDMSTATUS_NOPREDIAL;}
//static int mdm_dial_nocheck(char *phonenum){return FAIL;}
int mdm_dial(char *phonenum){return FAIL;}
int mdm_hangup(void){return FAIL;}
#endif

#if SDLC_ENABLE
//修改SDLC包的前5字节的包头
void update_SDLC_packet(char *buf, int len)//包长暂无用,留作扩展,形式与也update_ASYN_packet统一
{
	/*private & local definition*/
	
	/*process body*/
	if(len<SDLCPCKTHEADERLEN)
	{
		cls_show_msg1(10, "%s, line %d:包长不能小于%d!", __FILE__, __LINE__, SDLCPCKTHEADERLEN);
		return;
	}
	
	memcpy(buf, SDLCPCKTHEADER, SDLCPCKTHEADERLEN);//TPDU头
	return;
}

int __LinkUp_SDLC(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	cls_printf("建立同步拔号连接中...\n");

	//初始化
#if defined SP50||defined SP80
	if(sdlc_init_flag)
	{
		if((ret=NDK_MdmSdlcSingleForceInit(MDMPatchType, g_sdlc_bps))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:同步MODEM初始化失败!", __FILE__, __LINE__);
			return ret;
		}
	}
	else
#endif
	{
		if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:同步MODEM初始化失败!", __FILE__, __LINE__);
			return ret;
		}
	}
	NDK_MdmClrbuf();
	
	//拨号
	if((ret=mdm_dial(MDMDialStr))!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:建立同步拔号(ATDT%s)连接失败!", __FILE__, __LINE__, MDMDialStr);
		mdm_hangup();
		return ret;//FAIL;
	}
	NDK_MdmClrbuf();//sdlc_clrportbuf_all();

	linkstatus = LINKUP;
	return SUCC;
}

int __LinkDown_SDLC(void)
{
	/*private & local definition*/

	/*process body*/
	linkstatus = LINKDOWN;//无论成功与否,链路回到邋邋DOWN态
	cls_printf("断开同步拔号连接中...\n");
	return mdm_hangup();
}

static int __NetUp_SDLC(void)
{
	/*private & local definition*/
	
	/*process body*/
	//cls_printf("建立同步网络中...\n");
	cls_show_msg1(10, "%s, line %d:建立同步网络失败!", __FILE__, __LINE__);
	return FAIL;	
}

static int __NetDown_SDLC(void)
{
	/*private & local definition*/
	
	/*process body*/
	//cls_printf("断开同步网络中...\n");
	cls_show_msg1(10, "%s, line %d:断开同步网络失败!", __FILE__, __LINE__);
	return FAIL;
}
#else
void update_SDLC_packet(char *buf, int len){}
int __LinkUp_SDLC(void){return FAIL;}
int __LinkDown_SDLC(void){return FAIL;}
static int __NetUp_SDLC(void){return FAIL;}
static int __NetDown_SDLC(void){return FAIL;}
#endif

#if ASYN_ENABLE
#if 1//!( /*defined NL8510GP||*/defined GP730)730与8510GP同步
//拼命探测接收缓冲,直到不再读到新数据(读长度稳定)
//异常时才返回FAIL,其它情况下,返回>=0
int getmodemreadlenN(void)
{
	/*private & local definition*/
	int ret = 0, oldlen = 0;
	uint rdlen = 0;
	time_t oldtime = 0, interval = 2;//interval = (mdmstatus==PREDIALED?40:2);//两次接收间最大间隔时间;

	/*process body*/
	oldtime = time(NULL);
	while (1)
	{
		if((ret=NDK_MdmGetreadlen(&rdlen))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:接收缓冲区异常(%d)!", __FILE__, __LINE__, ret);
			return ret;//FAIL;
		}
		
		if (oldlen<rdlen)//有读到新数据,更新oldlen,并重新计时
		{
			oldlen = rdlen;
			oldtime = time(NULL);
		}
		else if(oldlen==rdlen)
		{
			//各个型号POS需要的拨号时间E80:35 nl8200:30 nl8510:30
			if (time(NULL)-oldtime>interval)//XXs内都读到一样长数据,认为稳定了,可以read了
				return rdlen;
		}
		else//oldlen>ret,??接收缓冲出现问题了！
		{
			cls_show_msg1(10, "%s, line %d:接收缓冲区异常!", __FILE__, __LINE__);
			return FAIL;
		}
		
		if (lib_kbhit()==ESC)//允许探测过程被中断
			return FAIL;
	}
}
#else
int getmodemreadlenN(void){return ASYNPCKTMAXLEN;}
#endif

//格式(与后台配合): 0x02(header, 1byte)+len(2bytes, BCD)+TPDU header(5bytes)+data(不定长)+0x03(tail, 1byte)+LRC(1byte)
//后台以%x格式显示
void update_ASYN_packet(char *buf, int len)
{
	/*private & local definition*/
	
	/*process body*/
	if(len<ASYNPCKTHEADERLEN)
	{
		cls_show_msg1(10, "%s, line %d:包长不能小于%d!", __FILE__, __LINE__, ASYNPCKTHEADERLEN);
		return;
	}
	
	create_0203pack(buf, len-ASYNPCKTHEADERLEN);
	return;
}

int ASYNDial4PPP(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	cls_printf("建立异步拔号连接中...\n");

	//初始化
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:异步MODEM初始化失败!", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	NDK_MdmClrbuf();

	//拨号
	//注意此拨号是普通链路的拨号非PPP拨号,PPP拨号后可以到网络层而普通链路的拨号不行
	if((ret=mdm_dial_nocheck(MDMDialStr))!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:建立异步拔号(ATDT%s)连接失败!", __FILE__, __LINE__, MDMDialStr);
		mdm_hangup();
		return ret;//FAIL;
	}
	//NDK_MdmClrbuf();//Asyn_clrportbuf_all();

	return SUCC;
}

int __LinkUp_ASYN(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
#if 1
	cls_printf("建立异步拔号连接中...\n");

	//初始化
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:异步MODEM初始化失败!(ret=%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;
	}
	NDK_MdmClrbuf();

	//拨号
	//注意此拨号是普通链路的拨号非PPP拨号,PPP拨号后可以到网络层而普通链路的拨号不行
	if((ret=mdm_dial(MDMDialStr))!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:建立异步拔号(ATDT%s)连接失败!", __FILE__, __LINE__, MDMDialStr);
		mdm_hangup();
		return ret;//FAIL;
	}
	NDK_MdmClrbuf();//Asyn_clrportbuf_all();
#else
	if((ret=ASYNDial())!=SUCC)
		return ret;
#endif

	linkstatus = LINKUP;
	return SUCC;
}

int __LinkDown_ASYN(void)
{
	/*private & local definition*/

	/*process body*/
	linkstatus = LINKDOWN;//无论成功与否,链路回到邋邋DOWN态
	cls_printf("断开异步拔号连接中...\n");
	return mdm_hangup();
}

/////////////////////////////说明:以下关于异步PPP的函数移植于NL8510GP的自检(收于yanm)///////////////////////////////
//字符串扫描函数
//返回串所在位置
static int findstr(const unsigned char * pstr, unsigned char * pbuf, int *buflen)
{
	unsigned int i,tmplen;
	char *pfirst3str,*pstris;
	char tmpstr[4];
	
	//查找前三个字节
	memcpy(tmpstr,pstr,3);
	tmpstr[3]=0;
	pfirst3str=strstr((char *)pbuf,tmpstr);
	//核对整个串
	if(pfirst3str!=NULL)
	{
		pstris=strstr((char *)pbuf,(char *)pstr);
		if(pstris!=NULL)
		{
			//找到了,返回串的启示位置
//			return pfirst3str-pbuf;
			if((unsigned int)pstris==(unsigned int)pbuf)
				return TRUE;
			tmplen=strlen(pstris);
			for(i=0;i<tmplen;i++)
			{
				pbuf[i]=pstris[i];
			}
			pbuf[tmplen]=0;
			*buflen=tmplen;
			return TRUE;
		}
		else
		{
			//有第一个字节则对缓冲进行调整????
			if(strlen((char *)pfirst3str)<strlen((char *)pstr))
				return FALSE;
			tmplen=strlen(pfirst3str);
			for(i=0;i<tmplen;i++)
			{
				pbuf[i]=pfirst3str[i];
			}
			pbuf[tmplen]=0;
			*buflen=tmplen;
//			return pfirst3str-pbuf;
		}
	}
	return FALSE;
}

#define STEPLEN	1
int WM_PPP_16300_DIAL(void)
{
	pppdial_state WM_PPP_STATESTR_16300[WM_PPP_STATE_SUM_16300+1]={
	{WM_PPP_CONNECT,"CONNECT"},
//	{WM_PPP_PROTOCOL,"PROTOCOL"},
	{WM_PPP_WELCOME,"welcome!!"},
	{WM_PPP_USERNAME,"username:"},
	{WM_PPP_PASSWORD,"password:"},
	{WM_PPP_MODE,"PPP mode.\r\n"},
	{WM_PPP_ENTHERNET,"(Ethernet0)\r\n"},
	{WM_PPP_SYSTEM,"system.\r\n"},
	{WM_PPP_IPMTU,"bytes\r\n"}
	};

	pppdial_state WM_PPP_ERRSTR_16300[WM_PPP_STATE_SUM_16300+1]={
	{WM_PPP_ERROR_NO_DIALTONE,"NO DIALTONE"},
	{WM_PPP_ERROR_NO_CARRIER,"NO CARRIER"},
	{WM_PPP_ERROR_BUSY,"BUSY"},
	{WM_PPP_ERROR_UNUMBER,"NUMBER"},
	{WM_PPP_ERROR_PRONONE,"PROTOCOL:NONE"}
	};

	unsigned char databuffer[1024];
	unsigned int datalen,olddatalen;
	int ret;
	uint len;
	unsigned int flag=0,j,errcnt,normalcnt;
	pppdialstatetype WM_PPP_dialstate;

	//初始化
	WM_PPP_dialstate.state_sum=WM_PPP_STATE_SUM_16300;
	WM_PPP_dialstate.state=WM_PPP_NULL_16300;
	WM_PPP_dialstate.error_start=WM_PPP_ERROR_STARTSTATE;
	WM_PPP_dialstate.error_stop=WM_PPP_ERROR_STOPSTATE;
	WM_PPP_dialstate.error_sum=WM_PPP_ERROR_SUM;
	WM_PPP_dialstate.pstate_str=WM_PPP_STATESTR_16300;
	WM_PPP_dialstate.perror_str=WM_PPP_ERRSTR_16300;
	//循环处理
	datalen=olddatalen=0;
	normalcnt=errcnt=0;
	while(!flag)
	{
	//获取缓冲区数据
		if(datalen<(sizeof(databuffer)-STEPLEN-1))
		{
			olddatalen=datalen;

			len = STEPLEN;
			if((ret=NDK_MdmRead((char *)(databuffer+datalen),&len,(WM_PPP_CONNECT==(WM_PPP_dialstate.pstate_str)[normalcnt].state)?30:3))!=NDK_OK)
			{
				cls_show_msg1(10, "%s, line %d:读取数据失败(%d)!", __FILE__, __LINE__, ret);
				return FAIL;
			}
			if(len!=STEPLEN)
			{
				cls_printf("数据读取已经结束");
				flag=1;	//处理完剩余数据后应当中断循环
			}
			datalen+=len;
		}
		else
		{
			cls_show_msg1(10, "%s, line %d:临时缓冲区已满!", __FILE__, __LINE__);
			return FAIL;			
		}
		//扫描数据如果发现非ASCII码数据则报错，或只扫描前一段的有效数据
		for(j=olddatalen;j<datalen;j++)
		{
			if(databuffer[j]&0x80)
			{
				cls_show_msg1(10, "%s, line %d:发现非ASCII码!", __FILE__, __LINE__);
				flag=1;
				break;
			}
		}
		datalen=j;
		databuffer[datalen]=0;
		//分析数据，判断状态
		//正常数据分析
		ret=findstr((WM_PPP_dialstate.pstate_str)[normalcnt].statestr,databuffer,(int *)&datalen);
		if(TRUE==ret)
		{
			flag=0;
#if 0
			cls_printf("match(%d):[%d]\n[%s]\n[%s]\n",normalcnt
				,(WM_PPP_dialstate.pstate_str)[normalcnt].state
				,(WM_PPP_dialstate.pstate_str)[normalcnt].statestr,databuffer);
#endif
			switch((WM_PPP_dialstate.pstate_str)[normalcnt].state)
			{
			case WM_PPP_USERNAME:
				NDK_MdmWrite("16300\r", strlen("16300\r"));//modemportwrite("16300\r",6);
//				modemportwrite("yp0039@vpdn.zp\r",15);
				break;
			case WM_PPP_PASSWORD:
				NDK_MdmWrite("16300\r", strlen("16300\r"));//modemportwrite("16300\r",6);
//				modemportwrite("5gf34g\r",7);
				break;
			case WM_PPP_IPMTU:
				flag=1;
				break;
			default:
				break;
			}
			normalcnt++;
		}
		//错误数据分析
		if((normalcnt>=WM_PPP_dialstate.error_start)&&(normalcnt<=WM_PPP_dialstate.error_stop))
		{
			for(errcnt=0;errcnt<WM_PPP_dialstate.error_sum;errcnt++)
			{
				ret=findstr((WM_PPP_dialstate.perror_str)[errcnt].statestr,databuffer,(int *)&datalen);
				if(TRUE==ret)
				{
					flag=1;
#if 0
					cls_printf("ERROR match:\n[%s]\n",(WM_PPP_dialstate.perror_str)[errcnt].statestr);//,databuffer
#endif
					if(WM_PPP_ERROR_PRONONE==WM_PPP_dialstate.perror_str[errcnt].state)
					{
#if 0
						cls_printf("协商错重新建链\n请稍候...");
						msdelay(8000);
#else
						cls_show_msg1(10, "%s, line %d:协商错误(%d)!", __FILE__, __LINE__, WM_PPP_dialstate.perror_str[errcnt].state);
#endif
					}
					return -WM_PPP_dialstate.perror_str[errcnt].state;
				}
			}
		}

	//出错退出
	}

	return	WM_PPP_dialstate.state;
}
#else
int getmodemreadlenN(void){return FAIL;}
void update_ASYN_packet(char *buf, int len){}
int ASYNDial4PPP(void){return FAIL;}
int __LinkUp_ASYN(void){return FAIL;}
int __LinkDown_ASYN(void){return FAIL;}
//static int findstr(const unsigned char * pstr, unsigned char * pbuf, int *buflen){return FAIL;}
int WM_PPP_16300_DIAL(void){return FAIL;}
#endif

#if WLM_ENABLE && PPP_ENABLE
int __LinkUp_WLM(void)
{
	/*private & local definition*/
	int ret = 0;
	EM_WLM_STATUS emStatus = -1;
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:	
		//建立GPRS,CDMA连接
		cls_printf("建立%s连接中...\n", (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"));
		if ((ret = NDK_WlInit(2000,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
		{
			cls_show_msg1(10,"%s, line %d:无线初始化失败(%d,%d)", __FILE__, __LINE__, ret, emStatus);
			return FAIL;
		}
		break;
	default:
		return FAIL;
		break;
	}

	linkstatus = LINKUP;
	return SUCC;
}

int __LinkDown_WLM(void)
{
	/*private & local definition*/
	int err = 0, ret = 0;
	LinkType type = getlinktype();

	/*process body*/
	linkstatus = LINKDOWN;//无论成功与否,链路回到邋邋DOWN态
	
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:	
		cls_printf("断开%s连接中...\n", (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"));

#if 0
		printf("请等待40秒...");
		sleep(40);//静置30秒,让移动后台自动释放链路//参数略大,保险
#else
		if((ret=WaitPPPState(PPP_STATUS_DISCONNECT, &err))!=SUCC)
		{
			cls_show_msg1(10, "%s, line %d:断开%s失败!(%d)", __FILE__, __LINE__, (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"), err);
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
			NDK_PppHangup(1);//NetPPPHangup(1);
#else
			NDK_PppHangup(0); //非阻塞挂断
#endif
			//linkstatus = LINKDOWN;
			return ret;//FAIL;
		}
#endif

		break;
	default:
		return FAIL;
		break;
	}

	//linkstatus = LINKDOWN;
	return SUCC;
}
#else
int __LinkUp_WLM(void){return FAIL;}
int __LinkDown_WLM(void){return FAIL;}
#endif

#if ETH_ENABLE
int __LinkUp_ETH(void)
{
	/*private & local definition*/

	/*process body*/
	cls_printf("建立以太网连接中...\n");

	linkstatus = LINKUP;
	return SUCC;
}

int __LinkDown_ETH(void)
{
	/*private & local definition*/

	/*process body*/
	cls_printf("断开以太网连接中...\n");
	NDK_EthDisConnect();//20151015 yanggf建议下增加的
	
	linkstatus = LINKDOWN;//以太网真正关闭才置LINKDOWN状态
	return SUCC;
}

int __NetUp_ETH(void)
{
	/*private & local definition*/
	int ret = 0;
	char LocalIp[16] = {0}, LocalMask[16] = {0}, LocalGateway[16] = {0}, DNS[16*3+2/*16*/] = {0}; //文档说明一次性取出所有DNS,之间以分号符';'隔开故需要开大缓冲区 20180322

	/*process body*/
	cls_printf("建立以太网网络中...\n");

	if(netsetting.DHCPenable==DISABLE)
	{
		if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, netsetting.DNSHost[1]))!=NDK_OK)
		{                                                                                            
			cls_show_msg1(10, "%s, line %d:设置静态地址失败(%d)", __FILE__, __LINE__, ret);
			return ret;//FAIL;
		}
	}

	if(netsetting.DHCPenable==ENABLE)
	{
		if((ret=NDK_NetDHCP())!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:启用DHCP失败(ret=%d)", __FILE__, __LINE__, ret);
			return ret;//FAIL;	
		}
		
		//进一步申请动态IP(可能一次取不到,多取几次)    
		if((ret=lib_GetLocalAddr(LocalIp, LocalMask, LocalGateway, DNS))!=SUCC)//if((LocalIp=GetLocalAddr())==0)
		{//先获取出错时的错误码,再关闭
			cls_show_msg1(10, "%s, line %d:获取动态IP失败", __FILE__, __LINE__);
			return ret;//FAIL;
		}

		//将获取到地址信息串转为ul数值同步到全局变量中(layer相关的全局变量一般应在配置时就填充和同步,在xxxUp等中只作为右值;动态获取是个例外,部分变量需要作为左值进行更新)
		strcpy(netsetting.LocalIP.Str, LocalIp);
		netsetting.LocalIP.value.data = INET_ADDR(LocalIp);
		strcpy(netsetting.LocalMask.Str, LocalMask);
		netsetting.LocalMask.value.data = INET_ADDR(LocalMask);
		strcpy(netsetting.LocalGateway.Str, LocalGateway);
		netsetting.LocalGateway.value.data = INET_ADDR(LocalGateway);
		//DNS呢?
	}

	netstatus = NETUP;
	return SUCC;
}

int __NetDown_ETH(void)
{
	/*private & local definition*/
	int ret = 0;
	char LocalIp[16] = "1.0.0.1", LocalMask[16] = "255.0.0.0", LocalGateway[16] = "1.0.0.254", DNS[16] = "0.0.0.0";

	/*process body*/
	cls_printf("断开以太网网络中...\n");

	//置本地IP为无效值,同时调EthClose切断所有网络连接
	if ((ret=NDK_EthSetAddress(LocalIp, LocalMask, LocalGateway, DNS))!=NDK_OK)
	{                                                                                            
		cls_show_msg1(10, "%s, line %d:设置静态地址失败(%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;
	}

#if 0 //断开只置IP为无效值,不能同步到全局设置中,否则会给下一次NetUp及配置带来不便和问题
	strcpy(netsetting.LocalIP.Str, LocalIp);
	netsetting.LocalIP.value.data = INET_ADDR(LocalIp);
	strcpy(netsetting.LocalMask.Str, LocalMask);
	netsetting.LocalMask.value.data = INET_ADDR(LocalMask);
	strcpy(netsetting.LocalGateway.Str, LocalGateway);
	netsetting.LocalGateway.value.data = INET_ADDR(LocalGateway);
#endif

	if((ret=__LinkDown_ETH())!=SUCC)//if(__LinkDown(type)!=SUCC)
	{
		netstatus = NETDOWN;
		return ret;//FAIL;
	}

	netstatus = NETDOWN;
	return SUCC;
}
#else
int __LinkUp_ETH(void){return FAIL;}
int __LinkDown_ETH(void){return FAIL;}
int __NetUp_ETH(void){return FAIL;}
int __NetDown_ETH(void){return FAIL;}
#endif

#if WLAN_ENABLE
int __LinkUp_WLAN(void)
{
	/*private & local definition*/
	int ret = 0;
#if NEW_WIFI_ENABLE
	ST_WIFI_STATUS status;
#else
	ST_WIFI_8021x_PARAM init2; //= {0}去除警告
	ST_WIFI_PARAM init = {0};
	EM_WPA_CONSTATE state = 0;
	time_t diff = 0, oldtime = 0;
#endif

	/*process body*/
#if NEW_WIFI_ENABLE
	/*if((ret=NDK_WifiStatus(&status))!=NDK_OK)
	{
		cls_show_msg1(5, "%s, line %d:获取%s状态失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		return FAIL;
	}*/
	NDK_WifiStatus(&status);
	if(status.status == WIFI_WLAN_STATUS_SUCC)
	{
		linkstatus = LINKUP;
		return SUCC;
	}
	cls_printf("接入AP(%s)中...\n", WlanEssid);
	//调用start函数
	if((ret=NDK_WifiStart())!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:启动连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		NDK_WifiStop();
		return FAIL;
	}
	//判断连接成功状态
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, WlanEssid) )
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接%s超时失败!(ret=%d,status=%d,ssid=%s)", __FILE__, __LINE__, WlanEssid, ret, status.status, status.ssid);
		NDK_WifiStop();
		return FAIL;
	}
#else
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:WIFI设备初始化失败(%d)", __FILE__, __LINE__, ret);
		return FAIL;
	}
	
#if 0	
	memset(&init, 0, sizeof(init));
	strcpy(init.psEthIp, netsetting.LocalIP.Str);
	strcpy(init.psEthNetmask, netsetting.LocalMask.Str);
	strcpy(init.psEthGateway, netsetting.LocalGateway.Str);
	strcpy(init.psEthDnsPrimary, netsetting.DNSHost[0]);
	strcpy(init.psEthDnsSecondary, netsetting.DNSHost[1]);
	init.emSecMode = linksetting.WLANEncMode;
	init.ucIfDHCP = netsetting.DHCPenable;
	init.emKeyType =  linksetting.WLANKeyType;
#endif 

	init.ucIfDHCP = netsetting.DHCPenable;
	init.psEthIp = netsetting.LocalIP.Str;
	init.psEthNetmask = netsetting.LocalMask.Str;
	init.psEthGateway = netsetting.LocalGateway.Str;
	init.psEthDnsPrimary = netsetting.DNSHost[0];
	init.psEthDnsSecondary = netsetting.DNSHost[1];
	init.pszKey = linksetting.WLANPwd;
	init.emKeyType = linksetting.WLANKeyType;
	init.emSecMode = linksetting.WLANEncMode;

	cls_printf("接入AP(%s)中...\n", WlanEssid);
	if(init.emSecMode==WIFI_NET_SEC_WPA_EAP)
	{
		init2.comm_param = init;
		init2.pszIdentity = linksetting.WLAN8021XIdentity;
		init2.emEap = linksetting.WLAN8021XEapMode;
		init2.pszca_cert = NULL;
	
		if((ret=NDK_WiFiConnect_8021x(WlanEssid, &init2))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
			NDK_WiFiDisconnect();
			NDK_WiFiShutdown();
			return FAIL;
		}
	}
	else
	{
		if((ret=NDK_WiFiConnect(WlanEssid, &init))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
			NDK_WiFiDisconnect();
			NDK_WiFiShutdown();
			return FAIL;
		}

	}
	
    oldtime = time(NULL);
	while(1)//do
	{
		if ((ret=NDK_WiFiConnectState(&state))!=NDK_OK||(diff=time(NULL)-oldtime)>30)//NORETURN_AFTERPREDIAL:0
		{
            cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接%s失败!(%d,%d)", __FILE__, __LINE__, WlanEssid, ret, state);
			NDK_WiFiDisconnect();
            NDK_WiFiShutdown();
		    return FAIL;
        }
		show_stopwatch(1, diff);
		if(state==WIFI_WPA_CONSTATE_CONTEXT)
			break;
		else
			NDK_SysMsDelay(10);//稳定一段时间再看看
	}

	if((ret=NDK_WiFiIsConnected())!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		NDK_WiFiDisconnect();
		NDK_WiFiShutdown();
		return FAIL;
	}
#endif

	linkstatus = LINKUP;
	return SUCC;
}

int __LinkDown_WLAN(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
#if NEW_WIFI_ENABLE
	//挂断操作
	if(g_wifistopflag == 1)
	{
		if((ret=NDK_WifiStop())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:断开%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
			return FAIL;
		}
	}
	else
		;//关闭
#else
	cls_printf("断开AP(%s)连接中...\n", WlanEssid);	
	if((ret=NDK_WiFiDisconnect())!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:断开%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		NDK_WiFiShutdown();
		return FAIL;
	}
	if((ret=NDK_WiFiShutdown())!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:断开%s失败!(%d)", __FILE__, __LINE__, WlanEssid, ret);
		NDK_WiFiShutdown();
		return FAIL;
	}
#endif
	linkstatus = LINKDOWN;//以太网真正关闭才置LINKDOWN状态
	return SUCC;
}

int __NetUp_WLAN(void)
{
	/*private & local definition*/
	int ret;
	char LocalIp[16],LocalMask[16], LocalGateway[16];
	
	/*process body*/
	cls_printf("建立WLAN网络中...\n");
	
	if(netsetting.DHCPenable==ENABLE)
	{
		//进一步申请动态IP(可能一次取不到,多取几次)
		if((ret=lib_GetLocalAddr(LocalIp, LocalMask, LocalGateway,NULL))!=SUCC)//if((LocalIp=GetLocalAddr())==0)
		{//先获取出错时的错误码,再关闭
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:获取动态IP失败(%d)", __FILE__, __LINE__, ret);
			return FAIL;
		}
		//将获取到地址信息串转为ul数值同步到全局变量中(layer相关的全局变量一般应在配置时就填充和同步,在xxxUp等中只作为右值;动态获取是个例外,部分变量需要作为左值进行更新)
		strcpy(netsetting.LocalIP.Str, LocalIp);
		netsetting.LocalIP.value.data = INET_ADDR(LocalIp);
		strcpy(netsetting.LocalMask.Str, LocalMask);
		netsetting.LocalMask.value.data = INET_ADDR(LocalMask);
		strcpy(netsetting.LocalGateway.Str, LocalGateway);
		netsetting.LocalGateway.value.data = INET_ADDR(LocalGateway);
	}
	
	//若是静态配置的,则IP等数据在wlan_config时已传入底层
	
	netstatus = NETUP;
	return SUCC;
}

int __NetDown_WLAN(void)
{
	/*private & local definition*/
	int ret = SUCC;

	/*process body*/
	cls_printf("断开WLAN网络中...\n");
	if(__LinkDown_WLAN()!=SUCC)//if(__LinkDown(type)!=SUCC)
		ret = FAIL;

	netstatus = NETDOWN;
	return ret;
}
#else
int __LinkUp_WLAN(void){return FAIL;}
int __LinkDown_WLAN(void){return FAIL;}
int __NetUp_WLAN(void){return FAIL;}
int __NetDown_WLAN(void){return FAIL;}
#endif

#if PPP_ENABLE||ETH_ENABLE||WLAN_ENABLE
//注意:本函数主要用于动态获取本地一系列地址(如,(有线或无线)以太网DHCP;无线)
int lib_GetLocalAddr(char *ipAddr, char *subnet, char *gateway, char *dns)//ulong GetLocalAddr(void);
{
	/*private & local definition*/
	int ret = 0;
	ulong LocalIp = 0/*, HostIp = 0*/;
	time_t diff = 0, oldtime = time(NULL);

	/*process body*/
#if 1
	LinkType type = getlinktype();
	switch(type)
	{
		case ETH:
			//return NDK_EthGetNetAddr(ipAddr, subnet, gateway, dns);
			return NDK_NetGetAddr(COMM_TYPE_ETH, ipAddr, subnet, gateway, dns);
			break;
		case WLAN:
			return NDK_NetGetAddr(COMM_TYPE_WIFI, ipAddr, subnet, gateway, dns);
			break;
		case GPRS:
		case CDMA:
		case TD:
		case ASYN:
			{
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
				while((ret=NDK_PppGetAddr(&LocalIp, NULL/*&HostIp*/))!=NDK_OK||LocalIp==0)//未取到本地地址(即未申请成功)  
				{
					if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
						return FAIL;
					show_stopwatch(ENABLE, diff);
				}
				show_stopwatch(DISABLE, 0);
				INET_NTOA(HTONL(LocalIp), ipAddr);
#elif defined ME50NX||defined ME68
				;		//ME50NX不支持NDK_PppGetNetAddr,实际底层会返回-18,故不调用	20171227	
#else
				while((ret=NDK_PppGetNetAddr(ipAddr))!=NDK_OK)
				{
					if((diff=time(NULL)-oldtime)>MAXTIMEOUT)
						return FAIL;
					show_stopwatch(ENABLE, diff);
				}
#endif
				
				if(subnet!=NULL)
					strcpy(subnet, "0.0.0.0");
				if(gateway!=NULL)
					strcpy(gateway, "0.0.0.0");
				if(dns!=NULL)
					strcpy(dns, "0.0.0.0");
				
				return SUCC;
			}
			break;
		//case BT:
			//return NDK_NetGetAddr(COMM_TYPE_BT, ipAddr, subnet, gateway, dns);
			//break;
		default:
			cls_show_msg1(10, "%s, line %d:建立未知类型(%d)的网络失败!", __FILE__, __LINE__, type);
			return FAIL;
			break;
	}
#else	
	if((getlinktype()==ETH)||(getlinktype()==WLAN))
		return NDK_EthGetNetAddr(ipAddr, subnet, gateway, dns);//FAIL;
	else
	{
		while((ret=NDK_PppGetAddr(&LocalIp, NULL/*&HostIp*/))!=NDK_OK||LocalIp==0)//未取到本地地址(即未申请成功)  
		{
			if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
				return FAIL;
			show_stopwatch(ENABLE, diff);
		}
		show_stopwatch(DISABLE, 0);
		INET_NTOA(HTONL(LocalIp), ipAddr);
		if(subnet!=NULL)
			strcpy(subnet, "0.0.0.0");
		if(gateway!=NULL)
			strcpy(gateway, "0.0.0.0");
		if(dns!=NULL)
			strcpy(dns, "0.0.0.0");
		
		return SUCC;
	}
#endif

}
#else
int lib_GetLocalAddr(char *ipAddr, char *subnet, char *gateway, char *dns){return FAIL;}
#endif

#if PPP_ENABLE
int WaitPPPState(EM_PPP_STATUS state, int *err)
{
	/*private & local definition*/
	int ret = 0, times = 0;
	EM_PPP_STATUS actual = PPP_STATUS_DISCONNECT;
	time_t diff = 0, oldtime = time(NULL);//不使用不可重入函数StartStopwatch()
	
	/*process body*/
	while(1)//((ret=GetPPPphase(ph))!=state)
	{
		if((ret=NDK_PppCheck(&actual, err))!=NDK_OK)
			return ret;//FAIL;
		if(actual==state)
		{
			if(++times==TIME4KEEPSTATE)
				break;
		}
		if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);//20140505linwl 将延时动作移动到show_stopwatch函数中
	}

	show_stopwatch(DISABLE, 0);
	return (actual==state?SUCC:-3/*FAIL*/);//-3以区别上面的调用失败返回-1
}

int __NetUp_PPP(void)
{
	/*private & local definition*/
	int ret = 0, err = 0;
	char LocalIp[16] = {0}, LocalMask[16] = {0}, LocalGateway[16] = {0}, DNS[16] = {0};
	//ST_PPP_CFG tmp_stPPPCfg;
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case ASYN:
		if((ret=WM_PPP_16300_DIAL())<0)
		{
			cls_show_msg1(10, "%s, line %d:异步拔打16300失败!(%d)", __FILE__, __LINE__,  ret);
			__LinkDown();//NET有错误了,走PPP的话,应直接切断LINK层(因为只断开NET层是不够的)
			return ret;//FAIL;//不必理会__LinkDown成功否,这里返回FAIL即可
		}
	case GPRS:
	case CDMA:
	case TD:
		//以下是为PPP网络层准备参数,移入PPP网络层更合适
#if 0	//zhnagnw20130201移到conf.c中
		memset((void *)&tmp_stPPPCfg, 0, sizeof(tmp_stPPPCfg));
		if(type==ASYN)
		{	
			strcpy(tmp_stPPPCfg.szDailNum, MDMDialStr);
			tmp_stPPPCfg.nDevType = 1;//0表示 无线,1为有线
			//tmp_stPPPCfg.nPPPFlag = 1;
			//strcpy(tmp_stPPPCfg.szApn, "CMNET");
			tmp_stPPPCfg.ModemDial = ASYNDial;//底层默认不帮忙做有线PPP拨号函数功能，如果需要应用层人员自己传入拨号函数．
			tmp_stPPPCfg.PPPIntervalTimeOut = 600;

			//目前在conf_MDM_lnk及conf_PPP中均无对以下2个异步PPP参数的配置,暂在此处写死
			strcpy(PPPUSR, DEFAULT_ASYN_USR);
			strcpy(PPPPWD, DEFAULT_ASYN_PWD);
		}
		else
		{
			strcpy(tmp_stPPPCfg.szDailNum, "*99***1#");//可由底层指定
			tmp_stPPPCfg.nDevType = 0;//0表示 无线,1为有线
			tmp_stPPPCfg.nPPPFlag = LCP_PPP_UNKEEPED;//短连接
			sprintf(tmp_stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(tmp_stPPPCfg.szApn, "\"%s\"", WLMAPN);
			tmp_stPPPCfg.ModemDial = NULL;
			//tmp_stPPPCfg.PPPIntervalTimeOut = 600;
		}
		
		if((ret=NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg)))!=NDK_OK)//if((ret=NetPPPCfg(&tmp_stPPPCfg, 0))<0)
		{
			cls_show_msg1(10, "%s, line %d:链路层参数配置失败(%d)", __FILE__, __LINE__, ret);
			return ret;//FAIL;
		}
#endif
		
		if((ret=NDK_PppDial(PPPUSR, PPPPWD))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:打开PPP通道失败!(%d)", __FILE__, __LINE__,  ret);
			__LinkDown();//NET有错误了,走PPP的话,应直接切断LINK层(因为只断开NET层是不够的)
			return ret;//FAIL;//不必理会__LinkDown成功否,这里返回FAIL即可
		}

		cls_printf("建立PPP网络中...\n");
		if((ret=WaitPPPState(PPP_STATUS_CONNECTED, &err))!=SUCC)
		{//先获取出错时的错误码,再关闭
			cls_show_msg1(10, "%s, line %d:获取PHASE_NETWORK失败!(ret=%d,err=%d)", __FILE__, __LINE__, ret, err);
			NDK_PppHangup(1);//NetPPPHangup(1);
			__LinkDown();//NET有错误了,走PPP的话,直接切断LINK层(只断开NET层是不够的)
			return ret;//FAIL;
		}		

		//进一步申请动态IP(可能一次取不到,多取几次)    
		if((ret=lib_GetLocalAddr(LocalIp, LocalMask, LocalGateway, DNS))!=SUCC)//if((LocalIp=GetLocalAddr())==0)
		{//先获取出错时的错误码,再关闭
			cls_show_msg1(10, "%s, line %d:获取动态IP失败(%d)!", __FILE__, __LINE__, ret);
			NDK_PppHangup(1);//NetPPPHangup(1);
			__LinkDown();//NET有错误了,走PPP的话,直接切断LINK层(只断开NET层是不够的)
			return ret;//FAIL;
		}

		//将获取到地址信息串转为ul数值同步到全局变量中(layer相关的全局变量一般应在配置时就填充和同步,在xxxUp等中只作为右值;动态获取是个例外,部分变量需要作为左值进行更新)
		strcpy(netsetting.LocalIP.Str, LocalIp);
		netsetting.LocalIP.value.data = INET_ADDR(LocalIp);
		strcpy(netsetting.LocalMask.Str, LocalMask);
		netsetting.LocalMask.value.data = INET_ADDR(LocalMask);
		strcpy(netsetting.LocalGateway.Str, LocalGateway);
		netsetting.LocalGateway.value.data = INET_ADDR(LocalGateway);
		//DNS呢?
		break;
	default:
		return FAIL;
		break;
	}

	netstatus = NETUP;
	return SUCC;	
}

int __NetDown_PPP(void)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:
	case ASYN:
		cls_printf("断开PPP网络中...\n");
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
		if((ret=NDK_PppHangup(1))!=NDK_OK)//if(NetPPPHangup(1)!=SUCC)//使用阻塞式挂断
#else
		if((ret=NDK_PppHangup(0))!=NDK_OK)
#endif			
		{
			cls_show_msg1(10, "%s, line %d:断开PPP网络失败!", __FILE__, __LINE__);
			//既然没有关成功,就不必让状态为DOWN了
			return ret;//FAIL;
		}
		
		//注意:不能直接调LinkDown!(把其实现体提取出来还是可以的)
		//原因:1、会形成A->B,B->A...循环调用;2、NET层调LINK层也不合适
		//PPPCLOSE后,本端已处于LINKDOWN状态,大约40s,对端才会完全挂断
		if((ret=__LinkDown())!=SUCC)
		{
			netstatus = NETDOWN;
			return ret;//FAIL;
		}
		break;
	default:
		return FAIL;
		break;
	}

	netstatus = NETDOWN;
	return SUCC;
}
#else
int WaitPPPState(EM_PPP_STATUS state, int *err){return FAIL;}
int __NetUp_PPP(void){return FAIL;}
int __NetDown_PPP(void){return FAIL;}
#endif

int LinkUp4AsynPPP(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	if(linkstatus==LINKUP)
		return SUCC;

	transstatus = TRANSDOWN;
	netstatus = NETDOWN;
	
	if((ret=ASYNDial4PPP())!=SUCC)
		return ret;	

	linkstatus = LINKUP;
	return SUCC;
}

//内部函数,与LinkUp区别在于无条件执行UP
//关于GetErrorCode的使用:对于非直接调用API出错,不要使用它来定位错误.建议开发人员使用ret来表明各种错误(如可以将errcode当ret返回即可).
static int __LinkUp(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:
		return __LinkUp_WLM();
		break;
	case ASYN:
		return __LinkUp_ASYN();
		break;
	case SYNC:
		return __LinkUp_SDLC();
		break;
	case ETH://调用者自己负责安装以太网
		return __LinkUp_ETH();
		break;
	case WLAN:
		return __LinkUp_WLAN();
		break;
	default:	//不识别不支持的连接类型,不改变状态变量
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:建立未知类型(%d)的连接失败!", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	//放到各子函数中负责linkstatus = LINKUP;
	cls_printf("建立链路层成功!");
	return SUCC;
}

//原则上本函数只保证链路的自端起来,不保证对端!
//层次上本函数处于第一层(PL)
//同在第一层的还有wireless_selfadapt,modemATCommond,WLM_get_res,InitAsynModem,AsynDial等
//调用建议:先对linktype赋值后,再LinkUp(linktype).要与LinkDown成对使用
//注意:要使用异步猫PPP,不能主动先调用LinkUp再调用NetUp,而应直接调用NetUp(或调用LinkUp4AsynPPP)!!!
int LinkUp(void)
{
	/*private & local definition*/

	/*process body*/
	if(linkstatus==LINKUP)
		return SUCC;

	transstatus = TRANSDOWN;
	netstatus = NETDOWN;
	return __LinkUp();
}

static int __LinkDown(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:
		return __LinkDown_WLM();
		break;
	case ASYN:
		return __LinkDown_ASYN();
		break;
	case SYNC:
		return __LinkDown_SDLC();
		break;
	case ETH:
		return __LinkDown_ETH();
		break;
	case WLAN:
		return __LinkDown_WLAN();
		break;
	default://不识别不支持的连接类型
		cls_show_msg1(10, "%s, line %d:断开未知类型(%d)的连接失败!", __FILE__, __LINE__, type);
		return FAIL;//不改变状态变量,提前return
		break;
	}

	//放到各子函数中负责linkstatus = LINKDOWN;
	cls_printf("断开链路层成功!");
	return SUCC;
}

//从本函数的实现看,对无线来说,即使不调用本函数,一定时间后也有DOWN的风险(而此时linkstatus会错误地仍指示为UP)
int LinkDown(void)
{
	/*private & local definition*/
	
	/*process body*/
	//自上而下逐层关闭(逐层判断)
	NetDown();//先关闭link的上层(即net层),以做到完全关闭
	
	if(linkstatus==LINKDOWN)
		return SUCC;

	return __LinkDown();
}

static int __NetUp(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:
	case ASYN:
		return __NetUp_PPP();
		break;
	case SYNC://不支持
		return __NetUp_SDLC();
		break;
	case ETH:
		return __NetUp_ETH();
		break;
	case WLAN:
		return __NetUp_WLAN();
		break;
	default:
		cls_show_msg1(10, "%s, line %d:建立未知类型(%d)的网络失败!", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	//放到各子函数中负责netstatus = NETUP;
	cls_printf("建立网络层成功!");
	return SUCC;
}

//层次上本函数处于第2,3层(DL,NL)
//同一层的还有IsNetConnected,conf_network等
//调用建议:先对linktype赋值后,再调用.另请自行确保已配置好网络了
//调用 上层的UP,会自行将下层先UP起来(以简化调用).关闭时,执行相反的动作
//特别说明:NetUp会LinkUp,NetDown也会引起LinkDown(本身应该是LinkDown也会引起NetDown).这里的设计是比较特别的.
int NetUp(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	//自下而上逐层起来(逐层判断)
	if((getlinktype()!=ASYN&&(ret=LinkUp())!=SUCC)
	||(getlinktype()==ASYN&&(ret=LinkUp4AsynPPP())!=SUCC))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:LinkUp失败(%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;
	}
	
	if(netstatus==NETUP)
		return SUCC;

	transstatus = TRANSDOWN;
	return __NetUp();
}

static int __NetDown(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();

	/*process body*/
	switch(type)
	{
	case GPRS:
	case CDMA:
	case TD:
	case ASYN:
		return __NetDown_PPP();
		break;
	case SYNC:
		return __NetDown_SDLC();
		break;
	case ETH:
		return __NetDown_ETH();
		break;
	case WLAN:
		return __NetDown_WLAN();
		break;
	default:
		cls_show_msg1(10, "%s, line %d:断开未知类型(%d)的网络失败!", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	//放到各子函数中负责netstatus = NETDOWN;
	cls_printf("断开网络层成功!");
	return SUCC;
}

//返回值没什么意义,可不使用.因为在返回FAIL,用户已能得知了
int NetDown(void)
{
	/*private & local definition*/

	/*process body*/
	TransDown(g_socket);//TransDown(g_socktype, g_socket);//保护
	transstatus = TRANSDOWN;//不论上面的操作成功否,这里再强制做一次
	
	if(netstatus==NETDOWN)
		return SUCC;
	
	return __NetDown();
}

#if NDKSOCK_ENABLE
static int NDKTransUp(uint *s)
{
	/*private & local definition*/
	uint timeout = SO_TIMEO, sckt = 0;
	int ret = 0;
	sock_t socktype = netsetting.socktype;
	int (*socket[])(uint *) = {NDK_TcpOpen/*,  NDK_UdpOpen*/};
	
	/*process body*/
	if(socktype!=SOCK_NDKTCP && socktype!=SOCK_NDKUDP)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:套接字类型错误!", __FILE__, __LINE__);
		return FAIL;
	}
	
	cls_printf("NDK传输层(%s)建立中...\n", (socktype==SOCK_NDKTCP)?"TCP":"UDP");
	
	//创建socket
	if ((ret=socket[socktype](&sckt))!=NDK_OK)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:申请套接字失败(%d)", __FILE__, __LINE__, ret);
		return ret;
	}
	
	//设置socket
	
	//绑定本地IP
	//netsetting.LocalPort = rand()%10000+5000;//在后台看到的这个值可能会变(故意义不大)
	netsetting.LocalPort = rand()%(65535-1024)+1024;  //将本地端口号范围由5000~15000 扩大到 1024~65535
	//netsetting.LocalPort++;//根据标准协议客户端需要4分钟去释放本地端口,避免随机到未释放的端口引起绑定失败修改端口用递增方式；
	if ((ret=NDK_TcpBind(sckt, netsetting.LocalIP.Str, HTONS(netsetting.LocalPort)))!=NDK_OK)//==SOCKET_ERROR)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:绑定失败(%d)", __FILE__, __LINE__, ret);
		NDK_TcpClose(sckt);
		NDK_TcpWait(sckt);
		return ret;//FAIL;
	}
	
	//连接socket
	//if(socktype==SOCK_NDKTCP)
	//{
		if((ret=NDK_TcpConnect(sckt, netsetting.SvrIP.Str, HTONS(netsetting.SvrTcpPort), timeout))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:连接失败(%d)", __FILE__, __LINE__, ret);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			//sleep(10);//必须等待本端发出拆链的第4个报文时,才从本函数跳出(否则链路太早断(NetDown),对方收不到ACK)
			return ret;//FAIL;
		}
#if NEW_WIFI_ENABLE||defined SP610
		if((ret=lib_tcp_checkstatus(sckt, timeout))!=SOCKET_STATUS_CONNECT)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:TCP获取连接失败(%d)", __FILE__, __LINE__, ret);
			NDK_TcpClose(sckt);
			NDK_TcpWait(sckt);
			return FAIL;//ret;
		}
#endif		
	//}

	*s = sckt;
	g_socktype = socktype;
	return SUCC;
}

//尽量不用粗鲁的方式down,而用一种稳妥的保险的方式来进行
static int NDKTransDown(uint s)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	cls_printf("NDK传输层(%s)断开中...\n", (g_socktype==SOCK_NDKTCP)?"TCP":"UDP");
	if(TCPRESETFLAG==FALSE)
	{
		if ((ret=NDK_TcpClose(s))!=NDK_OK)//释放本地资源并放出FIN
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:tcpClose失败(%d)", __FILE__, __LINE__, ret);
			NDK_TcpWait(s);//close都失败了(可能FIN都没发出就不用wait了),tcpWait(s);
			return ret;//FAIL;
		}
	}
	else
	{
#if defined ME50C  //ME50C只支持强制挂断,且没有Reset接口,需要使用close挂断 20180620 modify
		if ((ret=NDK_TcpClose(s))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:tcpClose失败(%d)", __FILE__, __LINE__, ret);
			NDK_TcpWait(s);
			return ret;
		}
#else
		if ((ret=NDK_TcpReset(s))!=NDK_OK)//强制挂断 20140422 linwl 4.4.7新增
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:tcpClose失败(%d)", __FILE__, __LINE__, ret);
			return ret;
		}
#endif
	}
#if 0
	if((ret=NDK_TcpWait(s))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:tcpWait失败(%d)", __FILE__, __LINE__, ret);
		return FAIL;
	}
#else
	//msdelay(100);
	NDK_TcpWait(s);//不判断,避免因网络断开等故障而等不到对端的FIN信号而本地其实又已释放了资源造成的状态误判
#endif
	//加sleep是为了防止tcpWait没有等待对方的FIN也就没回这个FIN的ACK(如tcpWait只是个空函数),充分考虑了网络延迟的因素(有时网络情况会突然不好等)
	//在测试无线时,甚至发现6s还不够用的情况(有时对方6s时才收到POS的FIN,此后就会引起对方不断回ACK+FIN,而POS不回ACK的情况)
	//sleep(10);//必须等待本端发出拆链的第4个报文时,才从本函数跳出(否则链路太早断(NetDown),对方收不到ACK)
	
	return ret;
}

//从s接收len个的数据放在基址为rbuf的缓冲区内,rbuf大小必须大于len(由上层者自行保证)
//返回值有2个含义:>=0为实际接收长度;<0为NDK_TcpRead的(错误)返回值.故设计返回值为int型,以达到一种返回值同时反映2方面信息的目的
int NDKTransReceiveN(uint s, char *buf, uint len, uint timeout)
{
	/*private & local definition*/
	int ret = 0/*, rlen = 0, rcount = -1*/;
	uint rest = 0, total = 0, tmplen = 0;
	time_t diff = 0, oldtime = time(NULL);

	/*process body*/
	while(total<len)//每次只能接收1488长度，所以3076要分包接收（网络分包）
	{
		if((diff=time(NULL)-oldtime)>timeout)
			return total;
		else//更新等待时间
			rest = timeout-diff;
		
		if((ret=NDK_TcpRead(s, buf+total, len-total, rest, &tmplen))!=NDK_OK)//不必重试,之后必定是失败的了
		{
			//cls_show_msg1(10, "%s, line %d:接收失败(%d)", __FILE__, __LINE__, ret);
			return (total?total:ret);//return ret;//FAIL;
		}
		
		if (tmplen==0)
		{
#if 0
			if (++rcount==1)//重试1次足矣
			{
				//cls_show_msg1(10, "%s, line %d:连续接收超时(预期:%d 实际:%d)",  __FILE__, __LINE__, len, rlen);
				return total;//ret;//FAIL;
			}
			continue;
#else	//不必重试
			return total;
#endif
		}
		else
		{
			//rcount = 0;
			total+=tmplen;//修改接收到的数据总长度
		}
	}

	return total;
}

int NDKTransSendN(uint s, char *buf, uint len, uint timeout)
{
	/*private & local definition*/
	uint tmplen = 0;
	int ret = 0;

	/*process body*/
	return ((ret=NDK_TcpWrite(s, buf, len, timeout, &tmplen))==NDK_OK)?tmplen:ret;
}
#else
static int NDKTransUp(uint *s){return FAIL;}
static int NDKTransDown(uint s){return FAIL;}
int NDKTransReceiveN(uint s, char *buf, uint len, uint timeout){return FAIL;}
int NDKTransSendN(uint s, char *buf, uint len, uint timeout){return FAIL;}
#endif

#if NDKSSL_ENABLE
int WaitNDKSSLState(NDK_HANDLE sh, int state, int timeout)
{
	/*private & local definition*/
	int ret = 0;
	int times = 0;
	int actual = -1;
	time_t diff = 0, oldtime = time(NULL);//不使用不可重入函数StartStopwatch()
	
	/*process body*/
	while(1)
	{
		if((ret=NDK_GetSSLConnectStatus(sh, &actual))==NDK_ERR_TIMEOUT)
			return ret;//超时的话,就明确返回超时,而不是FAIL
		else if(ret!=NDK_OK)
			return FAIL;
		if(actual==state)
		{
			if(++times==TIME4KEEPSTATE)
				break;
		}
		if((diff=time(NULL)-oldtime)>timeout)//MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}

	show_stopwatch(DISABLE, 0);
	return (actual==state?SUCC:-3/*FAIL*/);
}

static int NDKSSLUp(NDK_HANDLE *s)
{
#define CERTF   "clientcert.pem"
#define KEY     "clientkey.pem"
#define PRIKEY_PWD	"123456"
	/*private & local definition*/
	int timeout = SO_TIMEO*1000, state = 0;//NDK_SSLConnect超时参数单位是毫秒 所以要*1000
	NDK_HANDLE sckt = NULL;
	int ret = NDK_OK;
	//ST_SOCKET_ADDR myadd;
	ST_SOCKET_ADDR svradd;
	int ciphers[] = {SSL3_CIPHER_RSA_NULL_MD5, SSL3_CIPHER_RSA_NULL_SHA, SSL3_CIPHER_RSA_RC4_40_MD5, SSL3_CIPHER_RSA_RC4_128_MD5, SSL3_CIPHER_RSA_RC4_128_SHA,
					SSL3_CIPHER_RSA_RC2_40_MD5, SSL3_CIPHER_RSA_IDEA_128_SHA, SSL3_CIPHER_RSA_DES_40_CBC_SHA, SSL3_CIPHER_RSA_DES_64_CBC_SHA,
		             		SSL3_CIPHER_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_DSS_DES_40_CBC_SHA, SSL3_CIPHER_DH_DSS_DES_64_CBC_SHA,
					SSL3_CIPHER_DH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_40_CBC_SHA, SSL3_CIPHER_DH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_40_CBC_SHA,
					SSL3_CIPHER_EDH_DSS_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_EDH_RSA_DES_40_CBC_SHA,
					SSL3_CIPHER_EDH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_ADH_RC4_40_MD5, SSL3_CIPHER_ADH_RC4_128_MD5,
					SSL3_CIPHER_ADH_DES_40_CBC_SHA, SSL3_CIPHER_FZA_DMS_NULL_SHA, SSL3_CIPHER_CK_FZA_DMS_FZA_SHA, SSL3_CIPHER_CK_FZA_DMS_RC4_SHA,
					SSL3_CIPHER_CK_ADH_DES_64_CBC_SHA, SSL3_CIPHER_CK_ADH_DES_192_CBC_SHA, 0x0};	
	
	/*process body*/
	cls_printf("SPSSL层建立中...\n");
	//创建SSLsocket
	if ((sckt=NDK_OpenSSLSocket(netsetting.SSLHsOpt, netsetting.SSLAuthOpt, ciphers))==NULL)
	{
		cls_show_msg1(10, "%s, line %d:申请套接字失败", __FILE__, __LINE__);
		return FAIL;
	}
	//加载本地证书
	if((ret=NDK_LoadClientCertificate(sckt,CERTF,SSL_FILE_PEM))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:加载本地证书(%d)", __FILE__, __LINE__, ret);
		return ret;
	}
	//加载私钥
	if((ret=NDK_LoadClientPrivateKey(sckt, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:加载私钥(%d)", __FILE__, __LINE__, ret);
		return ret;
	}
	//设置socket
	if((ret=NDK_SetSSLBlockingMode(sckt, NDK_SUSPEND))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:设置 非阻塞模式失败(%d)", __FILE__, __LINE__, ret);
		NDK_CloseSSLSocket(sckt);
		return ret;
	}
	
#if 0	//no need
	//绑定本地IP
	memset((void *) &myadd, 0, sizeof(ST_SOCKET_ADDR));
	myadd.addr_type = ADDR_IPV4;
	myadd.port = netsetting.LocalPort;
	myadd.addr = netsetting.LocalIP.Str;
	if((ret=NDK_SSLBind(sckt, &myadd))!=orvOK)
	{
		cls_show_msg1(10, "%s, line %d:绑定失败(%d)", __FILE__, __LINE__, ret);
		NDK_CloseSSLSocket(sckt);
		return ret;
	}
#endif

	if(netsetting.SSLAuthOpt==SSL_AUTH_CLIENT)
	{
		if((ret=NDK_LoadServerCertificate(sckt, SCERTF, SSL_FILE_PEM))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:装载服务器证书失败(%d)", __FILE__, __LINE__, ret);
			NDK_CloseSSLSocket(sckt);
			return ret;
		}
	}
	
	//连接socket
	memset((void *)&svradd, 0, sizeof(ST_SOCKET_ADDR));
	svradd.unAddrType = SSL_ADDR_IPV4;
	svradd.usPort = netsetting.SvrTcpPort;
	svradd.psAddr = netsetting.SvrIP.Str;
	if((ret=NDK_SSLConnect(sckt, &svradd, timeout))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:连接失败(%d)", __FILE__, __LINE__, ret);
		NDK_CloseSSLSocket(sckt);
		return ret;
	}
	if((ret=NDK_GetSSLConnectStatus(sckt, &state))!=NDK_OK||state!=SSL_IS_CONNECTED)
	{
		cls_show_msg1(10, "%s, line %d:未连接上(%d,%d)", __FILE__, __LINE__, ret, state);
		NDK_CloseSSLSocket(sckt);
		return ret;
	}

	*s = sckt;
	g_socktype = SOCK_NDKSSL;
	return SUCC;
}

static int NDKSSLDown(NDK_HANDLE s)
{
	/*private & local definition*/
	int ret = NDK_OK;
	int state = 0;
	
	/*process body*/
	cls_printf("SPSSL断开中...\n");
	if((ret=NDK_SSLDisconnect(s))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:断开失败(%d)", __FILE__, __LINE__, ret);
		//sleep(getlinktype()==ETH?3:6);//必须等待本端发出拆链的第4个报文时,才从本函数跳出(否则链路太早断(NetDown),对方收不到ACK)
		NDK_CloseSSLSocket(s);
		return ret;
	}
	if((ret=NDK_GetSSLConnectStatus(s, &state))!=NDK_OK||state!=SSL_IS_DISCONNECTED)
	{
		cls_show_msg1(10, "%s, line %d:未断开(%d,%d)", __FILE__, __LINE__, ret, state);
		NDK_CloseSSLSocket(s);
		return ret;
	}
	if(TCPRESETFLAG==FALSE)
	{
		if((ret=NDK_CloseSSLSocket(s))!=NDK_OK)
		{
			cls_show_msg1(10, "%s, line %d:释放SSL失败(%d)", __FILE__, __LINE__, ret);
			//加sleep是为了防止tcpWait没有等待对方的FIN也就没回这个FIN的ACK(如tcpWait只是个空函数),充分考虑了网络延迟的因素(有时网络情况会突然不好等)
			//在测试无线时,甚至发现6s还不够用的情况(有时对方6s时才收到POS的FIN,此后就会引起对方不断回ACK+FIN,而POS不回ACK的情况)		
			//sleep(10);//必须等待本端发出拆链的第4个报文时,才从本函数跳出(否则链路太早断(NetDown),对方收不到ACK)
			return ret;
		}
	}
	else
	{
		if((ret=NDK_CloseSSLSocket(s))!=NDK_OK)//强制挂断  20140422 linwl 4.4.7新增
		{
			cls_show_msg1(10, "%s, line %d:释放SSL失败(%d)", __FILE__, __LINE__, ret);
			return ret;
		}
	}
	
	return ret;
}

int NDKSSLReceiveN(NDK_HANDLE handle, void *pBuffer, /*const */size_t SizeOfBuffer/*, size_t *recv_data*/)
{
	/*private & local definition*/
	size_t tmplen = 0;
	int ret = 0;	

	/*process body*/
	NDK_SSLDataAvailable(handle, 2000);
	if(g_socktype==SOCK_NDKSSL)
		return ((ret=NDK_SSLReceive(handle, pBuffer, SizeOfBuffer, &tmplen))==NDK_OK)?tmplen:ret;//return NDK_SSLReceive(handle, pBuffer, SizeOfBuffer, recv_data);
	else
		return FAIL;
}

int NDKSSLSendN(NDK_HANDLE handle, /*const */char *pBuffer, size_t SizeOfBuffer/*, size_t *recv_data*/)
{
	/*private & local definition*/
	size_t tmplen = 0;
	int ret = 0;

	/*process body*/
	if(g_socktype==SOCK_NDKSSL)
		return ((ret=NDK_SSLSend(handle, pBuffer, SizeOfBuffer, &tmplen))==NDK_OK)?tmplen:ret;//return NDK_SSLSend(handle, pBuffer, SizeOfBuffer, recv_data);
	else
		return FAIL;
}
#else
int WaitNDKSSLState(NDK_HANDLE sh, int state, int timeout){return FAIL;}
static int NDKSSLUp(NDK_HANDLE *s){return FAIL;}
static int NDKSSLDown(NDK_HANDLE s){return FAIL;}
int NDKSSLReceiveN(NDK_HANDLE handle, void *pBuffer, /*const */size_t SizeOfBuffer/*, size_t *recv_data*/){return FAIL;}
int NDKSSLSendN(NDK_HANDLE handle, /*const */char *pBuffer, size_t SizeOfBuffer/*, size_t *sent_data*/){return FAIL;}
#endif

int __TransUp(uint *s)
{
	/*private & local definition*/
	int ret = 0;
	sock_t socktype = netsetting.socktype;
	
	/*process body*/
	switch(socktype)
	{
	case SOCK_NDKTCP:
	case SOCK_NDKUDP:
		ret = NDKTransUp(s);
		break;
	case SOCK_NDKSSL:
		ret = NDKSSLUp((NDK_HANDLE *)s);
		break;
	default:
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:建立未知类型(%d)传输层失败!", __FILE__, __LINE__, socktype);
		return FAIL;
		break;
	}

	if(ret==SUCC)
	{
		g_socket = *s;//保存为linkdown时使用
		//g_socktype = socktype;//此句移入各自的TransUp中成功后保存,以使得各自的TransDown中可使用g_socktype
		transstatus = TRANSUP;
		cls_printf("建立传输层成功!");
	}
	
	return ret;
}

//注意:目前的设计是传输层只支持单通道传输
//调用本函数前,请自行调用NetUp
//由于传输层封装函数内部与全局变量绑定了(也因为此,TransUp等省去了socktype这样的参数),故使用前一定要配置一下才能保证正确使用
int TransUp(uint *s)
{
	/*private & local definition*/
	
	/*process body*/
	//自下而上逐层起来(逐层判断)
#if 0 //考虑到功能与DOWN对等,网络UP不由本函数负责
	if(NetUp(type)!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:NetUp失败", __FILE__, __LINE__);
		return FAIL;
	}
#endif
	
	if(transstatus==TRANSUP)
		return SUCC;

	return __TransUp(s);
}

int __TransDown(uint s)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	//移入相应的实现中去提示cls_printf("传输层断开中...\n");
	//只要传输层成功UP,刚g_socktype不会是未知类型,故default不要有错误提示,免得某些保护地方会有不友好信息
	//若传输层没有成功UP,则transstatus是DOWN状态的,根本调不进本函数
	switch(g_socktype)//switch(socktype)
	{
	case SOCK_NDKTCP:
	case SOCK_NDKUDP:
		ret = NDKTransDown(s);
		break;
	case SOCK_NDKSSL:
		ret = NDKSSLDown((NDK_HANDLE)s);
		break;
	default:
		transstatus = TRANSDOWN;//20160808 linwl增加未避免未知类型socket引起没有做实际挂断和连接动作
		//cls_show_msg1(10, "%s, line %d:断开未知类型(%d)传输层失败!", __FILE__, __LINE__, socktype);
		return FAIL;
		break;
	}
	if(ret==SUCC)
	{
		g_socket = 0;//-1;
		g_socktype = SOCK_DEFAULT;
		//transstatus = TRANSDOWN;
		cls_printf("断开传输层成功!");
	}
	else
		cls_show_msg1_record(FILENAME,FUNCNAME,10, "%s, line %d:传输层断开失败!", __FILE__, __LINE__);

	transstatus = TRANSDOWN;//zhangnw20091223 不论成功否,一律置位,即使本次socket未完全关闭(即close失败时)也先不管了
	return ret;
}

int TransDown(uint s)
{
	/*private & local definition*/
	
	/*process body*/
	if(transstatus==TRANSDOWN)
		return SUCC;
	
	return __TransDown(s);
}

#if 0
//设计:5s内不断尝试ping,若一直不成功,才判为失败.重试机制主要针对以太网在首次开机时,未自动加载驱动,但安装网络地址返回成功的情况
static int TestNetConnection(void)
{
	/*private & local definition*/
	time_t old = time(NULL), diff = 0;

	/*process body*/
	while(NDK_NetPing(netsetting.SvrIP.Str, 1)!=NDK_OK)//NDK_NetPing的最后一个参数为s!文档写错为ms
	{
		if((diff=time(NULL)-old)>MAXTIMEOUT_PING)
			return FALSE;			

		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}
	show_stopwatch(DISABLE, 0);
	return TRUE;
}
#else
#if !K21_ENABLE
static int TestNetConnection(void){return (NDK_NetPing(netsetting.SvrIP.Str, MAXTIMEOUT_PING)!=NDK_OK)?FALSE:TRUE;}//NDK_NetPing的最后一个参数为s!文档写错为ms
#else
#define TestNetConnection() FAIL
#endif
#endif

//用于判断"网络层"的连接情况,不可用于MODEM读写测试(属LINK层)前的连通性测试
//返回TRUE or FALSE.另外,本函数会改变lnk,net层状态,但不改变传输层状态
int IsNetConnected(void)
{
	/*private & local definition*/
	int ret = FALSE;

	/*process body*/
	if (NetUp()!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:网络UP失败!!!", __FILE__, __LINE__);
		return FALSE;
	}
	ret = TestNetConnection();//默认使用标准socket方式,否则会使用netsetting.socktype来测试连通性
	NetDown();
	
	return ret;
}
#endif

