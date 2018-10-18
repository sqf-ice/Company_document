/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 类型定义
* file name		: typedef.h
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20060704
* directory 		: .\inc\
* description		: 
* related document	: TCR690 API测试程序 详细设计说明书.doc
* 
************************************************************************
* log			: 
* Revision 1.0  20060704 14:34 zhangnw
* created
************************************************************************/
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
typedef void (*PF)(void);
typedef void* (*PTHREAD)(void*);

#if 0	//已由应用开发环境提供
#if !defined UCHAR
	#define UCHAR	unsigned char
#endif
#if !defined SCHAR
	#define SCHAR	signed char
#endif
#if !defined USHORT
	#define USHORT	unsigned short
#endif
#if !defined UINT
	#define UINT		unsigned int
#endif
#if !defined ULONG
	#define ULONG	unsigned long
#endif

#if !defined uchar
	#define uchar	UCHAR
#endif
#if !defined schar
	#define schar	SCHAR
#endif
#if !defined ushort
	#define ushort	USHORT
#endif
#if !defined uint
	#define uint		UINT
#endif
#if !defined ulong
	#define ulong	ULONG
#endif
#else
typedef uchar UCHAR;
typedef ushort USHORT;
typedef uint UINT;
typedef ulong ULONG;
#endif

/**	布尔型枚举*/
typedef enum __Bool{FALSE, TRUE}Bool;

/**字节序定义*/
typedef enum __ENDIAN_t{LITTLEENDIAN, BIGENDIAN}ENDIAN_t;

//自定义几个拔号的状态机值
/**	MDM状态枚举*/
//typedef enum __ModemStatus{INITIAL, INITED, PREDIALED, DIALED}ModemStatus;
/**	链路状态枚举*/
typedef enum __LinkStatus{LINKUP, LINKDOWN}LinkStatus;
/**	网络状态枚举*/
typedef enum _NetStatus{NETUP, NETDOWN}NetStatus;
/**	传输层状态枚举*/
typedef enum __TransStatus{TRANSUP, TRANSDOWN}TransStatus;

/**通讯方式*/
typedef enum __LinkType{NONE, GPRS, CDMA, TD, ASYN, SYNC, ETH, SERIAL, WLAN, BT}LinkType;

/*网络模式*/
typedef enum __Network_t{second_G,third_G,fourth_G}Network_t;

/**SOCK类型,从-1开始*/
typedef enum __sock_t{SOCK_DEFAULT=-1, SOCK_NDKTCP=0, SOCK_NDKUDP=1, SOCK_NDKSSL=2}sock_t;

typedef enum __SMART_t{CPU_A, CPU_B, /*HS_B, */MIFARE_1, FELICA, MIFARE_0,  ISO15693, /*RFID_SIM, */SAM, IC}SMART_t;

typedef enum __MEMORY_t{AT24C01,AT24C02,AT24C04,AT24C08,AT24C016,AT24C032,AT24C064,SLE4432_42,SLE4418_28,SLE5528,AT88SC102,AT88SC1604,AT88SC1608,AT88SC153}MEMORY_t;

typedef struct __LinkSetting
{
	int snd_com;
	int snd_flag;
	LinkType lnktype;
	char lnkbps[8];//int lnkbps;//一般串口bps,不是g_com的速率.g_com的BPS是定死为115200的
	int WMPatchType;//int WMCountry;//zhangnw20110427add
	char WMDialStr[16];
	char WLMDialStr[16];
	char WLMApn[64];
	char PPPUsr[8];
	char PPPPwd[8];
	int Simslot;//双SIM卡卡槽参数
	int devicecom;
	char devicecombps[8];//int devicecombps;//不与lnkbps复用,以免其它用例改变了值(且没有备份)
	int WLANEncMode;
	int WLANKeyType;
	//char WLANPwd[16];
	char WLANPwd[65];//路由器最大允许设置64字节密码长度,相应修改测试案例支持linwl20150819
	char WLANEssid[128];//T_ApInfo WLANAp;
	int WLAN8021XEapMode;//802.1x认证模式
	char WLAN8021XIdentity[128];//802.1x认证用户名
	char BTPin[7];//蓝牙密码
	char BTName[16];//蓝牙名称

	//wifi mac和设置标志
	uchar WIFIMac[6];
	int WIFIMacFlag;
	int WIFIHiddenFlag;
	
	int WIFIApShareDev;//WiFi热点接入的网络
	int WIFIApSecMode;//WiFi热点安全加密模式
	char WIFIApSsid[33];//WiFi热点ssid
	uchar WIFIApHidden;//是否隐藏ssid
	char WIFIApKey[65];//WiFi热点密码

}LinkSetting;

typedef union __Value
{
	ulong	data;
	uchar	asc[4];
}Value;

typedef struct __IPAddr	//IPv4
{
	char Str[16];
	Value value;
}IPAddr;

//ssl  //提取到flag.h中
/*#if CPU5892_ENABLE||OVERSEAS_ENABLE
	#define	NDKSSL_ENABLE	0   //ENABLE   20140822 ssl 在5.0版本中暂时去掉
#else
	#define	NDKSSL_ENABLE	0	//DISABLE
#endif*/

typedef struct __NetworkSetting
{
	IPAddr	SvrIP;
	ushort 	SvrTcpPort;
	ushort	SvrUdpPort;
	IPAddr	LocalIP;
	ushort	LocalPort;
	IPAddr	LocalMask;
	IPAddr	LocalGateway;
	sock_t	socktype;
#if NDKSSL_ENABLE
	EM_SSL_HANDSHAKE_OPT SSLHsOpt;
	EM_SSL_AUTH_OPT SSLAuthOpt;
	EM_SSL_FILE_FORMAT SSLFileFmt;
#endif
	char	DNSHost[3][16];
	int		DHCPenable;//zhangnw20111109add
	//char 	Reserved[8];

	IPAddr	WifiApIp;
}NetworkSetting;

typedef struct __Packet
{
	char	*header;
	int	orig_len;//起始长度
	int 	len;//实际长度
	int 	lifecycle;
	int 	forever;//本标志为TRUE时,lifecycle不起作用(包长生不老)
	int	IsLenRec;
	int	IsDataRnd;
}Packet;

#ifndef	C_class
	#define C_class struct 
#endif

typedef struct _KeyTable
{
	char* keyname;
	int keycode;
}KeyTable;

//各产品个性化的类型

//end

/*------------global variables definition-----------------------*/

/*---------------structure definition---------------------------*/

#endif

