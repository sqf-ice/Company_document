/*
 * FileName:       
 * Author:         zhangnw  Version: 1.6.3  Date: 2006-8-2
 * Description:   POS产品公共头文件（各模块的公共文件的内容也可放于此处）
 *			  本文件可放TCR产品共同的特性,若只是某产品特有的，还可定义在INC\TCR下它自己的目录中
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 *	zhangnw				1.6.3		从lib.c中分离
 *     zhangnw				1.6.5		在include 部分增加常用的库
 */

#ifndef _LIB_H_
#define _LIB_H_

/*-----------------------includes-------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "flag.h"

#include "gui.h"	//for gui.c display
#include "mifare-1.h"  //for rfcard general
#include "sec.h"  //for sec

/*---------------constants/macro definition---------------------*/
//DEBUGLEVEL
#define	COVERAGE	(1)

//EXITFLAG 单元测试请根据以下规则实现函数的返回
#if EXITFLAG
#define		RETURN	return;
#define		GOTOERR	goto ERR;
#define		GOTOERR2	goto ERR2;
#define		GOTOERR3	goto ERR3;
#else
#define		RETURN		;
#define		GOTOERR		;
#define		GOTOERR2		;
#define		GOTOERR3		;
#endif

#define	nlmain	main

#define	FUNCNAME	(char*)__FUNCTION__
#define	FILENAME	(char*)__FILE__

#if 0
#define	_CTYPE_INIT_SUPPORT
#endif

//zhangnw20060321 非UNIX下的回车换行要使用\r\n(从串口看到的应是0D0A)
#define REPORTHEAD \
"********************************************************\r\n\
*\r\n\
*                                     测试报告\r\n\
*                         星期%d %02d月%02d日 %02d:%02d:%02d %04d年\r\n\
*	被测机型: %s  BIOS版本: %s 主板版本: %s\r\n\
*\r\n\
********************************************************\r\n\r\n"

#define	MAXID		256
#define	MAXCOUNT	8
//提取到flag.h中
//#define	ENABLE		1
//#define	DISABLE		0

#define FAIL (-1)	//相当于NDK_ERR
#define SUCC 0	//相当于NDK_OK

#define SOH     1
#define STX     2
#define ETX     3
#define EOT     4
#define ENQ     5
#define ACK     6
#define DLE     0x10
#define NAK     0x15
#define ETB     0x17

#define MIN_BTCMD_LEN 14

#if !defined MAX
	#define	MAX(a, b)	(((a)>(b))?(a):(b))
#endif
#if !defined MIN
	#define	MIN(a, b)	(((a)<(b))?(a):(b))
#endif

#if !defined SZ_ARRAY
	#define SZ_ARRAY(x) (sizeof(x)/sizeof(x[0]))
#endif

#if !K21_ENABLE&&!defined ME50NX||defined ME68
#define ISDIGIT(a) (NDK_IsDigitChar(a)==NDK_OK?TRUE:FALSE)	//#define ISDIGIT isdigit		//#define ISDIGIT(a) (a>=0x30 && a<=0x39)
#else
#define ISDIGIT isdigit		//#define ISDIGIT(a) (a>=0x30 && a<=0x39)
#endif

#if CPU5810X_ENABLE||defined ME50NX||defined ME68
#define NDK_TcpBind(a, b, c) NDK_OK  //GPRS不支持
#define NDK_TcpWait(fd) NDK_OK		//GPRS不支持
#endif

#if defined ME50NX||defined ME68
#define NDK_WLCheckSIM NDK_WlCheckSIM
#endif

#if defined ME50N||defined ME50C//||defined ME50N||defined SP610||defined ME50S //全部使用当前路径20170921
#define PARACONF "/appfs/paraconf"
#else
#define PARACONF "paraconf"
#endif

//高端安卓打开动态库的路径 不再使用绝对路径20171010
#if 0//ANDIROD_ENABLE
#define DLIB_PATH "/data/app/com.example.highplattest-2/lib/arm/libjnindk.so"
#endif

//字节序
//这里的ENDIAN主要是指网络地址的字节序,与本地真正的字节序无关.目前POS(包括730)本地都是小数序
//老POS的INET_ADDR,localHost等都是小字序的(tcp_bind等函数内部会再转换为大数序,所以 也无需应用层去转换),
//而730则以网络大数序表示,与网络序保持了一致,省去了应用层字节序的转换工作(当然,代码的移植性是受到怀疑的)
//同时,730的port(小数序表示)字节序转换是在tcpbind,tcpconnect等函数中做的.故应用层也不必考虑转换了.
#if defined DEMO||GP_ENABLE||CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE
	#define	ENDIAN		BIGENDIAN
#else	//||defined NL8510GP要与传统一致,故也按小数序处理
	#define	ENDIAN		LITTLEENDIAN
#endif

#define MIN_SIGNINT		(1<<31)		//(-2147483647-1)
#if !(defined ME15CHX||defined ME15B||defined ME65||defined ME66)
#define MAX_SIGNINT		(~MIN_SIGNINT)	//(2147483647)
#else
#define MAX_SIGNINT		(80000)	//(2147483647)//ME15CHX方案时钟最大值约为89秒
#endif

#define	PCKT0203HEADERLEN		(5)

 //5810X平台产品WiFi数据没有上限故都用8k,中端产品eth wlm wlan都采用8k  20180806 modify  测试WiFi单向性能时使用12k数据会更准确故改成12k测 20180905modify
#define	PACKMAXLEN		(12*1024)	
#if K21_ENABLE  
#define  ETHPACKMAXLEN 	(2*1024) //目前低端只有ME50C支持
#define  WLMPACKMAXLEN 	(2*1024) //5810平台无线2G模块上限是2k,无线4G数据没有上限,暂时先用2k,要测4g模块,可改成8k测试
#else
#define  ETHPACKMAXLEN	 (8*1024)
#define  WLMPACKMAXLEN 	(8*1024)
#endif

/*
//wlan,eth,wlm网络通信模块数据缓冲使用8K，低端2047 
#if !K21_ENABLE
#define	PACKMAXLEN	(8*1024)	//zhangnw20120305提高标准 (5*1024)
#else
#if 0//CPU5810X_ENABLE||defined ME50NX //wifi目前支持3K无线只支持2k 故取二者较小值2K 20171010
#define PACKMAXLEN (8*1024)//5840 //ME50 低端wifi产品支持5840的缓冲大小 无线上TCP层的限制是4K,两者取较小的值
#else
#define PACKMAXLEN (1024*2)//K21平台产品底层堆栈比较小,太大会引起系统崩溃,故改小
#endif
#endif
*/

//kb
#define BTN_OK    	(1<<0)	// 1
#define BTN_CANCEL	(1<<1)	// 2

//自定义键码(NDK未提供)
#include "kb.h"

//统一定义,不同机型的键码差异统一到kb.h
#define DOT			KEY_POINT
#define ESC			KEY_ESC
#define BACKSPACE	KEY_BACK
#define ENTER		KEY_ENTER

#define UPKEY		KEY_UP
#define DOWNKEY		KEY_DOWN
#define LEFT			KEY_LEFT
#define RIGHT		KEY_RIGHT

#if !KB_DISABLE
#define	lib_kbflush	NDK_KbFlush
#endif
#define	lib_getch()	lib_getkeycode(0)

#if SPRINTF_UNSPORT
#define sprintf t_sprintf
#endif

//fs
//#define	MAXFD	(40)	//(0)			//0或-1表示无限制
#define	MAXFNAMELEN	(254)

//mag
#define MAXTRACKLEN 128 /* 目前3个道中最大的数据长度为107*/

#define  TK1					0x08	//只读一磁道
#define  TK2       				0x02		/*只读二道*/
#define  TK3       				0x04		/*只读三道*/
#define  TK1_2_3				( TK1 | TK2 | TK3 )
#define  TK2_3    				(TK2 | TK3)   	/* 0x06  同时读二三道*/
#define  TK1_2				( TK1 | TK2 )   /* 读一二道*/
#define  TK1_3				( TK1 | TK3 )   /* 读一三道*/

#define	 STRIPE		0x01		/*刷磁卡*/

//暂放于此,后续应由NDK.h来提供
// 存放于ErrorCode的系统级错误码
#if K21_ENABLE
enum SYS_ERRORCODE
{
	MAGCARD_TK1_LRC_ERR=0xC01,										// 一道LRC校验失败
	MAGCARD_TK2_LRC_ERR=0xC02,										// 二道LRC校验失败
	MAGCARD_TK3_LRC_ERR=0xC08,										// 三道LRC校验失败
	MAGCARD_TK12_LRC_ERR=MAGCARD_TK1_LRC_ERR|MAGCARD_TK2_LRC_ERR,	// 一、二道LRC校验失败
	MAGCARD_TK23_LRC_ERR=MAGCARD_TK2_LRC_ERR|MAGCARD_TK3_LRC_ERR,	// 二、三道LRC校验失败
	MAGCARD_TK13_LRC_ERR=MAGCARD_TK1_LRC_ERR|MAGCARD_TK3_LRC_ERR,	// 一、三道LRC校验失败
	MAGCARD_TK123_LRC_ERR
		=MAGCARD_TK1_LRC_ERR|MAGCARD_TK2_LRC_ERR|MAGCARD_TK3_LRC_ERR	// 一、二、三道LRC校验失败
};
#endif

//BT指令封装错误码
typedef enum
{
	BTCMD_OK = 0,				//命令设置成功
	BTCMD_SETTIME_ERR = -1,		//设置APK时间延时命令错误
	BTCMD_PARA_ERR = -2,		//参数错误
	BTCMD_ID_ERR = -3,			//未知命令错误
	BTCMD_SN_ERR = -4,			//序列号错误
	BTCMD_DATALEN_ERR = -5,
}EM_BTCMD_ERR;

//关于socket超时时间的说明:
//超时时间最小单位是1tick,timeo表示tick数.不同机器的tick值不同.多数产品为20ms(推荐的timeo为1000,即20s(标准的)),GP730可能为5.95ms......
//linux下可通过设置socket选项来设置超时,但输入的是struct timeval的地址(事先可以设置结构的成员sec或usec),即linux可以精确控制到s或us
//一般地,我们只要使得超时效果在10s(20?10?6?)左右即可.另,无线条件下,连接的时间约要6s才成功(留10s一般够),断开也要至少6s才会看到第4个报文
#if 1	//defined DEMO||defined SP730
	#define	SO_TIMEO			(40)//(20)	//(1600) //730上设置同一个TIMEO
#else
	#define	SO_TIMEO			(1600)	//(500) 
#endif

#if 0 //20130811移到typedef.h中
//ssl
#if !K21_ENABLE
	#define	NDKSSL_ENABLE	ENABLE	//K21平台不支持
#else
	#define	NDKSSL_ENABLE	DISABLE
#endif
#endif
//util

//链路默认设置
#define	DEFAULT_MDM_DIAL_STR		"016300"	//异步上网的拨号串,实为16300,首个0是公司拔外线要加拔的
#define	DEFAULT_ASYN_USR			"16300"
#define	DEFAULT_ASYN_PWD			"16300"
#define	DEFAULT_BPS				"115200"	//"115200,8,N,1"	//BPS115200
#define	DEFAULT_WLM_APN			"CMNET"
#define	DEFAULT_WLM_USR			"card"
#define	DEFAULT_WLM_PWD			"card"
#define	DEFAULT_PPP_USR			DEFAULT_WLM_USR
#define	DEFAULT_PPP_PWD			DEFAULT_WLM_PWD
#define	DEFAULT_WLAN_PWD		"12345678910"
#define	DEFAULT_WLAN_ESSID		"AP2&4same"  //"Netcore"		//"TP-LINK_four"	//"dlink615"
#define	DEFAULT_WLAN_8021XIDENTITY		"TESTIDENTITY" 
#define	DEFAULT_BT_PIN		"123456"  
#define	DEFAULT_BT_NAME		"BT_TEST" 

//wifi mac
//#define	DEFAULT_WIFI_MAC		{0x11,0x12,0x13,0x14,0x15,0x16} //Wifimac

//wifi ap 默认ssid 和 key
#define	DEFAULT_WLANAP_SSID		"AP_SSID"  
#define	DEFAULT_WLANAP_KEY		"0123456789"  

//网络默认设置
#define	DEFAULT_SVR_IP_STR				"218.66.48.230"//"192.168.1.158"  //"218.66.48.230"	//"192.168.2.173"
#define	DEFAULT_SVR_TCP_PORT				(3459)//(3458)
#define	DEFAULT_SVR_UDP_PORT				(3459)
#define	DEFAULT_LOCAL_IP_STR				"192.168.5.232"
#define	DEFAULT_LOCAL_MASK_STR			"255.255.255.0"
#define	DEFAULT_LOCAL_GATEWAY_STR		"192.168.5.254"

#define	DEFAULT_WIFIAP_IP_STR				"192.168.123.1"  

///////////////////////////以下特别处理按产品进行分类处理,暂不归入以上的模块处理//////////////////
//注意:由于不同产品的同一接口的返回值还可能存在差异,故这类接口应根据产品形态定义为空或SUCC/FAIL等,而不可一味地都定义为FAIL等


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NODEBUG
	#define ASSERT(exp) ((void)0)
#else
	#define ASSERT(exp) \
	do\
	{\
		if (!(exp))\
		{\
	 		cls_show_msg("%s, line %d: ASSERTION failed: "#exp,  __FILE__, __LINE__);NDK_SysReboot();\
		}\
	}while(0)
#endif

#define TRACE_INT(i)	cls_show_msg("%s, line %d:"#i"=%d", __FILE__, __LINE__, i)
#define	DEBUG_HEX(data, datalen, tip) \
{\
int i = 0;\
if(datalen)\
{\
	cls_printf("%s, line %d:%s ", __FILE__, __LINE__, (tip!=NULL)?tip:"");\
	for(; i<datalen; i++)\
		NDK_ScrPrintf(" %02X", *((char *)data+i));\
	NDK_ScrRefresh();\
	lib_getch();\
}\
}

/*----------global variables declaration("extern")--------------*/
//lib.c
extern int g_UCID;
extern char NowSystestName[64];//综合测试以文件方式记录测试错误引入
extern int g_SequencePressFlag;
extern int auto_flag;
extern int packetnum;
extern int g_wifistopflag;
extern int g_PackLen;
#if ANDIROD_ENABLE  //JNI调用NDK接口方案,显示相关函数都是通过动态调用上层的函数
extern int (*cls_show_msg)(const char* msg, ...);
extern int (*cls_show_msg1)(int time, const char* msg, ...);
extern int (*ShowMessageBox)(char* pMsg, char cStyle, int iWaitTime);
extern int (*cls_printf)(const char* msg, ...);
extern int (*lib_kbgetinput)(char * pszBuf,uint unMin,uint unMaxLen,uint * punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl);
extern int (*lib_getkeycode)(uint sec);
extern void (*send_result)(char *fmt, ...);
extern int (*NDK_ScrPrintf)(const char *psFormat, ...)	;
extern int (*NDK_ScrRefresh)(void);
extern int (*NDK_ScrClrs)(void);
extern int (*cls_show_msg_record)(char* filename, char* funname,const char* msg, ...);
extern int (*cls_show_msg1_record)(char* filename, char* funname,int time, const char* msg, ...);
#endif

//conf.c
#if PRN_ENABLE
#if PRNGREY_ENABLE
extern uint g_PrnGrey;
#endif
#if PRNUNDERLINE_EN
extern EM_PRN_UNDERLINE_STATUS g_PrnUnderline;
#endif
extern uint g_PrnSwitch;
#endif
#if BT_ENABLE
extern int g_at_enable;
extern int g_pair_mode;
extern int g_btdiscoverflag;
#endif
extern uint g_conf_flag;//参数配置文件使用标志位
#if SDLC_ENABLE||ASYN_ENABLE
extern int sdlc_init_flag;
extern int g_sdlc_bps;
#endif
extern int g_ScanType;
#if CAMERA_ENABLE
extern int g_ShowImage;//摄像头的相关参数
extern int g_Saturation;
extern int g_Sharpness;
extern int g_Contrast;
#endif
#if SCAN_ENABLE||SCANASYCN_ENABLE
extern int g_Floodlight;//扫描头的相关参数
extern int g_Focuslight;
extern int g_Sensitivity;
#endif

//layer.c
extern LinkSetting	linksetting;
extern NetworkSetting	netsetting;
extern sock_t g_socktype;
extern int TCPRESETFLAG;

//方便使用的几个变量别名(为了兼容历史代码)
#define	g_com				linksetting.snd_com
#define	send_flag			linksetting.snd_flag
#define	linktype			linksetting.lnktype
#define	g_bps				linksetting.lnkbps
#define	MDMPatchType		linksetting.WMPatchType
#define	MDMDialStr			linksetting.WMDialStr
#define	WLMAPN				linksetting.WLMApn
#define	PPPUSR				linksetting.PPPUsr
#define	PPPPWD				linksetting.PPPPwd
#if DOUBLESIM_ENABLE
#define SIMSLOT				linksetting.Simslot
#endif
#define	g_devicecom			linksetting.devicecom
#define	g_devicecombps		linksetting.devicecombps
#define	WlanEncMode			linksetting.WLANEncMode
#define	WlanKeyType			linksetting.WLANKeyType
#define	WlanPwd				linksetting.WLANPwd
#define	WlanEssid			linksetting.WLANEssid
#define	Wlan8021XEapMode	linksetting.WLAN8021XEapMode
#define	Wlan8021XIdentity	linksetting.WLAN8021XIdentity
#define	BtPin				linksetting.BTPin
#define	BtName				linksetting.BTName
//简单wifi流程需要用到ap的MAC地址
#define WifiMac				linksetting.WIFIMac
#define WifiMacFlag			linksetting.WIFIMacFlag
//wifi 隐藏AP标志位
#define WifiHiddenFlag		linksetting.WIFIHiddenFlag

//wifi ap 功能新增参数
#define	WifiApShareDev		linksetting.WIFIApShareDev
#define	WifiApSecMode		linksetting.WIFIApSecMode
#define	WifiApSsid			linksetting.WIFIApSsid
#define	WifiApHidden		linksetting.WIFIApHidden
#define	WifiApKey			linksetting.WIFIApKey

#if (MULTIAPP==2)
	extern const int _g_nFramLen;
	extern const char _g_sMachineType[2+1];
	extern const unsigned char _g_nSerio;
	extern const char _g_sVer[15+1];
	extern const char _g_sAppName[16+1];
#endif

/*---------------functions declaration--------------------------*/
#if defined _CTYPE_INIT_SUPPORT
int _ctype_init(void);
#endif

//lib.c
#if !ANDIROD_ENABLE
void send_result(char *fmt, ...);
#endif
int select_test(unsigned char select_id[], int cnt);
int	process(PF *list,  unsigned char id[]);
int jump2page(int maxpage, int *curpage);
void auto_test(int *auto_flag, int *nKeyin, int *SelPage, int maxpage, const char *tcrmenu[], const signed char *skip_modid);
void send_reporthead(void);
#if PRN_ENABLE
int print_Ex(char *buf, ...);
int lib_getprinterlen(void);
int lib_getprintertype(void);
#endif
char __check_stripe_result_tk23(char *track, char *name);
char check_stripe_result_tk2(char *track2);
char check_stripe_result_tk3(char *track3);
char check_stripe_result_tk1(char *track1);
#if !defined READCARD3_SUPPORT
int readcard3(/*int type, */int tracks, /*int keylen, */int waittime, char *track2, char *track3 , char *track1, int *err);
#endif
//#define lib_StartStopwatch NDK_SysStartWatch
void lib_StartStopwatch(void);
float lib_ReadStopwatch(void);
#if defined ME30GX||defined ME32GX	
int INT_ReadStopwatch(void);
#endif
#if !ANDIROD_ENABLE
int lib_kbgetinput(char * pszBuf,uint unMin,uint unMaxLen,uint * punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl);
#endif
void ftos(float f, char *str);
int lib_bt_pairing(uint unaccept);
void update_BT_packet(char *buf, int len);
int bt_cmd_packet(int cmdid, char *data, int datalen, char* cmd);
int bt_cmd_uppacket(char* cmd, int datalen, char *data);
int bt_comm_once(int packlen, int randflag);
#if BT_MASTER_ENABLE
int lib_btmaster_scan(char *pszMac, int dectype);
int lib_btmaster_pairing(int accept);
int lib_btmaster_getconstatus(char *pszMac, int pnStatus);
#endif
int touchscrean_getnum(void);
#if NEW_WIFI_ENABLE	//new wifi
int lib_getwifistatus(ST_WIFI_STATUS *pstStatus);
#endif
int GetWifiStopflag(void);
int SetWifiStopflag(int stopflag);
#if NEW_WIFI_ENABLE||defined SP610	//tcp check
int lib_tcp_checkstatus(uint unFd, int maxtime);
#endif
#if SPRINTF_UNSPORT
void t_printchar(char **str, int c);
int t_prints(char **out, const char *string, int width, int pad);
int t_printi(char **out, int i, int b, int sg, int width, int pad, int letbase);
int t_print( char **out, const char *format, va_list args );
int t_printf(const char *format, ...);
int t_sprintf(char *out, const char *format, ...);
int t_snprintf( char *buf, unsigned int count, const char *format, ... );
#endif
int lib_doscan(int asycn, int maxwaittime);
#if ANDIROD_ENABLE
int  dload_library(void);
#endif

//conf.c
int select_serial(void);
char *conf_serial(int *bpsid);
void conf_prn(void);
int lib_initprn(uint unSwitch);
//int conf_icsam_pps(void);
//void conf_readcard3(void);
int PppSetUsrCfg(void);
int conf_link(LinkType type);
int conf_local(void);
int conf_network(void);
int conf_trans(void);
int conf_conn_MDM(void);
int conf_conn_PPP(void);
int conf_conn_ETH(int IsTransConfed);
int conf_conn_sock(void);
int conf_conn_WLM(int IsCnncted);
int conf_conn_DNS(void);
void conf_WLAN_lnk1(void);
int conf_conn_WLAN(void);
int conf_wifi_AP(void);  
int conf_conn_AP(void);
void set_disktype(int type);
int get_disktype(void);
int conf_disktype(void);
int conf_conn_BT(void);
int conf_bt(void);
void conf_nfc_datatype(void);
#if NFC_ENABLE
extern int (*Nfc_Activate)(uchar *psRecebuf,int *pnRecvlen,int nSeekCnt);
#endif
void conf_camera(void);
void conf_scan(void);
int use_wifi_conf(int ApID);
int conf_server(void);

//layer.c
int serial_clrportbuf_all(EM_PORT_NUM port);
void init_linksetting(void);
void init_netsetting(void);
void setlinktype(LinkType type);
LinkType getlinktype(void);
void set_linkstatus_down(void);
void set_netstatus_down(void);
ENDIAN_t GetCurrentEndianType(void);
ulong INET_ADDR(char *a);
void INET_NTOA(ulong n, char *a);
unsigned short HTONS(unsigned short data);
unsigned long HTONL(unsigned long data);
void create_0203pack(char *buf, int tlen);
int mdm_detectCONNECT(void);
int mdm_dial(char *phonenum);
int mdm_hangup(void);
void update_SDLC_packet(char *buf, int len);
int getmodemreadlenN(void);
void update_ASYN_packet(char *buf, int len);
int ASYNDial4PPP(void);
int WM_PPP_16300_DIAL(void);
int lib_GetLocalAddr(char *ipAddr, char *subnet, char *gateway, char *dns);
int NDKTransReceiveN(uint s, char *buf, uint len, uint timeout);
int NDKTransSendN(uint s, char *buf, uint len, uint timeout);
int NDKSSLReceiveN(NDK_HANDLE handle, void *pBuffer, /*const */size_t SizeOfBuffer/*, size_t *recv_data*/);
int NDKSSLSendN(NDK_HANDLE handle, /*const */char *pBuffer, size_t SizeOfBuffer/*, size_t *sent_data*/);
int WaitPPPState(EM_PPP_STATUS state, int *err);
//int WaitWLANState(char *pEssid, int state);
int WaitNDKSSLState(NDK_HANDLE sh, int state, int timeout);
int LinkUp4AsynPPP(void);
int LinkUp(void);
int LinkDown(void);
int NetUp(void);
int NetDown(void);
int TransUp(uint *s);
int TransDown(uint s);
int IsNetConnected(void);

int __TransUp(uint *s);
int __TransDown(uint s);
int __LinkUp_WLAN(void);
int __LinkDown_WLAN(void);
int __NetUp_WLAN(void);
int __NetDown_WLAN(void);
int __LinkUp_ETH(void);
int __LinkDown_ETH(void);
int __NetUp_ETH(void);
int __NetDown_ETH(void);
int __LinkUp_WLM(void);
int __LinkDonw_WLM(void);
int __NetUp_PPP(void);
int __NetDown_PPP(void);
int __LinkUp_ASYN(void);
int __LinkDown_ASYN(void);
int __LinkUp_SDLC(void);
int __LinkDown_SDLC(void);

//tools.c
#if K21_ENABLE
time_t time(time_t *calptr);
#endif
void Beep(int sec);
#if KB_DISABLE
int lib_kbflush(void);
#endif
int lib_kbhit(void);
#if !ANDIROD_ENABLE
int lib_getkeycode(uint sec);
#endif
int wait_anykey(int time);
int is_digit_str(char *str);
int MemCmp(char *sbuf, char *rbuf, int len);
void debug_buf(char *bufname, char *bufaddr, int buflen);
uchar MakeLRC(uchar *buf, int len);
uchar Data2BCD(int data);
int IsContinuous(int *cur, int *last);
//查找配置文件的几个函数
int FGetC(int nFp);
char * FGets(char *pszBuf, int nMaxLen, int nFp);
int GetParaInfo(char *ConfName, char *pInfo);
int trim_string2int(char *inbuf, int *numbuf);
int trim_string(char *inbuf, char *outbuf);
void trim_left_string(char *inbuf, char *outbuf);
int trim_char2uchar(char *inbuf, uchar *outbuf);

#endif

